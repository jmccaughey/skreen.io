
#include "GifSpriter.h"

#include <algorithm>

GifSpriter::GifSpriter(int maxWidth, int maxHeight){
	_maxWidth = maxWidth;
	_maxHeight = maxHeight;
	int pixelsLength = maxWidth * maxHeight;
	printf("mallocing %d for pixels\n", pixelsLength);
	_pixels = (unsigned char*)malloc(pixelsLength);
	_R = (unsigned char*)malloc(MAX_COLORS);
	_G = (unsigned char*)malloc(MAX_COLORS);
	_B = (unsigned char*)malloc(MAX_COLORS);
	_encoded = (unsigned char*)malloc(MAX_GIF_BYTES);
	//int scanlinesLength = sizeof(unsigned char *) * maxHeight * 4;
	printf("maxHeight %d\n", maxHeight);
	//printf("mallocing %d for scanlines\n", scanlinesLength);
	//_scanlines = (unsigned char**)malloc(scanlinesLength);
	_scanlines = new std::vector<unsigned char*>();
	_mappedRegions = new std::vector<MappedRegion>();
	_colorCount = 0;
	_width = 0;
	_height = 0;
}
GifSpriter::~GifSpriter(void){
	free(_pixels);
	free(_R); free(_G); free(_B);
	_scanlines->clear();	
	delete _scanlines;
	_mappedRegions->clear();
	delete _mappedRegions;
	free(_encoded);
}

void GifSpriter::makeSprites(std::vector<deskblast::Region*>* regionVector, unsigned char * bmp, 
		std::vector<GifSpriteInfo*>* spriteInfos,
		std::vector<deskblast::Region*>* ungiffableRegions){
    //std::vector<Region*> regionVector;
    //for(int i = 0; i < regionCount; i++){
    //	regionVector.push_back(regions[i]);
    //}
    //int outputCursor = 0;
    int result;
    int ungiffable = 0;
    int gifLength;
    int totalBytes = 0;
    int gifOutCursor = 0;
    while(!regionVector->empty()){
    	  std::vector<int> eraseIndices;    	
    	  for(unsigned int i = 0; i < regionVector->size(); i++){
    	  	  deskblast::Region* rptr = regionVector->at(i);  
    	  	  printf("attempting to GIF merge x: %d y: %d\n", rptr->getX(), rptr->getY());  
			  result = merge(rptr, bmp);
			  if(result == -1){
			  	printf("merge %d failed - permanently ungiffable/too many colors\n", i);
			  	ungiffable++;
			  	eraseIndices.push_back(i);
			  	printf("could not gif x: %d y: %d width: %d height: %d index %d ungiffable: %d\n",
			  	        rptr->getX(), rptr->getY(), rptr->getWidth(), rptr->getHeight(), i, ungiffable);
			  	// hack: only add region if not already present
			  	// TODO: figure out why region would be added more than once in the first place
			  	if(std::find(ungiffableRegions->begin(), ungiffableRegions->end(), rptr)
			  	        != ungiffableRegions->end()) {
					printf("would add region already added x: %d y: %d\n", rptr->getX(), rptr->getY());
			  	}else{
			  		printf("adding region to ungiffables x: %d y: %d\n", rptr->getX(), rptr->getY());
			  	    ungiffableRegions->push_back(rptr);
			  	}
		 	  }else if(result == 0){
		 	  	printf("merge %d failed - wrong dimensions or incompatible palette\n", i);
		 	  	// leave in collection for next pass
			  }else{
			  	printf("returned from merge %d\n", i);
			  	eraseIndices.push_back(i);
			  }
			  printf("regionVector->size() == %lu\n", regionVector->size());
		  }
		  printf("erasing %lu items\n", eraseIndices.size());
		  for(int i = eraseIndices.size()-1; i >= 0 ; i--){
		    int eraseIndex = eraseIndices.at(i);
		  	printf("erasing %d x: %d y: %d width: %d height: %d\n", i, 
		  		regionVector->at(eraseIndex)->getX(), regionVector->at(eraseIndex)->getY(),
		  		regionVector->at(eraseIndex)->getWidth(), regionVector->at(eraseIndex)->getHeight());
		  	regionVector->erase(regionVector->begin() + eraseIndex);
		  }
		  eraseIndices.clear();
		  // call callback, with meta data and pointer to pointer to output
		  // callback writes non-back ref metadata, returns 
		  // gif encode to buffer pointer
		  // call callback for following backrefs  
		  unsigned char * gifStart = _encoded + gifOutCursor;
    	  gifLength = encode(gifStart, MAX_GIF_BYTES - gifOutCursor);
    	  if(gifLength > 0){
    	  	spriteInfos->push_back(new GifSpriteInfo(gifStart, gifLength, _mappedRegions));
    	  	gifOutCursor += gifLength;
    	  }
    	  //spriteHandler->handleSprite(_encoded, gifLength, _mappedRegions);
    	  reset();
    	  printf("returned from encode\n");
		  if(gifLength > 0){
			printf("wrote %d bytes\n", gifLength);
			//sprintf(outFile, "out_%d.gif", fileCount++);
			//FILE* fpOut = fopen(outFile, "wb");
			//fwrite(output, 1, gifLength, fpOut);
			//fclose(fpOut);
			totalBytes += gifLength;
		  }else{
		    printf("problem encoding\n");
		  }
    }
    // return vector of non-giffable Regions	
}

