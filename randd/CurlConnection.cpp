#include "CurlConnection.h"

void p_trace(const char *msg) {
}

size_t CurlConnection::curlProgressCallback(void *clientp,
                                        double dltotal,
                                        double dlnow,
                                        double ultotal,
                                        double ulnow){
    return static_cast<CurlConnection*>(clientp)->handleProgress(dltotal, dlnow, ultotal, ulnow);
}

size_t CurlConnection::curlDataCallback(void * buffer, size_t size, size_t number, void* context){
    return static_cast<CurlConnection*>(context)->handleData(buffer, size, number);
}
size_t CurlConnection::handleData(void * buffer, size_t size, size_t number){
    printf("handleData size:%ld number:%ld\n", size, number);
    size_t pending = size * number;
    size_t remaining = RESPONSE_LENGTH - _responseCursor;
    size_t copyAmount = std::min(remaining, pending);
    printf("got %lu bytes\n", size * number);
    printf("copying %lu\n", copyAmount);
    if(copyAmount + _responseCursor > this->_responseBufferLength){
        printf("response buffer too small: %lu \n ", this->_responseBufferLength);
        return 0;
    }
    memcpy(this->_pResponse + _responseCursor, buffer, copyAmount);
    _responseCursor += copyAmount;
    return copyAmount;
}
size_t CurlConnection::handleProgress(double dltotal, double dlnow, double ultotal, double ulnow){
    printf("got progress callback\n");
    // return non-zero to abort transfer
    return this->stopRequested;
}
CurlConnection::CurlConnection(std::string schemeHostPort) {
    this->lib_handle = NULL;
    _responseCursor = 0;
    _responseBufferLength = 0;
    this->_curl = NULL;
    this->_pResponse = NULL;
    this->stopRequested = false;
    this->schemeHostPort = schemeHostPort;
    p_trace(__DATE__);
    p_trace(__TIME__);
}

bool CurlConnection::initialize(){
    this->lib_handle = dlopen("libcurl.dylib", RTLD_NOW);
    if(!this->lib_handle){
        printf("dlopen failed\n");
        return false;
    }
    this->curl_easy_init = (CURL*(*)(void)) dlsym(lib_handle, "curl_easy_init");
    this->curl_easy_setopt = (CURLcode(*)(CURL*, CURLoption, ...)) dlsym(lib_handle, "curl_easy_setopt");
    this->curl_easy_perform = (CURLcode(*)(CURL*)) dlsym(lib_handle, "curl_easy_perform");
    this->curl_easy_strerror = (char*(*)(CURLcode)) dlsym(lib_handle, "curl_easy_strerror");
    this->curl_slist_append = (struct curl_slist*(*)(struct curl_slist *, const char *))
    dlsym(lib_handle, "curl_slist_append");
    this->curl_slist_free_all = (void(*)(struct curl_slist *)) dlsym(lib_handle, "curl_slist_free_all");
    this->curl_easy_getinfo = (CURLcode(*)(CURL*, CURLINFO, ...)) dlsym(lib_handle, "curl_easy_getinfo");
    this->curl_easy_cleanup = (void(*)(CURL*)) dlsym(lib_handle, "curl_easy_cleanup");
    this->curl_easy_reset = (void(*)(CURL*)) dlsym(lib_handle, "curl_easy_reset");
    _curl = this->curl_easy_init();
    if(_curl){
        printf("curl_easy_init success\n");
    }else{
        printf("curl_easy_init fail\n");
    }
    return true;
}

