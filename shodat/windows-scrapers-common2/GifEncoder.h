#pragma once

#include "../GifEncodeC/gd_io.h"
#include "../GifEncodeC/gd_io_memory.h"

#include "Region.h"

//extern "C" {
void GIFEncode(gdIOCtx *fp, int GWidth, int GHeight, int GInterlace, int Background, 
	int Transparent, int BitsPerPixel, int *Red, int *Green, int *Blue, int* im);

int colorstobpp(int colors);
int closeEnough(int first, int second, int difference);
int addOrIndex(int* reds, int* greens, int* blues, int red, int green, int blue, int* count);




//}


class GifEncoder{

public:
	GifEncoder(int maxWidth, int maxHeight);
	int encode(Region* regionPtr, unsigned char * pixels, char *output,
						unsigned int outputSize); 
	~GifEncoder(void);
	
private:
	static const int MAX_COLORS = 256;
	static const int BYTES_PER_PIXEL = 3;

	int* indexTable;
	int* Red;
	int* Green;
	int* Blue;	
	int _maxWidth;
	int _maxHeight;
	

};
