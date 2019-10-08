#pragma once

#include <curl/curl.h>
#include <stdio.h>
#include <dlfcn.h>
#include <string>
#include <stdlib.h>
#include <sstream>

#define RESPONSE_LENGTH 2048

#define TRACE_FILE "shodat_net.bin"

class CurlConnection {
public:
    static size_t curlDataCallback(void * buffer, size_t size, size_t number, void* context);
    size_t handleData(void * buffer, size_t size, size_t number);
    static size_t curlProgressCallback(void *clientp,
                                       double dltotal,
                                       double dlnow,
                                       double ultotal,
                                       double ulnow);
    size_t handleProgress(double dltotal, double dlnow, double ultotal, double ulnow);
    CurlConnection(std::string host, unsigned int port, bool ssl);
    CurlConnection(std::string schemeHostPort);
    int sendPost(const char* path, char * data, int length, char * response,
                 int responseBufferLength, size_t* responseLength);
    bool sendTimedHead(const char* path, char * response,
                       int responseBufferLength, size_t* responseLength, double * seconds);
   	void interrupt();
    char* getResponse(){ return _pResponse; }
    const std::string getServer() const{ return schemeHostPort; }
    ~CurlConnection(void);
private:
    std::string schemeHostPort;
    char * _pResponse;
    size_t _responseBufferLength;
    size_t _responseCursor;
    CURL * _curl;
    void cleanup(void);
    bool stopRequested;
    bool initialize(void);    
    void* lib_handle;
    CURL*(*curl_easy_init)(void);
    char*(*curl_easy_strerror)(CURLcode);
    CURLcode(*curl_easy_setopt)(CURL*, CURLoption, ...);
    CURLcode(*curl_easy_perform)(CURL*);
    struct curl_slist*(*curl_slist_append)(struct curl_slist *, const char *);
    void(*curl_slist_free_all)(struct curl_slist *);
    void(*curl_easy_reset)(CURL*);
    CURLcode(*curl_easy_getinfo)(CURL *curl, CURLINFO info, ...);
    void(*curl_easy_cleanup)(CURL*);
    
};