int GifSpriter::merge(deskblast::Region* regionPtr, unsigned char * pixels){
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
	int otherHeight = regionPtr->getHeight();
	unsigned char red, green, blue;
	int scln, colorIndex;	
	for(scln = 0; scln < otherHeight; scln++){
		unsigned char * row = regionPtr->getScanline(scln, pixels);
		int rowBytes = _width * deskblast::Region::BYTES_PER_PIXEL;
		_scanlines->push_back(_pixels + pixelCount);
		for(int j = 0; j < rowBytes;){
			blue = row[j++]; green = row[j++]; red = row[j++];
//#if defined(_WINDOWS)
//#else			
			j++; // TODO: fixme: for xwindows 4 byte pixels
//#endif			
			colorIndex = -1;
			for(int c = 0; c < _colorCount; c++){
				// lossy: accepts "close enough" colors, except white
				if(red == 255 && green == 255 && blue == 255){
					if(red >> LOSSY_FACTOR_WHITE == _R[c] >> LOSSY_FACTOR_WHITE
							&& green >> LOSSY_FACTOR_WHITE == _G[c] >> LOSSY_FACTOR_WHITE
							&& blue >> LOSSY_FACTOR_WHITE == _B[c] >> LOSSY_FACTOR_WHITE){
							colorIndex = c;
							break;
					}
				}else if(red >> LOSSY_FACTOR == _R[c] >> LOSSY_FACTOR
					&& green >> LOSSY_FACTOR == _G[c] >> LOSSY_FACTOR 
					&& blue >> LOSSY_FACTOR == _B[c] >> LOSSY_FACTOR){
					colorIndex = c;
					break;
				}
			}
			if(colorIndex == -1){
				if(_colorCount == MAX_COLORS){					
					_colorCount = originalColorCount;
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
	// merge success -- note MappedRegion
	_mappedRegions->push_back(MappedRegion(regionPtr, 0, _height - (regionPtr->getHeight())));
	printf("%d colors\n", _colorCount);
	return _colorCount;
}
long GifSpriter::encode(unsigned char* output, unsigned int outputSize){
	if(_scanlines->empty()){
		printf("nothing to encode\n");
		return -1;
	}	
	long result = ::GIFencode(_width, _height, _colorCount, _R, _G, _B,
		&(_scanlines->at(0)), // item 16, Effective STL Seth Meyers
		output);
	return result;
}
