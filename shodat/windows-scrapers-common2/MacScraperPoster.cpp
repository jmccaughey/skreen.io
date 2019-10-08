#include "MacScraperPoster.h"
#include "LatencyChecker.hpp"

#define NANO_SECONDS_PER_MILLI  1000000

#ifndef JPEG_QUALITY
#define JPEG_QUALITY 35
#endif

extern "C" {
    #include "md5.h"
    #include "jpeg_mem_dest.h"
}

const char LOG[] = "skreen.io_scrape.log";

void s_trace(const char *msg) {
    //trace(LOG, msg);
    printf("%s\n", msg);
}

MacScraperPoster::MacScraperPoster(){
    s_trace("MacScraperPoster ctor");
    s_trace(__DATE__);
    s_trace(__TIME__);
	this->poster = NULL;
	this->screen = NULL;
    this->servers = NULL;
    mSendBuffer = (unsigned char *)malloc(SEND_BUFFER_LENGTH);
    memset(mSendBuffer, 0, SEND_BUFFER_LENGTH);
    printf("set memory\n");    
    gifSpriter = NULL;
    mShutting = false;
    millisTimer = NULL;
	mContinueScraping = true;
    this->regionCount = 0;
    this->regions = NULL;
    this->sentHashes = NULL;
    this->subtileMap = NULL;
}

int MacScraperPoster::connectToSessionHost(){
    printf("getting anonymous credentials\n");
    char* response = (char*)malloc(4097);
    size_t responseLength = 0;
   	int result = this->poster->sendPost("/createAndStartBlast", (char*)"{}", 2, response, 4096, &responseLength);
    response[responseLength] = '\0';
    printf("got %d from poster\n", result);
    if(result != 0){
        return NULL;
    }
    if(responseLength > 0){
        JsonParser* jsonParser = new JsonParser(std::string(response));
        std::string blastId = jsonParser->getValue("blastId");
        std::string id = jsonParser->getValue("id");
        std::string key = jsonParser->getValue("key");
        std::string name = jsonParser->getValue("name");
        if(!blastId.length() || !id.length() || !key.length() || !name.length()){
            printf("expected parameters not received from server (got:\n %s)\n", response);
            return NULL;
        }
        pScraperPosterMetaData = new ScraperPosterMetaData(this->poster->getServer(),
                                                           blastId.c_str(),
                                                           id.c_str(),
                                                           key.c_str(),
                                                           name.c_str()
                                                           );
    }else{
        printf("empty response received from server\n");
        return NULL;
    }
    free(response);
    return 1;
}

int MacScraperPoster::getSessionServer(){
    printf("getting server list\n");
    char* response = (char*)malloc(4097);
    size_t responseLength = 0;
    if(servers == NULL){
        servers = new std::vector<CurlConnection*>;
        servers->push_back(new CurlConnection(this->THE_HOST));
    }
    int result = servers->at(0)->sendPost("/listServers", (char*)"{}", 2, response, 4096, &responseLength);
    response[responseLength] = '\0';
    printf("got %d from poster\n", result);
    if(result != 0){
        return NULL;
    }
    if(responseLength > 0){
        std::string responseStr = response;
        printf("response: \n%s \n", response);
        std::istringstream ss(responseStr);
        while (!ss.eof()){
            std::string x;
            getline( ss, x, '\n' );
            if(x.length() > 2 && servers->size() < MAX_SERVERS){
                if(x.find("http://") ==  std::string::npos){
                    x = "http://" + x;
                }
                printf("normalized server: %s\n", x.c_str());
                if(x.compare(this->THE_HOST) != 0){
                    servers->push_back(new CurlConnection(x));
                }else{
                    printf("skipping %s\n", this->THE_HOST);
                }
            }
        }
        printf("server list size: %lu\n", servers->size());
        LatencyChecker latencyChecker;
        this->poster = latencyChecker.findLowestLatencyConnection(servers);
        if(this->poster == NULL){
            // there was a problem testing the servers
            printf("unable to test servers\n");
            free(response);
            return NULL;
        }
        printf("%s is the fastest server\n", this->poster->getServer().c_str());
    }else{
        printf("empty response received from server\n");
        return NULL;
    }
    free(response);
    return 1;
}

int MacScraperPoster::connect(){
    int result = getSessionServer();
    if(result == 0){
        return 0;
    }
    result = connectToSessionHost();
    if(result == 0){
        return 0;
    }
    const char* servlet = "/scrapes";
    std::string path = pScraperPosterMetaData->buildPath(servlet);
    s_trace(path.c_str());
    strcpy(postPath, path.c_str());
	return 1;
}

