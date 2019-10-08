#include <stdio.h>
#include <stdlib.h>

#include "GifSpriter.h"
#include "Region.h"
//#include "TestSpriteHandler.h"

#include <time.h>
#include <sys/time.h>
#include <stdint.h>

using namespace deskblast;

int stampstart(){
        struct timeval  tv;
        struct timezone tz;
        struct tm      *tm;
        uint32_t         start;
        gettimeofday(&tv, &tz);
        tm = localtime(&tv.tv_sec);
        printf("TIMESTAMP-START\t  %d:%02d:%02d:%d (~%d ms)\n", tm->tm_hour,
               tm->tm_min, tm->tm_sec, tv.tv_usec,
               tm->tm_hour * 3600 * 1000 + tm->tm_min * 60 * 1000 +
               tm->tm_sec * 1000 + tv.tv_usec / 1000);
        start = tm->tm_hour * 3600 * 1000 + tm->tm_min * 60 * 1000 +
                tm->tm_sec * 1000 + tv.tv_usec / 1000;
        return (start);
}

int stampstop(uint32_t start){
        struct timeval  tv;
        struct timezone tz;
        struct tm      *tm;
        uint32_t         stop;
        gettimeofday(&tv, &tz);
        tm = localtime(&tv.tv_sec);
        stop = tm->tm_hour * 3600 * 1000 + tm->tm_min * 60 * 1000 +
                tm->tm_sec * 1000 + tv.tv_usec / 1000;
        printf("TIMESTAMP-END\t  %d:%02d:%02d:%d (~%d ms) \n", tm->tm_hour,
               tm->tm_min, tm->tm_sec, tv.tv_usec,
               tm->tm_hour * 3600 * 1000 + tm->tm_min * 60 * 1000 +
               tm->tm_sec * 1000 + tv.tv_usec / 1000);
        printf("ELAPSED\t  %d ms\n", stop - start);
        return (stop);
}



int readInt(unsigned char * reader);

