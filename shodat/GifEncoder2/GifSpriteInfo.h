#ifndef GIF_SPRITE_INFO_H
#define GIF_SPRITE_INFO_H

#include <vector>
#include "../windows-scrapers-common2/MappedRegion.h"

class GifSpriteInfo
{
public:

	GifSpriteInfo(unsigned char * gifStart, int gifLength, std::vector<MappedRegion>* mappedRegions){
		_gifStart = gifStart;
		_gifLength = gifLength;
		_mappedRegions = new std::vector<MappedRegion>();
		for(unsigned int i = 0; i < mappedRegions->size(); i++){
			_mappedRegions->push_back(mappedRegions->at(i));
		}
	}

	~GifSpriteInfo(){
		_mappedRegions->clear();
		delete _mappedRegions;
	}

	unsigned char * getGifStart(){ return _gifStart; }
	int getGifLength(){ return _gifLength; }
	std::vector<MappedRegion>* getMappedRegions(){ return _mappedRegions; }

private:
	unsigned char * _gifStart;
	int _gifLength;
	std::vector<MappedRegion>* _mappedRegions;
};
#endif /* GIF_SPRITE_INFO */