void MacScraperPoster::getMeetingCode(char* buffer){
    strcpy(buffer, this->pScraperPosterMetaData->getName().c_str());
}

void MacScraperPoster::init(){
    screen = new PlatformScreen();
    screenWidth = screen->getWidth();
    screenHeight = screen->getHeight();    
    regions = NULL;
    mSendKeyframe = true;
    sentHashes = new boost::unordered_set<std::string>();
    subtileMap = new boost::unordered_map<std::string, SubtileInfo>();
    initRegions();
    
}
void MacScraperPoster::cleanup(){
    if(screen){
        printf("about to delete screen\n");
        delete screen;
        screen = NULL;
    }
    if (this->poster) {
        printf("about to delete this->poster\n");
        delete (this->poster);
        this->poster = NULL;
        cleanUpHash();
    }
    if(gifSpriter){
        printf("about to delete gifSpriter\n");
        delete gifSpriter;
        gifSpriter = NULL;
    }
    printf("deleting %d regions\n", regionCount);
    for(int i = 0; i < regionCount; i++){
    	delete regions[i];
    }
    if(millisTimer){
        printf("deleting timer\n");
        delete millisTimer;
        millisTimer = NULL;
    }
    printf("deleting region array\n");
    delete[] regions;
}
MacScraperPoster::~MacScraperPoster() {
    printf("MacScraperPoster dtor\n");
    cleanup();
}
void MacScraperPoster::doScreenShot() {
	if(screen == NULL){
        printf("about to call MacScraperPoster::init\n");
		init();
        printf("returned from MacScraperPoster::init\n");
	}
    int changedResolution = screen->doScreenShot();
    if(changedResolution){
        printf("changedResolution\n");
        cleanup();
        init();
        screen->doScreenShot();
    }
}