int main(int argc, char* argv[]){
	if(argc < 2){
		printf("usage: %s <infile.bmp>", argv[0]);
		return -1;
	}
 	printf("%s\n", argv[1]);
 	int outputSize = 100*1024;
	unsigned char * output = (unsigned char*)malloc(outputSize); // 100K
	unsigned char * allPixels;

	FILE *fpSource = fopen(argv[1], "rb");
	long lSize, result;
	unsigned char * fileBuffer;
	fseek (fpSource , 0 , SEEK_END);
  	lSize = ftell (fpSource);
  	rewind (fpSource);
  	fileBuffer = (unsigned char*) malloc (sizeof(char)*lSize);
  	result = fread (fileBuffer,1,lSize,fpSource);
  	printf("read: %ld\n", result);
  	if (result != lSize) {fputs ("Reading error",stderr); exit (3);}
	fclose(fpSource);
	//getchar();
	allPixels = fileBuffer+54;
	unsigned int PIXEL_COUNT = Region::TILE_WIDTH * Region::TILE_HEIGHT;
	printf("%d pixels\n", PIXEL_COUNT);

	int xIndex = 0;
	int yIndex = 0;
	if(argc > 2){
		xIndex = atoi(argv[2]);
		yIndex = atoi(argv[3]);
	}
	unsigned char * reader = fileBuffer + 18;
	int screenWidth = readInt(reader);
	reader += 4;
	int height = readInt(reader);
	printf("width: %d height: %d\n", screenWidth, height);

	// flip scanlines
	int byteCount = screenWidth * height * 3;
	printf("allocating %d bytes for flipping BMP\n", byteCount);
	unsigned char* bmp = (unsigned char*)malloc(byteCount);

	int j = 0;
	for(int i = height-1; i >= 0; i--){
		//printf("flipping %d\n", i);
		memcpy(bmp + (j*(screenWidth*3)), allPixels + (i*screenWidth*3), screenWidth*3);
		j++;
	}

    uint32_t         start, stop;
	start = stampstart();
	int regionCount;
	Region** regions = Region::allocateRegionsForBounds(screenWidth, height, &regionCount);
	printf("returned from allocateRegions %d\n", regionCount);
	long gifLength;
	char outFile[64];
        long totalBytes = 0;
        int ungiffable = 0;
    //GifPreEncoder enc1(screenWidth, height);
    
    std::vector<Region*> regionVector;
    for(int i = 0; i < regionCount; i++){
    	regionVector.push_back(regions[i]);
    }
    
    GifSpriter spriter(screenWidth, height);
    //ISpriteHandler * spriteHandler = new TestSpriteHandler();
    std::vector<Region*> ungiffableRegions;
    std::vector<GifSpriteInfo*> gifSpriteInfos;
    spriter.makeSprites(&regionVector, bmp, &gifSpriteInfos, &ungiffableRegions);
    printf("%d ungiffable regions\n", ungiffableRegions.size());
    printf("%d GifSpriteInfos\n", gifSpriteInfos.size());
    for(int i = 0; i < gifSpriteInfos.size(); i++){
    	//printf("gifSpriteInfo %d\n", i);
    	//printf("length %d\n", gifSpriteInfos.at(i)->getGifLength());
    	//printf("regions %d\n", gifSpriteInfos.at(i)->getMappedRegions()->size());
    	sprintf(outFile, "out_%d.gif", i);
    	for(int j = 0; j < gifSpriteInfos.at(i)->getMappedRegions()->size(); j++){
    		MappedRegion mr = gifSpriteInfos.at(i)->getMappedRegions()->at(j);
    		printf("<div style='position: absolute; left: %dpx; top: %dpx; width: %dpx; height: %dpx; background-image: url(\"%s\"); background-position: 0px -%dpx;'></div>\n", 
    			mr.getRegion()->getX(), mr.getRegion()->getY(), mr.getRegion()->getWidth(), mr.getRegion()->getHeight(), outFile, mr.getTranslateY()); 
    	}
    	//printf("first %d\n", *(gifSpriteInfos.at(i)->getGifStart()));
		//printf("writing %ld byte file...\n", gifSpriteInfos.at(i)->getGifLength());		
		FILE* fpOut = fopen(outFile, "wb");
		fwrite(gifSpriteInfos.at(i)->getGifStart(), 1, gifSpriteInfos.at(i)->getGifLength(), fpOut);
		fclose(fpOut);
		totalBytes += gifLength;
    }
    stop = stampstop(start);
  	printf("elapsed: %d\n", (double)(stop-start));
    
    for(int i = 0; i < gifSpriteInfos.size(); i++){
    	delete gifSpriteInfos.at(i);
    }
    
    //delete spriteHandler;
    /*
    std::vector<Region*> regionVector;
    for(int i = 0; i < regionCount; i++){
    	regionVector.push_back(regions[i]);
    }
    //int outputCursor = 0;
    int fileCount = 0;
    GifPreEncoder enc1(screenWidth, height);
    while(!regionVector.empty()){
    	  std::vector<int> eraseIndices;    	
    	  for(int i = 0; i < regionVector.size(); i++){
    	  	  Region* rptr = regionVector[i];  
			  result = enc1.merge(rptr, bmp);
			  if(result == -1){
			  	printf("merge %d failed - permanently ungiffable/too many colors\n", i);
			  	ungiffable++;
			  	eraseIndices.push_back(0);
		 	  }else if(result == 0){
		 	  	printf("merge %d failed - wrong dimensions or incompatible palette\n", i);
		 	  	// leave in collection for next pass
			  }else{
			  	printf("returned from merge %d\n", i);
			  	eraseIndices.push_back(i);
			  }
			  printf("regionVector.size() == %d\n", regionVector.size());
		  }
		  printf("erasing %d\n", eraseIndices.size());
		  for(int i = eraseIndices.size()-1; i >= 0 ; i--){
		  	printf("erasing %d\n", i);
		  	regionVector.erase(regionVector.begin() + eraseIndices.at(i));
		  }
		  eraseIndices.clear();
		  // call callback, with meta data and pointer to pointer to output
		  // callback writes non-back ref metadata, returns 
		  // gif encode to buffer pointer
		  // call callback for following backrefs  
    	  gifLength = enc1.encode(output, outputSize);
    	  enc1.reset();
    	  printf("returned from encode\n");
		  if(gifLength > 0){
			printf("writing %ld byte file...\n", gifLength);
			sprintf(outFile, "out_%d.gif", fileCount++);
			FILE* fpOut = fopen(outFile, "wb");
			fwrite(output, 1, gifLength, fpOut);
			fclose(fpOut);
			totalBytes += gifLength;
		  }else{
		    printf("problem encoding\n");
		  }
    }
    // call callback with vector of "ungiffable" Regions
    // or just return vector of "ungiffable" Regions?
	printf("wrote %d out of %d regions (%d bytes total)\n", regionCount-ungiffable, regionCount, (int)totalBytes);
	*/
	free(fileBuffer);
	free(output);
	free(bmp);
	for(int i = 0; i < regionCount; i++){
		delete regions[i];
	}
	delete[] regions;
}

int readInt(unsigned char * reader){
	return (*(reader + 3) << 24 | *(reader + 2) << 16 | *(reader + 1) << 8 | *reader);
}
