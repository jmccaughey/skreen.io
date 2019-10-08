//
//  HashString2.h
//  HashString2
//
//  Created by John McCaughey on 10/8/13.
//  Copyright (c) 2013 John McCaughey. All rights reserved.
//

#ifndef __HashString2__HashString2__
#define __HashString2__HashString2__

#include <iostream>
#include <string>


class HashString2
{
public:
	HashString2(const char * hash);
	HashString2(void);
	HashString2(const HashString2 &other);
	//const char* getHash();
	HashString2 & operator=(const HashString2 &rhs);
	bool operator==(const HashString2 &other) const;
	//bool operator>(const HashString2 &other) const;
	//bool operator<(const HashString2 &other) const;
	void copyTo(char * buff, int offset) const;
    size_t hash();
    bool is_empty(){ return false; }
private:
    std::string* pString;
};


#endif /* defined(__HashString2__HashString2__) */
