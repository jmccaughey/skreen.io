#include <arpa/inet.h>

#include "X11ScraperPoster.h"

#include "Trace.h"

extern "C" {
    #include "md5.h"
    #include "jpeglib.h"
    #include "jpeg_mem_dest.h"
}

const char LOG[] = "skreen.io_scrape.log";

void s_trace(const char *msg) {
    //trace(LOG, msg);
    printf("%s\n", msg);
}

X11ScraperPoster::X11ScraperPoster(
    ScraperPosterMetaData* pScraperPosterMetaData) {
    s_trace("X11ScraperPoster ctor");
    s_trace(__DATE__);
    s_trace(__TIME__);
    sentHashes = new std::set<HashString>();//new HashTree();
    subtileMap = new std::map<HashString, SubtileInfo>();
    screen = new PlatformScreen();
    screenWidth = screen->getWidth();
    screenHeight = screen->getHeight();
    poster = NULL;
    regions = NULL;
    mSendKeyframe = true;
    this->pScraperPosterMetaData = pScraperPosterMetaData;
    const char* servlet = "/scrapes";
    buildUrlFromMetaData(pScraperPosterMetaData, postUrl, servlet);
    s_trace(postUrl);
    memset(mSendBuffer, 0, SEND_BUFFER_LENGTH);
    printf("set memory\n");    
    gifSpriter = NULL;
    std::string url("http");
    if(pScraperPosterMetaData->getSsl()){
    	url += "s://";
    }else{
    	url += "://";
    }
    url += pScraperPosterMetaData->getHost();
    url += ":";
    char port[256];
    sprintf(port, "%d", pScraperPosterMetaData->getPort()); 
    url += port;
    url += postUrl;
    strcpy(postUrl, url.c_str());
    this->poster = new PlatformPoster();
	initRegions();
	mShutting = false;  
	millisElapsed = 0;                           
}
X11ScraperPoster::~X11ScraperPoster() {
    s_trace("X11ScraperPoster dtor");
    //stopScraping();
    if(screen){
        delete screen;
        screen = NULL;
    }
    if (this->poster) {
        delete (this->poster);
        this->poster = NULL;
        cleanUpHash();
    }
    if(gifSpriter) delete gifSpriter;
    for(int i = 0; i < regionCount; i++){
    	delete regions[i];
    }
    delete[] regions;
}
void X11ScraperPoster::buildUrlFromMetaData(
    ScraperPosterMetaData* pScraperPosterMetaData, char* buffer,
    const char* servlet) {
    char contextPart[POST_URL_BUFFER_LENGTH];
    if (strlen(pScraperPosterMetaData->getContext())) {
        sprintf(contextPart, "%s/", pScraperPosterMetaData->getContext());
    } else {
        contextPart[0] = 0;
    }
    sprintf(buffer, "%s%s?blastId=%s&blasterId=%s&key=%s", contextPart,
             servlet, pScraperPosterMetaData->getBlastId(),
             pScraperPosterMetaData->getBlasterId(),
             pScraperPosterMetaData->getKey());
}
void X11ScraperPoster::doScreenShot() {
    screen->doScreenShot();
}
void X11ScraperPoster::sleepAndUpdateTicks(){
	unsigned long millis = getSleepTime();
	if(millis > 0){
		printf("sleeping %lu...\n", millis);	
		nanosleep((struct timespec[]){{0, millis * 1000000}}, NULL);
		printf("...unsleeping\n");
	}else{
		printf("not sleeping\n");
	}
	struct timespec ts;
	if(clock_gettime(CLOCK_MONOTONIC, &ts) != 0) {
 		printf("error getting ticks\n");
	}else{
		millisElapsed = (ts.tv_sec*1000) + (ts.tv_nsec/1000000);
		printf("millisElapsed = %lu\n", millisElapsed);
	}
}
unsigned long X11ScraperPoster::getSleepTime(void) {
    if (millisElapsed == 0){
        return 0;
    }
    struct timespec ts;
    unsigned long nowMillis;
    if(clock_gettime(CLOCK_MONOTONIC, &ts) != 0) {
 		printf("error getting ticks\n");
 		return SCRAPE_THROTTLE_MS;
	}else{
		nowMillis = (ts.tv_sec*1000) + (ts.tv_nsec/1000000);
	}
    unsigned long diff = nowMillis - millisElapsed;
    printf("diff millis = %lu\n", diff);
    if (diff < 0 || diff > SCRAPE_THROTTLE_MS) {
        return 0;
    } else {
        return SCRAPE_THROTTLE_MS - diff;
    }
}
void X11ScraperPoster::doScreenScrape() {
	sleepAndUpdateTicks();
    printf("about to doScreenShot\n");
    doScreenShot();
    printf("returned from doScreenShot\n"); 
    printf("send key frame: %d\n", this->mSendKeyframe);
    printf("region count: %d\n", this->regionCount);
    int changeCount = this->mSendKeyframe ? this->regionCount : this->checkForChanges();
    int writtenCount = 0;
    bool mouseMoved = screen->updateMousePosition(mouseX, mouseY);
    if (changeCount > 0 || mouseMoved) {
        int cursor = 0;
        cursor += writeHeader(changeCount);
        printf("announced %d changed tiles\n", changeCount);
        char hashBuffer[HASH_LENGTH_BYTES];
        // MD5 all tiles, check if already sent
        std::vector<deskblast::Region*> newRegions;
        std::map<HashString, SubtileInfo>::iterator it;
        std::map<deskblast::Region, HashString> rectHashes;
        std::map<deskblast::Region, HashString>::iterator rectHashesIt;
        for (int k = 0; k < this->regionCount; k++) {
            if (this->regions[k]->isChanged() || this->mSendKeyframe) {
                hashPixels((void*)(screen->getCurrentBuffer()), this->regions[k], (char*)hashBuffer);
                HashString hs(hashBuffer);
                it = subtileMap->find(hs);
                if (it != subtileMap->end()) {
                    // this subtile has been sent before so write it as back ref
                    SubtileInfo found = it->second;
                    int hashOffset = cursor;
                    found.copyTo((char*)mSendBuffer, cursor);
                    cursor += (28 + HASH_LENGTH_BYTES); // 3 (7) ints (4 bytes each) plus hash
                    cursor = compressIfNeededAndWriteTile(this->regions[k], cursor, hashOffset, found.getX(), found.getY(), false);
                    writtenCount++;
                } else {
                    newRegions.push_back(this->regions[k]);
                    printf("inserting %s into rectHashes\n", hs.getHash());
                    rectHashes.insert(std::pair<deskblast::Region,HashString>(*(this->regions[k]), hs));
                }
            }
        }
        printf("%d backref regions\n", writtenCount);
        printf("%lu new regions\n", newRegions.size());
        if(!this->gifSpriter){
            this->gifSpriter = new GifSpriter(screenWidth, screenHeight);
        }
        std::vector<deskblast::Region*> ungiffableRegions;
        std::vector<GifSpriteInfo*> gifSpriteInfos;

        // TODO: note the hash of the upload before we know it succeeded
        // so if it is interrupted, we find it among the 'sent' uploads but it
        // never actually uploaded
        unsigned int newRegionCount = newRegions.size();
        gifSpriter->makeSprites(&newRegions, (unsigned char*) screen->getCurrentBuffer(), &gifSpriteInfos, &ungiffableRegions);
        newRegions.clear();
        printf("%lu ungiffable regions\n", ungiffableRegions.size());
        printf("%lu GifSpriteInfos\n", gifSpriteInfos.size());
        unsigned int mappedRegionCount = 0;
        for(unsigned int i = 0; i < gifSpriteInfos.size(); i++){
            mappedRegionCount += gifSpriteInfos.at(i)->getMappedRegions()->size();
        }
        if(mappedRegionCount + ungiffableRegions.size() != newRegionCount){
            printf("PROBLEM: mappedRegionCount (%d) plus ungiffables (%lu) does not equal new region count (%d)\n",
                    mappedRegionCount, ungiffableRegions.size(), newRegionCount);
        }
        for(unsigned int i = 0; i < gifSpriteInfos.size(); i++){
            GifSpriteInfo* gsiPtr = gifSpriteInfos.at(i);
            hashBytes(gsiPtr->getGifStart(), gsiPtr->getGifLength(), (char*)hashBuffer);
            HashString hs(hashBuffer);
            boolean spriteSent = false;
            printf("gifspriteinfo # %d has %lu mapped regions\n",
                    i, gifSpriteInfos.at(i)->getMappedRegions()->size());
            for(unsigned int j = 0; j < gifSpriteInfos.at(i)->getMappedRegions()->size(); j++){
                MappedRegion mr = gifSpriteInfos.at(i)->getMappedRegions()->at(j);
                if (spriteSent || this->sentHashes->count(hs)) {
                    int hashOffset = cursor;
                    hs.copyTo((char*)mSendBuffer, cursor);
                    cursor += (28 + HASH_LENGTH_BYTES); // 3 (7) ints (4 bytes each) plus hash
                    cursor = compressIfNeededAndWriteTile(mr.getRegion(), cursor, hashOffset,
                                    mr.getTranslateX(), mr.getTranslateY(), false);
                    writtenCount++;
                }else{
                    cursor = writeCompressedTile(mr.getRegion(), cursor, &hs, mr.getTranslateX(), mr.getTranslateY(),
                            gsiPtr->getGifStart(), gsiPtr->getGifLength());
                    writtenCount++;
                    this->sentHashes->insert(hs);
                    printf("inserted gif %s into sentHashes\n", hs.getHash());
                    spriteSent = true;
                }
                rectHashesIt = rectHashes.find(*(mr.getRegion()));
                SubtileInfo si(hs.getHash(), mr.getTranslateX(), mr.getTranslateY());
                this->subtileMap->insert(std::pair<HashString, SubtileInfo>(rectHashesIt->second, si));
            }
        }
        for(std::vector<GifSpriteInfo*>::iterator it = gifSpriteInfos.begin() ; it != gifSpriteInfos.end(); ++it){
        	delete *it;
        }

        std::vector<deskblast::Region> mergedRegions;
        for (unsigned int i = 0; i < ungiffableRegions.size(); i++) {
            mergedRegions.push_back(*(ungiffableRegions[i]));
        }
        deskblast::Region::doMergeRegions(mergedRegions, deskblast::Region::TILE_WIDTH * deskblast::Region::TILE_HEIGHT * deskblast::Region::MAX_MERGED_TILES);
        char logBuff[512];
        sprintf(logBuff, "%lu regions merged to %lu", ungiffableRegions.size(), mergedRegions.size());
        s_trace(logBuff);
        for(unsigned int i = 0; i < ungiffableRegions.size(); i++){
            printf("ungiffable x: %d y: %d w: %d h: %d [%d]\n",
                    ungiffableRegions[i]->getX(), ungiffableRegions[i]->getY(),
                    ungiffableRegions[i]->getWidth(), ungiffableRegions[i]->getHeight(), i);
        }
        for (unsigned int i = 0; i < mergedRegions.size(); i++) {
            hashPixels(screen->getCurrentBuffer(), (deskblast::Region*)(&mergedRegions[i]), (char*)hashBuffer);
            HashString hs(hashBuffer);
            if (this->sentHashes->count(hs)) {
                printf("this merged region found in sentHashes\n");
                for (unsigned int m = 0; m < ungiffableRegions.size(); m++) {
                    if (mergedRegions[i].contains(*(ungiffableRegions[m]))) {
                        int hashOffset = cursor;
                        hs.copyTo((char*)mSendBuffer, cursor);
                        cursor += (28 + HASH_LENGTH_BYTES); // 3 (7) ints (4 bytes each) plus hash
                        cursor = compressIfNeededAndWriteTile(ungiffableRegions[m], cursor, hashOffset,
                                   ungiffableRegions[m]->getX() - mergedRegions[i].getX(), ungiffableRegions[m]->getY() - mergedRegions[i].getY(),
                                                              false);
                        writtenCount++;
                    }
                }
            } else {
                printf("not found; adding jpeg to sentHashes %s\n", hs.getHash());
                this->sentHashes->insert(hs);
                boolean spriteSent = false;
                for (unsigned int m = 0; m < ungiffableRegions.size(); m++) {
                    if (mergedRegions[i].contains(*(ungiffableRegions[m]))) {
                        printf("mergedRegion x: %d y: %d w: %d h: %d contains region x: %d y: %d w: %d h: %d\n",
                                mergedRegions[i].getX(), mergedRegions[i].getY(), mergedRegions[i].getWidth(), mergedRegions[i].getHeight(),
                                ungiffableRegions[m]->getX(), ungiffableRegions[m]->getY(),
                                ungiffableRegions[m]->getWidth(), ungiffableRegions[m]->getHeight());
                        if (!spriteSent) {
                            int hashOffset = cursor;
                            hs.copyTo((char*)mSendBuffer, cursor);
                            cursor += (28 + HASH_LENGTH_BYTES); // 3 (7) ints (4 bytes each) plus hash
                            printf("posting jpeg %s cursor: %d\n", hs.getHash(), cursor);
                            cursor = compressIfNeededAndWriteTile(&(mergedRegions[i]), cursor, hashOffset,
                                                                  ungiffableRegions[m]->getX() - mergedRegions[i].getX(),
                                                                  ungiffableRegions[m]->getY() - mergedRegions[i].getY(),
                                                                  true);
                            writtenCount++;
                            printf("done posting jpeg %s cursor: %d\n", hs.getHash(), cursor);
                            spriteSent = true;
                        } else {
                            int hashOffset = cursor;
                            hs.copyTo((char*)mSendBuffer, cursor);
                            cursor += (28 + HASH_LENGTH_BYTES); // 3 (7) ints (4 bytes each) plus hash
                            printf("not posting jpeg %s\n", hs.getHash());
                            cursor = compressIfNeededAndWriteTile(ungiffableRegions[m], cursor, hashOffset,
                                                                  ungiffableRegions[m]->getX() - mergedRegions[i].getX(),
                                                                  ungiffableRegions[m]->getY() - mergedRegions[i].getY(),
                                                                  false);
                            writtenCount++;
                        }
                        rectHashesIt = rectHashes.find(*(ungiffableRegions[m]));
                        SubtileInfo si(hs.getHash(),
                                       ungiffableRegions[m]->getX() - mergedRegions[i].getX(),
                                       ungiffableRegions[m]->getY() - mergedRegions[i].getY());
                        this->subtileMap->insert(std::pair<HashString, SubtileInfo>(rectHashesIt->second, si));
                    }
                }
            }
            //}
        }
//        sprintf(logBuff, "compressing %d tiles took %d milliseconds",
//                this->regionCount, (int)(GetTickCount() - start));
        //s_trace(logBuff);
        this->mSendKeyframe = false;
        if (!mShutting) {
            char response[RESPONSE_BUFFER_LENGTH];
            int responseLength = 0;
            printf("about to post %d bytes\n", cursor);
            bool result = this->poster->sendPost(postUrl, (char*)mSendBuffer, cursor, response,
                                                 RESPONSE_BUFFER_LENGTH, &responseLength);
            //writeScrapeToFile(mSendBuffer, cursor);
            //fprintf(this->mlogfile, "...done posting:%d", result);
            //fflush(this->mlogfile);
            if (result) {
                char* bytes = (char*) response;
                char command = bytes[0];
                //this->ticksLastCommand = GetTickCount();
                //fprintf(this->mlogfile, "got data %d", command);
                //fflush(this->mlogfile);
                this->mContinueScraping = (command == 1 || command == 2);
                this->mSendKeyframe = (command == 2);
            } else {
                this->mContinueScraping = false;
                s_trace("sendPost failed");
                //fprintf(this->mlogfile, "got stop command");
                //fflush(this->mlogfile);
            }
        }
        if(writtenCount != changeCount){
            printf("changeCount: %d writtenCount: %d\n", changeCount, writtenCount);
        }
    } else {
        if (true){//this->commandExpired()) {
            this->mSendKeyframe = true;
            //fprintf(this->mlogfile, "command expired -- forcing keyframe");
        }
        this->mContinueScraping = true;
        //SetEvent(this->mScrapeControlEvent);
        //fprintf(this->mlogfile, "no changes");
        //fflush(this->mlogfile);
    }
}
int X11ScraperPoster::writeCompressedTile(deskblast::Region* region, int cursor, HashString* hs, int spriteX, int spriteY,
        unsigned char * gifStart, int gifLength){
    s_trace("starting GIF writeCompressedTile...");
    printf("writing GIF region x: %d y: %d\n", region->getX(), region->getY());
    hs->copyTo((char*)mSendBuffer, cursor);
    int *now_index_as_int = (int*) (mSendBuffer + cursor + HASH_LENGTH_BYTES);
    now_index_as_int[0] = htonl(gifLength);
    now_index_as_int[1] = htonl(region->getX());
    now_index_as_int[2] = htonl(region->getY());
    now_index_as_int[3] = htonl(region->getWidth());
    now_index_as_int[4] = htonl(region->getHeight());
    now_index_as_int[5] = htonl(spriteX);
    now_index_as_int[6] = htonl(spriteY);
    cursor += (28 + HASH_LENGTH_BYTES); // 3 (7) ints (4 bytes each) plus hash
    memcpy(&mSendBuffer[cursor], gifStart, gifLength);
    return cursor + gifLength;
}
int X11ScraperPoster::compressIfNeededAndWriteTile(
        deskblast::Region * region,
    int cursor, int hashOffset, int spriteX, int spriteY, bool compress) {
    s_trace("starting compressIfNeededAndWriteTile...");
    printf("writing region x: %d y: %d compress: %s\n", region->getX(), region->getY(),
            (compress)?"true":"false");
    int imageSize = 0;
    if(compress){
        // TODO: if screen size changes GifEncoder2 will blow up
        if (imageSize < 1) {
            imageSize = compressRegionToBuffer(region, cursor);
                                               //(byte*) this->getCurrentBuffer());
            s_trace("compressIfNeededAndWriteTile returned from jpeg encode");
            printf("done JPEG compressing to buffer size: %d\n", imageSize);
        }
        cursor += imageSize;
    }
    int *now_index_as_int = (int*) (mSendBuffer + hashOffset + HASH_LENGTH_BYTES);
    now_index_as_int[0] = htonl(imageSize);
    now_index_as_int[1] = htonl(region->getX());
    now_index_as_int[2] = htonl(region->getY());
    now_index_as_int[3] = htonl(region->getWidth()); // TODO: this is the width and height of the merged rect but
    now_index_as_int[4] = htonl(region->getHeight());// it should be the width and height of the first region mapped to it
    now_index_as_int[5] = htonl(spriteX);
    now_index_as_int[6] = htonl(spriteY);
    s_trace("compressIfNeededAndWriteTile returning");
    return cursor;
}
void X11ScraperPoster::initRegions(void) {
    s_trace("X11ScraperPoster::initRegions");
    this->regions = deskblast::Region::allocateRegionsForBounds(screenWidth, screenHeight, &regionCount);
}
int X11ScraperPoster::checkForChanges() {
    int changes = 0;
    for (int i = 0; i < this->regionCount; i++) {
        bool result = this->regions[i]->checkForChange(
                          screen->getCurrentBuffer(), screen->getPreviousBuffer());
        if (result)
            changes++;
    }
    return changes;
}
void X11ScraperPoster::hashBytes(unsigned char* pixels, int length, char* dest) {
    MD5_CTX ctx;
    MD5Init(&ctx);
    MD5Update(&ctx, pixels, length);
    MD5Final(&ctx);
    for (int i = 0; i < 16; i++) {
        sprintf((char*) &(dest[(i * 2)]), "%02x", ctx.digest[i]);
    }

}
void X11ScraperPoster::hashPixels(void* pixels, deskblast::Region* sourceRegion, char* dest) {
    MD5_CTX ctx;
    MD5Init(&ctx);
    int scanlines = sourceRegion->getHeight();
    int scanlineLengthInBytes = sourceRegion->getWidth() * deskblast::Region::BYTES_PER_PIXEL;
    for (int i = 0; i < scanlines; i++) {
        MD5Update(&ctx, sourceRegion->getScanline(i, pixels), scanlineLengthInBytes);
    }
    MD5Final(&ctx);
    for (int i = 0; i < 16; i++) {
        sprintf((char*) &(dest[(i * 2)]), "%02x", ctx.digest[i]);
    }
}
int X11ScraperPoster::writeHeader(int changedTileCount) {
    int *now_index_as_int = (int*) mSendBuffer;
    now_index_as_int[0] = htonl(changedTileCount == this->regionCount ? 1 : 0); // keyframe
    now_index_as_int[1] = htonl(screenWidth);
    now_index_as_int[2] = htonl(screenHeight);
    now_index_as_int[3] = htonl(mouseX);
    now_index_as_int[4] = htonl(mouseY);
    now_index_as_int[5] = htonl(changedTileCount);
    return 6 * 4;
}
int X11ScraperPoster::compressRegionToBuffer(deskblast::Region* region, int offset){
//        byte* lpvpxldata) {
    unsigned char * lpvpxldata = (unsigned char*)screen->getCurrentBuffer();
    int jpegSize;
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);
    my_jpeg_mem_dest(&cinfo, &mSendBuffer[offset], (SEND_BUFFER_LENGTH) - offset);
    cinfo.input_components = 4; // TODO: fixme: was 3
    cinfo.in_color_space = JCS_EXT_BGRX;//JCS_RGB;
    cinfo.image_width = region->getWidth();
    cinfo.image_height = region->getHeight();
    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, 65, TRUE);
    jpeg_start_compress(&cinfo, TRUE);
    JSAMPROW row_pointer[1];
    while (cinfo.next_scanline < cinfo.image_height) {
        row_pointer[0] = region->getScanline(cinfo.next_scanline, lpvpxldata);
        jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }
    jpeg_finish_compress(&cinfo);
    jpegSize = my_jpeg_mem_size(&cinfo);
    jpeg_destroy_compress(&cinfo);
    return jpegSize;
}
void X11ScraperPoster::expireDisplay() {
    this->displayChanged = TRUE;
}
bool X11ScraperPoster::displayExpired() {
    bool res = this->displayChanged;
    this->displayChanged = FALSE;
    return res;
}
void X11ScraperPoster::cleanUpHash(void) {
    //treeCleanup( root);
    //sentSet->clear();
    delete sentHashes;
    delete subtileMap;
}
/*
void X11ScraperPoster::sendScrapingStoppedMessage() {
    char postUrl[4096];
    int responseLength = 0;
    char* servlet = "stoppedShowing";
    buildUrlFromMetaData(pScraperPosterMetaData, postUrl, servlet);
    Poster stopPoster(pScraperPosterMetaData->getHost(),
                      pScraperPosterMetaData->getPort(), postUrl,
                      pScraperPosterMetaData->getSsl());
    char* data = "bogodata";
    char response[4096];
    BOOL result = stopPoster.sendPost((BYTE*) data, 8, (BYTE*) response, 4096,
                                      &responseLength);
    if (result) {
        s_trace("stoppedShowing sent successfully");
    } else {
        s_trace("error posting stoppedShowing");
    }
}
*/
