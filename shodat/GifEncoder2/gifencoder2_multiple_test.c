#include <stdio.h>
#include <stdlib.h>

#include "GifEncoder2.h"
#include "Region.h"


#include <time.h>
#include <sys/time.h>
#include <stdint.h>

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

	int regionCount;
	Region** regions = Region::allocateRegionsForBounds(screenWidth, height, &regionCount);
	printf("returned from allocateRegions %d\n", regionCount);        
	long gifLength;
	char outFile[64];
        long totalBytes = 0;
        int ungiffable = 0;
//	for(int i = 0; i < regionCount; i++){
          GifEncoder2 enc(screenWidth, height);
          printf("created encoder on stack\n");
          gifLength = enc.encodeMultiple(regions, 2, allPixels, output, outputSize);
          printf("returned from encode\n");
	  if(gifLength > 0){	
		printf("writing %ld byte file...\n", gifLength);
		sprintf(outFile, "out_0.gif");
		FILE* fpOut = fopen(outFile, "wb");
		fwrite(output, 1, gifLength, fpOut);
		fclose(fpOut);
		totalBytes += gifLength;
	  }else{
		printf("too many Colors\n");
                ungiffable++;
	  }  		
//	}
	printf("wrote %d out of %d regions (%d bytes total)\n", regionCount-ungiffable, regionCount, (int)totalBytes);
	free(fileBuffer);		
	free(output);
}

int readInt(unsigned char * reader){
	return (*(reader + 3) << 24 | *(reader + 2) << 16 | *(reader + 1) << 8 | *reader);
}
