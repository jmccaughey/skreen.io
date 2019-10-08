#ifndef MAPPED_REGION_H
#define MAPPED_REGION_H

#include <vector>
#include "Region.h"

class MappedRegion
{
public:

	MappedRegion(deskblast::Region * region, int translateX, int translateY){
		_region = region;
		_translateX = translateX;
		_translateY = translateY;
	}

	deskblast::Region * getRegion(){ return _region; }
	int getTranslateX(){ return _translateX; }
	int getTranslateY(){ return _translateY; }
private:
	deskblast::Region * _region;
	int _translateX;
	int _translateY;
	
};
#endif /* MAPPED_REGION_H */
