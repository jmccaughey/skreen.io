
#include "GifEncoder2.h"


GifEncoder2::GifEncoder2(int maxWidth, int maxHeight){
	int _maxWidth = maxWidth;
	int _maxHeight = maxHeight;
	_pixels = (unsigned char*)malloc(maxWidth * maxHeight);
	_R = (unsigned char*)malloc(MAX_COLORS); 
	_G = (unsigned char*)malloc(MAX_COLORS); 
	_B = (unsigned char*)malloc(MAX_COLORS);
	_scanlines = (unsigned char**)malloc(sizeof(unsigned char *) * maxHeight);
}
GifEncoder2::~GifEncoder2(void){
	free(_pixels);
	free(_R); free(_G); free(_B);
	free(_scanlines);
}
long GifEncoder2::encode(
	deskblast::Region* regionPtr, 
	unsigned char* pixels, 
	unsigned char* output, 
	unsigned int outputSize){

	int colorCount = 0;
        //printf("getting width\n");
	int width = regionPtr->getWidth();
	int height = regionPtr->getHeight();
	//printf("region width: %d\n", width);
	//printf("region height: %d\n", height);
	
	unsigned char red, green, blue;	
	int scln, colorIndex;
	int pixelCount = 0;
	for(scln = 0; scln < height; scln++){
		unsigned char * row = regionPtr->getScanline(scln, pixels);
		int rowBytes = width * BYTES_PER_PIXEL;
		_scanlines[scln] = _pixels + pixelCount;
		for(int j = 0; j < rowBytes;){
			blue = row[j++];
			green = row[j++];
			red = row[j++];
			j++; // TODO: fixme added for alpha support
			colorIndex = -1;
			for(int c = 0; c < colorCount; c++){
				// lossy: accepts "close enough" colors
				// TODO: fixme was >> 2
				if(red >> LOSSY_FACTOR_RED == _R[c] >> LOSSY_FACTOR_RED 
				        && green >> LOSSY_FACTOR_GREEN == _G[c] >> LOSSY_FACTOR_GREEN 
				        && blue >> LOSSY_FACTOR_BLUE == _B[c] >> LOSSY_FACTOR_BLUE){
					colorIndex = c;
					//printf("found 'close enough'\n");
					break;
				}
			}	
			if(colorIndex == -1){
				if(colorCount == MAX_COLORS){
					return -1;
				}
				_R[colorCount] = red; _G[colorCount] = green; _B[colorCount] = blue;
				colorIndex = colorCount++;
			}
			_pixels[pixelCount++] = colorIndex;			
		}		
	}
	printf("%d colors\n", colorCount);
	return ::GIFencode(width, height, colorCount, _R, _G, _B, _scanlines, output);
}
int GifEncoder2::merge(deskblast::Region* regionPtr, unsigned char * pixels){
    // do everything short of the call to ::GIFencode
    // note current scanline index
    // current color index
    // if no room for merge, reset both to noted values
    return 0;

}

// TODO: encodeMultiple(Region*[] regionPtrs, pixels, output, otputSize)
// check if widths are equal (if not return -1)
// loop on regions
// loop as above
// height is all heights added together
long GifEncoder2::encodeMultiple(
	deskblast::Region** regionPtr,
	int regionCount, 
	unsigned char* pixels, 
	unsigned char* output, 
	unsigned int outputSize){

	int colorCount = 0;
        printf("getting width\n");
	int width = regionPtr[0]->getWidth();
	int height = regionPtr[0]->getHeight();
	printf("region width: %d\n", width);
	printf("region height: %d\n", height);
	
	unsigned char red;
	unsigned char green;
	unsigned char blue;	
	int scln = 0;
	int pixelCount = 0;
	int colorIndex;
	for(int i = 0; i < regionCount; i++){ 	
		for(scln = 0; scln < height; scln++){
			unsigned char * row = regionPtr[i]->getScanline(scln, pixels);
			int rowBytes = width * BYTES_PER_PIXEL;
			_scanlines[scln+(i*height)] = _pixels + pixelCount;
			for(int j = 0; j < rowBytes;){
				blue = row[j++];
				green = row[j++];
				red = row[j++];
				colorIndex = -1;
				for(int c = 0; c < colorCount; c++){
					// lossy: accepts "close enough" colors
					if(red >> LOSSY_FACTOR_RED == _R[c] >> LOSSY_FACTOR_RED && green >> LOSSY_FACTOR_GREEN == _G[c] >> LOSSY_FACTOR_GREEN && blue >> LOSSY_FACTOR_BLUE == _B[c] >> LOSSY_FACTOR_BLUE){
						colorIndex = c;
						break;
					}
				}	
				if(colorIndex == -1){
					if(colorCount == MAX_COLORS){
						return -1;
					}
					_R[colorCount] = red; _G[colorCount] = green; _B[colorCount] = blue;
					colorIndex = colorCount++;
				}
				_pixels[pixelCount++] = colorIndex;	
				//pixelCount++;
			}		
		}
	}
	printf("%d colors\n", colorCount);
	return ::GIFencode(width, height*regionCount, colorCount, _R, _G, _B, _scanlines, output);
}