void MacScraperPoster::sleepAndUpdateTicks(){
	unsigned int millis = getSleepTime();
	if(millis > 0){
		printf("sleeping %u...\n", millis);
        useconds_t sleep_time = millis * 1000;
        usleep(sleep_time);
	}else{
		printf("not sleeping\n");
	}
    this->millisTimer->reset();
}
unsigned int MacScraperPoster::getSleepTime(void) {
    if(millisTimer == NULL){
        millisTimer = new PlatformTimer();
        return 0;
    }
    unsigned int elapsed = millisTimer->getElapsedMillis();
    if(elapsed > SCRAPE_THROTTLE_MS){
        return 0;
    }else{
        return SCRAPE_THROTTLE_MS - elapsed;
    }
}
void MacScraperPoster::doScreenScrape() {
	sleepAndUpdateTicks();
    printf("about to doScreenShot\n");
    doScreenShot();
    printf("returned from doScreenShot\n");
    PlatformTimer profileTimer;
    printf("send key frame: %d\n", this->mSendKeyframe);
    printf("region count: %d\n", this->regionCount);
    int changeCount = this->mSendKeyframe ? this->regionCount : this->checkForChanges();
    int writtenCount = 0;
    bool mouseMoved = screen->updateMousePosition(&mouseX, &mouseY);
    int cursor = 0;
    if (changeCount > 0 || mouseMoved) {
        cursor += writeHeader(changeCount);
        printf("announced %d changed tiles\n", changeCount);
        char hashBuffer[HASH_LENGTH_BYTES+1];
        // MD5 all tiles, check if already sent
        std::vector<deskblast::Region*> newRegions;
        boost::unordered_map<std::string, SubtileInfo>::iterator it;
        std::map<deskblast::Region, std::string> rectHashes;
        std::map<deskblast::Region, std::string>::iterator rectHashesIt;
        for (int k = 0; k < this->regionCount; k++) {
            if (this->regions[k]->isChanged() || this->mSendKeyframe) {
                hashPixels((void*)(screen->getCurrentBuffer()), this->regions[k], (char*)hashBuffer);
                std::string hs(hashBuffer);
                it = subtileMap->find(hs);
                if (it != subtileMap->end()) {
                    // this subtile has been sent before so write it as back ref
                    SubtileInfo found = it->second;
                    int hashOffset = cursor;
                    found.copyTo((char*)mSendBuffer, cursor);
                    cursor += (28 + HASH_LENGTH_BYTES); // 3 (7) ints (4 bytes each) plus hash
                    cursor = writeBackReference(this->regions[k], cursor, hashOffset, found.getX(), found.getY());
                    writtenCount++;
                } else {
                    newRegions.push_back(this->regions[k]);
                    //printf("inserting %s into newRegions\n", hs.c_str());
                    rectHashes.insert(std::pair<deskblast::Region, std::string>(*(this->regions[k]), hs));
                    //printf("inserted. now has %zu\n", rectHashes.size());
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
        unsigned long newRegionCount = newRegions.size();
        gifSpriter->makeSprites(&newRegions, (unsigned char*) screen->getCurrentBuffer(), &gifSpriteInfos, &ungiffableRegions);
        newRegions.clear();
        printf("%lu ungiffable regions\n", ungiffableRegions.size());
        printf("%lu GifSpriteInfos\n", gifSpriteInfos.size());
        unsigned int mappedRegionCount = 0;
        for(unsigned int i = 0; i < gifSpriteInfos.size(); i++){
            mappedRegionCount += gifSpriteInfos.at(i)->getMappedRegions()->size();
        }
        if(mappedRegionCount + ungiffableRegions.size() != newRegionCount){
            printf("PROBLEM: mappedRegionCount (%d) plus ungiffables (%lu) does not equal new region count (%lu)\n",
                    mappedRegionCount, ungiffableRegions.size(), newRegionCount);
        }
        for(unsigned int i = 0; i < gifSpriteInfos.size(); i++){
            GifSpriteInfo* gsiPtr = gifSpriteInfos.at(i);
            hashBytes(gsiPtr->getGifStart(), gsiPtr->getGifLength(), (char*)hashBuffer);
            std::string hs(hashBuffer);
            boolean spriteSent = false;
            printf("gifspriteinfo # %d has %lu mapped regions\n",
                    i, gifSpriteInfos.at(i)->getMappedRegions()->size());
            for(unsigned int j = 0; j < gifSpriteInfos.at(i)->getMappedRegions()->size(); j++){
                MappedRegion mr = gifSpriteInfos.at(i)->getMappedRegions()->at(j);
                if (spriteSent || this->sentHashes->count(hs)) {
                    int hashOffset = cursor;
                    memcpy(mSendBuffer + cursor, hs.c_str(), HASH_LENGTH_BYTES);
                    cursor += (28 + HASH_LENGTH_BYTES); // 3 (7) ints (4 bytes each) plus hash
                    cursor = writeBackReference(mr.getRegion(), cursor, hashOffset,
                                    mr.getTranslateX(), mr.getTranslateY());
                    writtenCount++;
                }else{
                    cursor = writeCompressedTile(mr.getRegion(), cursor, &hs, mr.getTranslateX(), mr.getTranslateY(),
                            gsiPtr->getGifStart(), gsiPtr->getGifLength());
                    writtenCount++;
                    this->sentHashes->insert(hs);
                    printf("inserted gif %s into sentHashes\n", hs.c_str());
                    spriteSent = true;
                }
                rectHashesIt = rectHashes.find(*(mr.getRegion()));
                SubtileInfo si(hs.c_str(), mr.getTranslateX(), mr.getTranslateY());
                this->subtileMap->insert(std::pair<std::string, SubtileInfo>(rectHashesIt->second, si));
            }
        }
        for(std::vector<GifSpriteInfo*>::iterator iter = gifSpriteInfos.begin() ; iter != gifSpriteInfos.end(); ++iter){
        	delete *iter;
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
            std::string hs(hashBuffer);
            if (this->sentHashes->count(hs)) {
                printf("this merged region found in sentHashes\n");
                for (unsigned int m = 0; m < ungiffableRegions.size(); m++) {
                    if (mergedRegions[i].contains(*(ungiffableRegions[m]))) {
                        int hashOffset = cursor;
                        memcpy(mSendBuffer + cursor, hs.c_str(), HASH_LENGTH_BYTES);
                        cursor += (28 + HASH_LENGTH_BYTES); // 3 (7) ints (4 bytes each) plus hash
                        cursor = writeBackReference(ungiffableRegions[m], cursor, hashOffset,
                                ungiffableRegions[m]->getX() - mergedRegions[i].getX(), ungiffableRegions[m]->getY() - mergedRegions[i].getY());
                        writtenCount++;
                    }
                }
            } else {
                printf("not found; adding jpeg to sentHashes %s\n", hs.c_str());
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
                            memcpy(mSendBuffer + cursor, hs.c_str(), HASH_LENGTH_BYTES);
                            cursor += (28 + HASH_LENGTH_BYTES); // 3 (7) ints (4 bytes each) plus hash
                            cursor = compressAndWriteTile(&(mergedRegions[i]), cursor, hashOffset,
                                                                    ungiffableRegions[m]->getWidth(), ungiffableRegions[m]->getHeight(),
                                                                  ungiffableRegions[m]->getX() - mergedRegions[i].getX(),
                                                                  ungiffableRegions[m]->getY() - mergedRegions[i].getY());
                            writtenCount++;
                            spriteSent = true;
                        } else {
                            int hashOffset = cursor;
                            memcpy(mSendBuffer + cursor, hs.c_str(), HASH_LENGTH_BYTES);
                            cursor += (28 + HASH_LENGTH_BYTES); // 3 (7) ints (4 bytes each) plus hash
                            printf("not posting jpeg %s\n", hs.c_str());
                            cursor = writeBackReference(ungiffableRegions[m], cursor, hashOffset,
                                                                  ungiffableRegions[m]->getX() - mergedRegions[i].getX(),
                                                                  ungiffableRegions[m]->getY() - mergedRegions[i].getY());
                            writtenCount++;
                        }
                        rectHashesIt = rectHashes.find(*(ungiffableRegions[m]));
                        SubtileInfo si(hs.c_str(),
                                       ungiffableRegions[m]->getX() - mergedRegions[i].getX(),
                                       ungiffableRegions[m]->getY() - mergedRegions[i].getY());
                        this->subtileMap->insert(std::pair<std::string, SubtileInfo>(rectHashesIt->second, si));
                    }
                }
            }
            //}
        }
        printf("compressing took %lu\n", profileTimer.getElapsedMillis());
        this->mSendKeyframe = false;
        if (!mShutting) {
            char response[RESPONSE_BUFFER_LENGTH];
            size_t responseLength = 0;
            printf("about to post %d bytes\n", cursor);
            //AbsoluteTime start =  UpTime();
            profileTimer.reset();
            int result = this->poster->sendPost((char*)postPath,
                                                 /*(unsigned char*)*/(char*)mSendBuffer,
                                                 cursor,
                                                 /*(unsigned char*)*/response,
                                                 RESPONSE_BUFFER_LENGTH, &responseLength);
            printf("post took %lu milliseconds result: %d\n",
                   profileTimer.getElapsedMillis(), result);
            if (result == CURLE_OK) {
                char* bytes = (char*) response;
                char command = bytes[0];
                this->mContinueScraping = (command == 1 || command == 2);
                this->mSendKeyframe = (command == 2);
            } else {
                // TODO: return error code or something
                this->mContinueScraping = false;
                s_trace("sendPost failed");
            }
        }
        if(writtenCount != changeCount){
            printf("changeCount: %d writtenCount: %d\n", changeCount, writtenCount);
        }
    } else {
        // TODO: check below
//        if (this->commandExpired()) {
//            this->mSendKeyframe = true;
//            //fprintf(this->mlogfile, "command expired -- forcing keyframe");
//        }
        this->mContinueScraping = true;
    }
}
int MacScraperPoster::writeCompressedTile(deskblast::Region* region, int cursor, std::string* hs, int spriteX, int spriteY,
        unsigned char * gifStart, int gifLength){
    s_trace("starting GIF writeCompressedTile...");
    printf("writing GIF region x: %d y: %d\n", region->getX(), region->getY());
    memcpy(mSendBuffer + cursor, hs->c_str(), HASH_LENGTH_BYTES);
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
int MacScraperPoster::writeBackReference(deskblast::Region * region,
                                         int cursor, int hashOffset, int spriteX, int spriteY) {
    s_trace("starting writeBackReference...");
    //    printf("writing region x: %d y: %d compress: %s\n", region->getX(), region->getY(),
    //            (compress)?"true":"false");d
    int *now_index_as_int = (int*) (mSendBuffer + hashOffset + HASH_LENGTH_BYTES);
    now_index_as_int[0] = htonl(0); // image size is zero b/c this is back reference
    now_index_as_int[1] = htonl(region->getX());
    now_index_as_int[2] = htonl(region->getY());
    now_index_as_int[3] = htonl(region->getWidth());
    now_index_as_int[4] = htonl(region->getHeight());
    now_index_as_int[5] = htonl(spriteX);
    now_index_as_int[6] = htonl(spriteY);
    s_trace("...writeBackReference returning");
    return cursor;
}
int MacScraperPoster::compressAndWriteTile(
        deskblast::Region * region,
    int cursor, int hashOffset, int width, int height, int spriteX, int spriteY) {
    s_trace("starting compressAndWriteTile...");
    int imageSize = 0;
        // TODO: if screen size changes GifEncoder2 will blow up
        if (imageSize < 1) {
            imageSize = compressRegionToBuffer(region, cursor);
                                               //(byte*) this->getCurrentBuffer());
            s_trace("compressAndWriteTile returned from jpeg encode");
            printf("done JPEG compressing to buffer size: %d\n", imageSize);
        }
        cursor += imageSize;
    int *now_index_as_int = (int*) (mSendBuffer + hashOffset + HASH_LENGTH_BYTES);
    now_index_as_int[0] = htonl(imageSize);
    now_index_as_int[1] = htonl(region->getX());
    now_index_as_int[2] = htonl(region->getY());
    now_index_as_int[3] = htonl(width);
    now_index_as_int[4] = htonl(height);
    now_index_as_int[5] = htonl(spriteX);
    now_index_as_int[6] = htonl(spriteY);
    s_trace("compressAndWriteTile returning");
    return cursor;
}
void MacScraperPoster::initRegions(void) {
    s_trace("MacScraperPoster::initRegions");
    printf("width: %d height: %d\n", screenWidth, screenHeight);
    this->regions = deskblast::Region::allocateRegionsForBounds(screenWidth, screenHeight, &regionCount, false);
    printf("returned from allocateRegionsForBounds\n");
}
int MacScraperPoster::checkForChanges() {
    int changes = 0;
    for (int i = 0; i < this->regionCount; i++) {
        bool result = this->regions[i]->checkForChange(
                          screen->getCurrentBuffer(), screen->getPreviousBuffer());
        if (result)
            changes++;
    }
    return changes;
}
void MacScraperPoster::hashBytes(unsigned char* pixels, int length, char* dest) {
    MD5_CTX ctx;
    MD5Init(&ctx);
    MD5Update(&ctx, pixels, length);
    MD5Final(&ctx);
    for (int i = 0; i < 16; i++) {
        sprintf((char*) &(dest[(i * 2)]), "%02x", ctx.digest[i]);
    }
}
void MacScraperPoster::hashPixels(void* pixels, deskblast::Region* sourceRegion, char* dest) {
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
int MacScraperPoster::writeHeader(int changedTileCount) {
    int *now_index_as_int = (int*) mSendBuffer;
    printf("changedTileCount: %d\n", changedTileCount);
    printf("this->regionCount: %d\n", this->regionCount);
    now_index_as_int[0] = htonl(changedTileCount == this->regionCount ? 1 : 0); // keyframe
    now_index_as_int[1] = htonl(screenWidth);
    now_index_as_int[2] = htonl(screenHeight);
    now_index_as_int[3] = htonl(mouseX);
    now_index_as_int[4] = htonl(mouseY);
    now_index_as_int[5] = htonl(changedTileCount);
    return 6 * 4;
}
int MacScraperPoster::compressRegionToBuffer(deskblast::Region* region, int offset){
    // TODO: if in grayscale mode, try PNG compression. If result is .2 byte
    // per pixel or better, go with that. If not, attempt jpeg compression
    // & use that if it is more space efficient
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
    jpeg_set_quality(&cinfo, JPEG_QUALITY, TRUE);
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
void MacScraperPoster::expireDisplay() {
    this->displayChanged = TRUE;
}
bool MacScraperPoster::displayExpired() {
    bool res = this->displayChanged;
    this->displayChanged = FALSE;
    return res;
}
void MacScraperPoster::cleanUpHash(void) {
    printf("deleting sentHashes\n");
    delete sentHashes;
    printf("deleting subtileMap\n");
    delete subtileMap;
}
void MacScraperPoster::stopScraping(){
    printf("stopScraping setting mShutting = TRUE\n");
    mContinueScraping = FALSE;
    mShutting = TRUE;
    if(this->servers && this->servers->size() > 0){
        for(std::vector<CurlConnection*>::iterator iter = servers->begin() ; iter != servers->end(); ++iter){
            (*iter)->interrupt();
        }
    }
}

void MacScraperPoster::sendScrapingStoppedMessage() {
    unsigned long responseLength = 0;
    std::string stopPath = pScraperPosterMetaData->buildPath("/stoppedShowing");
    CurlConnection stopPoster(this->poster->getServer());
    printf("about to send stopped message\n");
    const char* data = "bogo";
    char response[4096];
    bool result = stopPoster.sendPost(stopPath.c_str(), (char*) data, strlen(data), (char*) response, 4096,
                                      &responseLength);
    if (!result) {
        s_trace("stoppedShowing sent successfully");
    } else {
        s_trace("error posting stoppedShowing");
    }
}
