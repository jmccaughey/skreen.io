#include <stdio.h>
#include <stdlib.h>
#include <vector>


#include "Region.h"

long GIFencode(int Width, int Height, int Ncol, 
		unsigned char* R, unsigned char* G, unsigned char* B, 
		unsigned char** scanlines, unsigned char* outputBuffer);

class GifPreEncoder{

public:
    static const int MAX_HEIGHT = 4096;
	GifPreEncoder(int maxWidth, int maxHeight);
	int preEncode(Region* regionPtr, unsigned char * pixels);
	int merge(GifPreEncoder * other);
	int merge(Region* regionPtr, unsigned char * pixels);
	long encode(unsigned char* output, unsigned int outputSize);
	~GifPreEncoder(void);
	int getColorCount(){ return _colorCount; }
	int getWidth(){ return _width; }
	int getHeight(){ return _height; }
	//unsigned char** getScanlines(){ return _scanlines; }
	unsigned char* getR(){ return _R; }
	unsigned char* getG(){ return _G; }
	unsigned char* getB(){ return _B; }
	void reset(){
		_colorCount = 0;
		_width = 0;
		_height = 0;
		_scanlines->clear();
	}
private:
	static const int MAX_COLORS = 256;
	static const int BYTES_PER_PIXEL = 3;

	unsigned char* _pixels;
	//unsigned char** _scanlines;
	std::vector<unsigned char*>* _scanlines;
	unsigned char* _R;
	unsigned char* _G;
	unsigned char* _B;	
	int _maxWidth;
	int _maxHeight;
	int _colorCount;
	int _width;
	int _height;

};