
#include "GifPreEncoder.h"

GifPreEncoder::GifPreEncoder(int maxWidth, int maxHeight){
	_maxWidth = maxWidth;
	_maxHeight = maxHeight;
	int pixelsLength = maxWidth * maxHeight;
	printf("mallocing %d for pixels\n", pixelsLength);
	_pixels = (unsigned char*)malloc(pixelsLength);
	_R = (unsigned char*)malloc(MAX_COLORS);
	_G = (unsigned char*)malloc(MAX_COLORS);
	_B = (unsigned char*)malloc(MAX_COLORS);
	//int scanlinesLength = sizeof(unsigned char *) * maxHeight * 4;
	printf("maxHeight %d\n", maxHeight);
	//printf("mallocing %d for scanlines\n", scanlinesLength);
	//_scanlines = (unsigned char**)malloc(scanlinesLength);
	_scanlines = new std::vector<unsigned char*>();
	_colorCount = 0;
	_width = 0;
	_height = 0;
}
GifPreEncoder::~GifPreEncoder(void){
	free(_pixels);
	free(_R); free(_G); free(_B);
	_scanlines->clear();
	delete _scanlines;
}

int GifPreEncoder::merge(Region* regionPtr, unsigned char * pixels){
	if(_width != 0 && regionPtr->getWidth() != _width){
		printf("widths do not match %d %d\n", regionPtr->getWidth(), _width);
		return 0;
	}
	if(_height + regionPtr->getHeight() > MAX_HEIGHT){
		printf("height exceeds limit %d %d\n", regionPtr->getHeight() + _height, MAX_HEIGHT);
		return 0;
	}
	// note current color count
	int originalColorCount = _colorCount;
	int originalScanlinesCount = _scanlines->size();
	int pixelCount = _width * _height;
	if(_width == 0){	
		_width = regionPtr->getWidth();
		_height = regionPtr->getHeight();
	}else{
		_height += regionPtr->getHeight();
	}
	int otherWidth = regionPtr->getWidth();
	int otherHeight = regionPtr->getHeight();
	unsigned char red, green, blue;
	int scln, colorIndex;	
	for(scln = 0; scln < otherHeight; scln++){
		unsigned char * row = regionPtr->getScanline(scln, pixels);
		int rowBytes = _width * BYTES_PER_PIXEL;
		_scanlines->push_back(_pixels + pixelCount);
		for(int j = 0; j < rowBytes;){
			blue = row[j++]; green = row[j++]; red = row[j++];
			colorIndex = -1;
			for(int c = 0; c < _colorCount; c++){
				// lossy: accepts "close enough" colors
				if(red >> 2 == _R[c] >> 2 && green >> 2 == _G[c] >> 2 && blue >> 2 == _B[c] >> 2){
					colorIndex = c;
					break;
				}
			}
			if(colorIndex == -1){
				if(_colorCount == MAX_COLORS){					
					_colorCount = originalColorCount;
					int scanlinesToRemove = _scanlines->size() - originalScanlinesCount;
					_scanlines->erase(_scanlines->begin() + originalScanlinesCount, _scanlines->end());
					_height = _height - regionPtr->getHeight();
					printf("too many colors at %d %d\n", regionPtr->getX(), regionPtr->getY());
					return originalColorCount == 0 ? -1 : 0;
				}
				_R[_colorCount] = red; _G[_colorCount] = green; _B[_colorCount] = blue;
				colorIndex = _colorCount++;
			}
			_pixels[pixelCount++] = (unsigned char)colorIndex;
		}
	}
	printf("%d colors\n", _colorCount);
	return _colorCount;
}
long GifPreEncoder::encode(unsigned char* output, unsigned int outputSize){
	if(_scanlines->empty()){
		printf("nothing to encode\n");
		return -1;
	}	
	long result = ::GIFencode(_width, _height, _colorCount, _R, _G, _B,
		&(_scanlines->at(0)), // item 16, Effective STL Seth Meyers
		output);
	return result;
}
