
#include "HashString2.h"



//HashString22::HashString22(void)
//{
//}
HashString2::HashString2(const char * hash){
	this->pString = new std::string(hash);//memcpy(this->hash, hash, 32);
}
HashString2::HashString2(const HashString2 &other){
	this->pString = new std::string(other.pString->c_str());
    //memcpy(this->hash, other.hash, 32);
}
//HashString2 & HashString2::operator=(const HashString2 &rhs){
//	memcpy(this->hash, rhs.hash, 32);
//	return *this;
//}
bool HashString2::operator==(const HashString2 &other) const {
	return this->pString == other.pString;
    //dmemcmp(hash, other.hash, 32) == 0;
}
//bool HashString2::operator>(const HashString2 &other) const{
//	return memcmp(hash, other.hash, 32) > 0;
//}
//bool HashString2::operator<(const HashString2 &other) const{
//	return memcmp(hash, other.hash, 32) < 0;
//}
void HashString2::copyTo(char * buff, int offset) const{
	memcpy(buff+offset, this->pString->c_str(), 32);
}

