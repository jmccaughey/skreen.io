#include "SubtileInfo.h"

#include <string.h>

SubtileInfo::SubtileInfo(void)
{
}
SubtileInfo::~SubtileInfo(void)
{
}
SubtileInfo::SubtileInfo(const char * hash, int x, int y){ 
	memcpy(_hash, hash, 32); 
	_x = x;
	_y = y;
}

SubtileInfo & SubtileInfo::operator=(const SubtileInfo &rhs){
	memcpy(_hash, rhs._hash, 32);
	_x = rhs._x;
	_y = rhs._y;
	return *this;
}
bool SubtileInfo::operator==(const SubtileInfo &other) const {
	return memcmp(_hash, other._hash, 32) == 0;
}
bool SubtileInfo::operator>(const SubtileInfo &other) const{
	return memcmp(_hash, other._hash, 32) > 0;
}
void SubtileInfo::copyTo(char* buff, int offset){
	memcpy(buff+offset, _hash, 32);
}
