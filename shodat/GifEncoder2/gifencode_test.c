#include <stdio.h>
#include <stdlib.h>
//#include <string.h>


//typedef unsigned char byte
unsigned char* pixels;
unsigned char** scanlines;
unsigned char* output;
unsigned int cursor;

/*
int get_scline(int y, int width, unsigned char scline[])
{
  int i;
  for (i=0; i<width; i++) scline[i] = scanlines[(255-y)*width+i];//0;//getc(stdin);
}
*/
int put_byte(unsigned char b){
  //if (ferror(stdout) == 0) fputc(b, stdout);
  output[cursor++] = b;
}   

int main(int argc, char* argv[]){
 	//printf("%s\n", argv[1]);
	unsigned char* R;
	unsigned char* G;
	unsigned char* B;
	cursor = 0; 
	
	R = malloc(256);
	G = malloc(256);
	B = malloc(256);
	pixels = malloc(256*256);
	scanlines = malloc(sizeof(unsigned char*) * 256);
	output = malloc(100*1024); // 100K
	
	int i, j;
	
	unsigned char* scanline;
	scanline = malloc(256);
	
	// read bmp file pixels into byte array
	// for every three bytes, add to color table or get index
	// put index into pixels	
	
	int colorCount;
	
	FILE *fpSource = fopen(argv[1], "rb");
	long lSize, result;
	char * fileBuffer;
	unsigned char* allPixels;
	fseek (fpSource , 0 , SEEK_END);
  	lSize = ftell (fpSource);
  	rewind (fpSource);
  	fileBuffer = (char*) malloc (sizeof(char)*lSize);
  	result = fread (fileBuffer,1,lSize,fpSource);
  	//printf("read: %d\n", result);
  	if (result != lSize) {fputs ("Reading error",stderr); exit (3);}
	fclose(fpSource);
	
	allPixels = fileBuffer+54;
	unsigned int PIXEL_COUNT = 256*256;
	//printf("%d pixels\n", PIXEL_COUNT);
	unsigned int PIXEL_BYTES_LENGTH = PIXEL_COUNT * ((unsigned int)3);
		
	unsigned char red;
	unsigned char green;
	unsigned char blue;
	int byteIndex;
	int colorIndex;
	colorCount = 0;
	for(i = 0; i < PIXEL_COUNT; i++){
		byteIndex = i * 3;
		//printf("looking at pixel #%d\n", i);
		blue = allPixels[byteIndex];
		green = allPixels[byteIndex+1];
		red = allPixels[byteIndex+2];
		colorIndex = -1;
		for(j = 0; j < colorCount; j++){
			if(red == R[j] && green == G[j] && blue == B[j]){
				colorIndex = j;
				//printf("found match\n");
				break;
			}
		}	
		if(colorIndex == -1){
			R[colorCount] = red; G[colorCount] = green; B[colorCount] = blue;
			//printf("added r: %d g: %d b: %d\n", red, green, blue);
			colorIndex = colorCount++;
		}
		pixels[i] = colorIndex;
		if(i % 256 == 0){
			scanlines[255 - i/256] = pixels + i;			
		}
	}
	//printf("about to free fileBuffer...\n");
	free(fileBuffer);		
	
	long size = GIFencode(256, 256, colorCount, R, G, B, scanlines, output);
	printf("writing %d byte file...\n", size);
	FILE* fpOut = fopen("out.gif", "wb");
	fwrite(output, 1, size, fpOut);
	fclose(fpOut);
}
