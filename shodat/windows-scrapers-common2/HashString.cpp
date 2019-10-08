#include "HashString.h"

#include <string.h>

HashString::HashString(void)
{
}
HashString::HashString(const char * hash){ 
	memcpy(this->hash, hash, 32);
}
HashString::HashString(const HashString &other){
	memcpy(this->hash, other.hash, 32);
}
HashString & HashString::operator=(const HashString &rhs){
	memcpy(this->hash, rhs.hash, 32);
	return *this;
}
bool HashString::operator==(const HashString &other) const {
	return memcmp(hash, other.hash, 32) == 0;
}
bool HashString::operator>(const HashString &other) const{
	return memcmp(hash, other.hash, 32) > 0;
}
bool HashString::operator<(const HashString &other) const{
	return memcmp(hash, other.hash, 32) < 0;
}
void HashString::copyTo(char * buff, int offset) const{
	memcpy(buff+offset, this->hash, 32);
}
