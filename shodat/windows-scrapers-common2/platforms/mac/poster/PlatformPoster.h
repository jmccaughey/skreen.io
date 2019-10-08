#pragma once
//#ifndef POSTER_H
//#define POSTER_H

#include <CoreFoundation/CoreFoundation.h>
#include <CoreServices/CoreServices.h>


class PlatformPoster{
	
public:
    PlatformPoster();
	~PlatformPoster();
	
	int sendPost(char* url, const unsigned char* data, unsigned long dataLength,
			unsigned char* response, unsigned long responseBufferLength, unsigned long *responseLength);
	
private:
	
	CFReadStreamRef		_stream;
    CFHTTPMessageRef	_request;
	CFURLRef			_urlRef;
	
	
	
};

//#endif