int CurlConnection::sendPost(const char* path, char * data, int length, char * response,
                         int responseBufferLength, size_t* responseLength) {
    
    /*
     FILE * pFile = fopen (TRACE_FILE, "ab");
     if (pFile!=NULL){
     for(int i = 0; i < length; i++){
     fputc(data[i], pFile);
     }
     fclose (pFile);
     }
     */
    
    CURLcode res;
    
    if(this->lib_handle == NULL){
        if(!initialize()){
            fprintf(stderr, "could not load libcurl\n");
            return 0;
        }
        printf("loaded libcurl\n");
    }
    
    this->_pResponse = response;
    this->_responseBufferLength = responseBufferLength;
    std::string url = this->schemeHostPort + path;
    printf("url: %s\n", url.c_str());
    curl_easy_reset(_curl);
    curl_easy_setopt(_curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(_curl, CURLOPT_POST, 1L);
    curl_easy_setopt(_curl, CURLOPT_WRITEFUNCTION, CurlConnection::curlDataCallback);
    curl_easy_setopt(_curl, CURLOPT_WRITEDATA, this);
    curl_easy_setopt(_curl, CURLOPT_POSTFIELDS, data);
    curl_easy_setopt(_curl, CURLOPT_NOPROGRESS, 0);
    curl_easy_setopt(_curl, CURLOPT_PROGRESSFUNCTION,
                     CurlConnection::curlProgressCallback);
    curl_easy_setopt(_curl, CURLOPT_PROGRESSDATA, this);
    curl_easy_setopt(_curl, CURLOPT_INFILESIZE_LARGE, (curl_off_t)length);
    struct curl_slist *slist = NULL;
    slist = curl_slist_append(slist, "Expect:"); // set this to blank to disable it, otherwise curl will enable
    if (slist == NULL)
        return -1;
    curl_easy_setopt(_curl, CURLOPT_HTTPHEADER, slist);
    //curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    curl_easy_setopt(_curl, CURLOPT_POSTFIELDSIZE, length);
    curl_easy_setopt(_curl, CURLOPT_TCP_NODELAY, 1L);
    printf("about to curl_easy_perform\n");
    res = curl_easy_perform(_curl);
    curl_slist_free_all(slist);
    printf("returned from curl_easy_perform\n");
    if(res != CURLE_OK){
        printf("curl_easy_perform() failed: %s\n",
               curl_easy_strerror(res));
    }
    *responseLength = _responseCursor;
    _responseCursor = 0;
    return res;
}
bool CurlConnection::sendTimedHead(const char* path, char * response,
                               int responseBufferLength, size_t* responseLength, double * seconds){
    
    CURLcode res;
    if(this->lib_handle == NULL){
        if(!initialize()){
            printf("could not load libcurl\n");
            return 0;
        }
        printf("loaded libcurl\n");
    }
    double lookup;
    double connect;
    curl_easy_getinfo(_curl, CURLINFO_CONNECT_TIME, &connect);
    curl_easy_getinfo(_curl, CURLINFO_NAMELOOKUP_TIME, &lookup);
    if(connect > 0){
        printf("already connected\n");
        *seconds = connect - lookup;
        return true;
    }
    this->_pResponse = response;
    this->_responseBufferLength = responseBufferLength;
    std::string url = this->schemeHostPort + path;
    printf("url: %s\n", url.c_str());
    curl_easy_reset(_curl);
    curl_easy_setopt(_curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(_curl, CURLOPT_NOBODY, 1L);
    curl_easy_setopt(_curl, CURLOPT_WRITEFUNCTION, CurlConnection::curlDataCallback);
    curl_easy_setopt(_curl, CURLOPT_WRITEDATA, this);
    curl_easy_setopt(_curl, CURLOPT_TCP_NODELAY, 1L);
    printf("about to curl_easy_perform %s\n", url.c_str());
    res = curl_easy_perform(_curl);
    printf("returned from curl_easy_perform %s\n", url.c_str());
    if(res != CURLE_OK){
        printf("curl_easy_perform() failed: %s\n",
               curl_easy_strerror(res));
        return false;
    }
    curl_easy_getinfo(_curl, CURLINFO_NAMELOOKUP_TIME, &lookup);
    printf("lookup: %f for %s\n", lookup, url.c_str());
    curl_easy_getinfo(_curl, CURLINFO_CONNECT_TIME, &connect);
    *seconds = connect - lookup;
    return true;
}

void CurlConnection::interrupt() {
    this->stopRequested = true;
}
void CurlConnection::cleanup(void) {
    printf("CurlConnection::cleanup\n");
    this->stopRequested = true;
    if (this->_curl) {
        printf("about to curl_easy_cleanup\n");
        curl_easy_cleanup(this->_curl);
        printf("returned from  curl_easy_cleanup\n");
    }
    this->_curl = NULL;
    if(this->lib_handle){
        dlclose(this->lib_handle);
    }
}
CurlConnection::~CurlConnection(void) {
   	printf("CurlConnection dtor %s\n", this->schemeHostPort.c_str());
   	cleanup();
}

