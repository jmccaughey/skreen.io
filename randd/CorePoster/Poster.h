/*
 *  Poster.h
 *  CorePoster
 *
 *  Created by John McCaughey on 8/18/13.
 *  Copyright 2013 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef POSTER_H
#define POSTER_H

#include <CoreFoundation/CoreFoundation.h>
#include <CoreServices/CoreServices.h>


class Poster{
	
public:
    Poster();
	~Poster();
	
	int postData(CFStringRef url, const UInt8* data, unsigned int dataLength, 
			UInt8* response, unsigned int responseBufferLength, int *responseLength);
	
private:
	
	CFReadStreamRef		_stream;
    CFHTTPMessageRef	_request;
	CFURLRef			_urlRef;
	
	
	
};

#endif