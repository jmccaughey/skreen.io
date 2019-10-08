#include <stdlib.h>
#include <stdio.h>
#include "GifEncoder.h"

GifEncoder::GifEncoder(int maxWidth, int maxHeight){
	indexTable = (int*)malloc((sizeof(int))*(maxWidth*maxHeight));
	_maxWidth = maxWidth;
	_maxHeight = maxHeight;
	Red = (int*)malloc(sizeof(int)*MAX_COLORS);
	Green = (int*)malloc(sizeof(int)*MAX_COLORS);
	Blue = (int*)malloc(sizeof(int)*MAX_COLORS);
}

GifEncoder::~GifEncoder(void){
	//printf("freeing indextable\n");
	free(indexTable);
	//printf("done freeing indexTable\n");
	free(Red);
	free(Green);
	free(Blue);
	//printf("done freeing\n");
}
int GifEncoder::encode(
	Region* regionPtr, 
	unsigned char * pixels, 
	char *output, 
	unsigned int outputSize){
	
	int colorCount = 0;
	int byteIndex = 0;
	//int PIXEL_COUNT = (regionPtr->getWidth()) * (regionPtr->getHeight());
	int width = regionPtr->getWidth();
	int height = regionPtr->getHeight();
	printf("region width: %d\n", width);
	printf("region height: %d\n", height);
	int indexCount = 0;
	for(int i = height-1; i > -1; i--){
		unsigned char * row = regionPtr->getScanline(i, pixels);
		int rowBytes = width * BYTES_PER_PIXEL;
		for(int j = 0; j < rowBytes;){
			int blue = row[j++];
			int green =	row[j++];
			int red = row[j++];
			int index = addOrIndex(Red, Green, Blue, red, green, blue, &colorCount);
			if(index == -1){
				printf("too many colors row: %d column: %d\n", i, indexCount % width);
				return 0;		
			}
			//int indexIndex = i*width+j;
			//printf("%d\n",indexIndex);
			indexTable[indexCount++] = index;
		}
	}
	printf("%d colors found\n", colorCount);	
	gdIOCtx *outCtx = 0;
	outCtx = (gdIOCtx*)newMemoryCtx(output, outputSize);
	int interlace, BitsPerPixel;	
	interlace = 0;
	BitsPerPixel = colorstobpp(colorCount);
	printf("about to GIFEncode...\n");	
	GIFEncode(
		outCtx, regionPtr->getWidth(), regionPtr->getHeight(), interlace, 1, -1, BitsPerPixel,
		Red, Green, Blue, indexTable);
	int gifSize = outCtx->getSize(outCtx);  
	printf("size: %d\n", gifSize);
	outCtx->gd_free(outCtx);
	return gifSize;
} 
