#include <stdio.h>
#include <stdlib.h>

#include "Region.h"

long GIFencode(int Width, int Height, int Ncol, 
		unsigned char* R, unsigned char* G, unsigned char* B, 
		unsigned char** scanlines, unsigned char* outputBuffer);

class GifEncoder2{

public:
	GifEncoder2(int maxWidth, int maxHeight);
	long encode(deskblast::Region* regionPtr, unsigned char * pixels, unsigned char *output,
						unsigned int outputSize); 
	long encodeMultiple(
		deskblast::Region** regionPtr,
		int regionCount, 
		unsigned char* pixels, 
		unsigned char* output, 
		unsigned int outputSize);
	// return -1 if too many colors, 0 if too many colors to merge
	int merge(deskblast::Region* regionPtr, unsigned char * pixels);
	~GifEncoder2(void);
	
private:
	static const int MAX_COLORS = 256;
	static const int BYTES_PER_PIXEL = 4; // TODO: fixme bytes per pixel was 3
	static const int LOSSY_FACTOR_RED = 4; // TODO: fixme was 2
	static const int LOSSY_FACTOR_GREEN = 4; // TODO: fixme was 2
	static const int LOSSY_FACTOR_BLUE = 4; // TODO: fixme was 2

	unsigned char* _pixels;
	unsigned char** _scanlines;
	unsigned char* _R;
	unsigned char* _G;
	unsigned char* _B;	
	int _maxWidth;
	int _maxHeight;

};
