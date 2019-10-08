#pragma once

#include <curl/curl.h>
#include <stdio.h>
#include <string.h>
//#include "PostJob.h"

#define RESPONSE_LENGTH 1024

#define TRACE_FILE "shodat_net.bin"

class PlatformPoster {
public:
	static size_t curlDataCallback(void * buffer, size_t size, size_t number, void* context);
	size_t handleData(void * buffer, size_t size, size_t number);
	PlatformPoster();
	int sendPost(char* url, char * data, int length, char * response,
			int responseBufferLength, int* responseLength);
	void interrupt();
	~PlatformPoster(void);
private:
    //char url[4096];
    unsigned char response[RESPONSE_LENGTH];
    size_t responseCursor;
    CURL *curl;
    struct curl_slist *chunk;
	void cleanup(void);	
};
