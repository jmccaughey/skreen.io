/*
 *  Poster.cpp
 *  CorePoster
 *
 *  Created by John McCaughey on 8/18/13.
 *  Copyright 2013 __MyCompanyName__. All rights reserved.
 *
 */
#include <SystemConfiguration/SystemConfiguration.h>
#include "Poster.h"

Poster::Poster(){
	_request = NULL;
	_stream = NULL;
	_urlRef = NULL;
}

Poster::~Poster(){
	if(_request){
		CFRelease(_request);
	}
	if(_stream){
	    CFReadStreamClose(_stream);
		CFRelease(_stream);
	}
	if(_urlRef){
		CFRelease(_urlRef);
	}
}

int Poster::postData(CFStringRef url, const UInt8* data, unsigned int dataLength, UInt8* response, 
											unsigned int responseBufferLength, int *responseLength){
    if(_urlRef){
		CFRelease(_urlRef);
	}
	_urlRef = CFURLCreateWithString(NULL, url, NULL);
	CFHTTPMessageRef request = CFHTTPMessageCreateRequest(
		kCFAllocatorDefault, CFSTR("POST"), (CFURLRef)_urlRef, kCFHTTPVersion1_1);
		
    CFHTTPMessageRef old;
    
    // Swap the old request and the new request.  It is done in this
    // order since the new request could be the same as the existing
    // request.  If the old one is released first, it could be destroyed
    // before retain.
    old = _request;
    _request = (CFHTTPMessageRef)CFRetain(request);
    if (old)
        CFRelease(old);
    
    CFDataRef bodyData = CFDataCreateWithBytesNoCopy(NULL, data, dataLength, kCFAllocatorNull);
	CFHTTPMessageSetBody(_request, bodyData);
	CFReadStreamRef myReadStream = CFReadStreamCreateForHTTPRequest(NULL, _request);
	CFReadStreamSetProperty(myReadStream, 
	 						kCFStreamPropertyHTTPAttemptPersistentConnection, kCFBooleanTrue);
	
	// Get system-configured proxy settings and apply
	CFDictionaryRef proxyDict = SCDynamicStoreCopyProxies(NULL);
	CFReadStreamSetProperty(myReadStream, kCFStreamPropertyHTTPProxy, proxyDict);
	//CFShow(proxyDict);
	CFRelease(proxyDict);						
							
	CFReadStreamOpen(myReadStream);
	CFIndex numBytesRead = 0 ;
	CFIndex totalBytesRead = 0;
	do
	{	
		numBytesRead = CFReadStreamRead(myReadStream, response + totalBytesRead, 
							responseBufferLength - totalBytesRead);
		totalBytesRead += numBytesRead;		
	} while(numBytesRead > 0 && totalBytesRead < responseBufferLength);
	
	UInt8 extra[2048];
	while(numBytesRead > 0){
		numBytesRead = CFReadStreamRead(myReadStream, extra, sizeof(extra));
		totalBytesRead += numBytesRead;
	}
	
	
	CFHTTPMessageRef reply = (CFHTTPMessageRef) CFReadStreamCopyProperty(
													myReadStream,
													kCFStreamPropertyHTTPResponseHeader);
	
	//   Pull the status code from the headers
	UInt32 statusCode;
	if (reply) {
		statusCode = 
			CFHTTPMessageGetResponseStatusCode(reply);
		CFRelease(reply);
	}
	//printf("status code: %d\n", statusCode);
		
	if(_stream){
	    CFReadStreamClose(_stream);
		CFRelease(_stream);
	}	
	CFRelease(bodyData);
	_stream = myReadStream;
	*responseLength = totalBytesRead;
	return (statusCode >= 200 && statusCode < 300);
}