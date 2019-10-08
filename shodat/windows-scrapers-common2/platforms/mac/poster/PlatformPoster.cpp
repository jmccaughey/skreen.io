#include <SystemConfiguration/SystemConfiguration.h>
#include "PlatformPoster.h"

PlatformPoster::PlatformPoster(){
	_request = NULL;
	_stream = NULL;
	_urlRef = NULL;
}

PlatformPoster::~PlatformPoster(){
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

int PlatformPoster::sendPost(char* url, const unsigned char* data, unsigned long dataLength,
			unsigned char* response, unsigned long responseBufferLength, unsigned long *responseLength){
    if(_urlRef){
		CFRelease(_urlRef);
	}
	CFStringRef urlStringRef = CFStringCreateWithCString(NULL, url, kCFStringEncodingUTF8);
	_urlRef = CFURLCreateWithString(NULL, urlStringRef, NULL);
	CFRelease(urlStringRef);
	CFHTTPMessageRef request = CFHTTPMessageCreateRequest(
		kCFAllocatorDefault, CFSTR("POST"), (CFURLRef)_urlRef, kCFHTTPVersion1_1);
    //CFRelease(urlRef);
    if(request == NULL){
        printf("CFHTTPMessageCreateRequest: got null result from createrequest\n");
    }
    CFHTTPMessageRef old;
    
    // Swap the old request and the new request.  It is done in this
    // order since the new request could be the same as the existing
    // request.  If the old one is released first, it could be destroyed
    // before retain.
    old = _request;
    _request = (CFHTTPMessageRef)CFRetain(request);
    if (old){
        CFRelease(old);
//        printf("CFHTTPMessageCreateRequest: released\n");
    }
    
    CFDataRef bodyData = CFDataCreateWithBytesNoCopy(NULL, data, dataLength, kCFAllocatorNull);
	CFHTTPMessageSetBody(_request, bodyData);
	CFReadStreamRef myReadStream = CFReadStreamCreateForHTTPRequest(NULL, _request);
    if(myReadStream == NULL){
        printf("got null result from readstream create\n");
    }
	CFReadStreamSetProperty(myReadStream,
	 						kCFStreamPropertyHTTPAttemptPersistentConnection, kCFBooleanTrue);
	
	// Get system-configured proxy settings and apply
	CFDictionaryRef proxyDict = SCDynamicStoreCopyProxies(NULL);
	CFReadStreamSetProperty(myReadStream, kCFStreamPropertyHTTPProxy, proxyDict);
	//CFShow(proxyDict);
	CFRelease(proxyDict);					
	bool success = CFReadStreamOpen(myReadStream);
	if(!success){
        printf("CFReadStreamOpen fail\n");
    }
    CFIndex numBytesRead = 0 ;
	CFIndex totalBytesRead = 0;
	do
	{	
		numBytesRead = CFReadStreamRead(myReadStream, response + totalBytesRead, 
							responseBufferLength - totalBytesRead);
		totalBytesRead += numBytesRead;
	} while(numBytesRead > 0 && totalBytesRead < responseBufferLength);
	
    if(totalBytesRead > 0){
    	UInt8 extra[4096];
	    // drain stream if it still has more
        while(numBytesRead > 0){
		    numBytesRead = CFReadStreamRead(myReadStream, extra, sizeof(extra));
		    totalBytesRead += numBytesRead;
	    }
	    CFHTTPMessageRef reply = (CFHTTPMessageRef) CFReadStreamCopyProperty(
													myReadStream,
													kCFStreamPropertyHTTPResponseHeader);
	    //   Pull the status code from the headers
	    CFIndex statusCode = 0;
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
    }else{
        return FALSE;
    }
}
