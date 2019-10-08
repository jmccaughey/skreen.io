#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include "Region.h"
#include "GifSpriteInfo.h"

#ifdef __cplusplus
extern "C" {
#endif
long GIFencode(int Width, int Height, int Ncol,
		unsigned char* R, unsigned char* G, unsigned char* B, 
		unsigned char** scanlines, unsigned char* outputBuffer);
#ifdef __cplusplus
}
#endif

class GifSpriter{
    
public:
    static const int MAX_HEIGHT = 4096;
    static const int MAX_GIF_BYTES = 1024 * 1024;
	GifSpriter(int maxWidth, int maxHeight);
	void makeSprites(std::vector<deskblast::Region*>* regions, unsigned char * pixels, 
		std::vector<GifSpriteInfo*>* spriteInfos, 
		std::vector<deskblast::Region*>* ungiffableRegions);
	~GifSpriter(void);
	
private:
	static const int MAX_COLORS = 256;
/*
	static const int BYTES_PER_PIXEL = 
#ifdef _WINDOWS
	3
#else
    4
#endif
	;
*/	
	static const int LOSSY_FACTOR = 3; // TODO: fixme was 2
	static const int LOSSY_FACTOR_WHITE = 2;  

	unsigned char* _pixels;
	unsigned char * _encoded;
	std::vector<unsigned char*>* _scanlines;
	std::vector<MappedRegion>* _mappedRegions;
	unsigned char* _R;
	unsigned char* _G;
	unsigned char* _B;	
	int _maxWidth;
	int _maxHeight;
	int _colorCount;
	int _width;
	int _height;
	void reset(){
		_colorCount = 0;
		_width = 0;
		_height = 0;
		_scanlines->clear();
		_mappedRegions->clear();
	}
	int merge(deskblast::Region* regionPtr, unsigned char * pixels);
	long encode(unsigned char* output, unsigned int outputSize);
};
