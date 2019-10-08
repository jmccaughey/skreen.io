#pragma once
class HashString
{	
public:
	HashString(const char * hash);
	HashString(void);
	HashString(const HashString &other);	
	const char* getHash(){ return hash; }
	HashString & operator=(const HashString &rhs);
	bool operator==(const HashString &other) const;
	bool operator>(const HashString &other) const;
	bool operator<(const HashString &other) const;
	void copyTo(char * buff, int offset) const;
private:
	char hash[32];
};
