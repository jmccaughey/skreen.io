#include "PlatformPoster.h"
//#include "PostJob.h"
#include <algorithm>

void p_trace(const char *msg) {
}

size_t PlatformPoster::curlDataCallback(void * buffer, size_t size, size_t number, void* context){
	return static_cast<PlatformPoster*>(context)->handleData(buffer, size, number);
}
size_t PlatformPoster::handleData(void * buffer, size_t size, size_t number){
	size_t pending = size * number;
	size_t remaining = RESPONSE_LENGTH - responseCursor;
	size_t copyAmount = std::min(remaining, pending);
	printf("got %lu bytes\n", size * number);
	memcpy(&(response[responseCursor]), buffer, copyAmount);
	responseCursor += copyAmount;
	return copyAmount;
}
PlatformPoster::PlatformPoster() {
	responseCursor = 0;
	this->curl = NULL;
	this->chunk = NULL;
	p_trace(__DATE__);
	p_trace(__TIME__);
}
int PlatformPoster::sendPost(char* url, char * data, int length, char * response,
		int responseBufferLength, int* responseLength) {

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
	if (this->curl == NULL){
      curl = curl_easy_init();
      if(!curl){
        fprintf(stderr, "curl_easy_init() failed\n");
        return 0;
      }	
    }
    printf("url: %s\n", url);
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    chunk = curl_slist_append(chunk, "Expect:");
    res = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
    	/* use curl_slist_free_all() after the *perform() call to free this list again */
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, PlatformPoster::curlDataCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);
    curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE,
                     (curl_off_t)length);
    //curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, length);
    //curl_easy_setopt(curl, CURLOPT_TCP_NODELAY, 1L);
    printf("about to curl_easy_perform\n");
    res = curl_easy_perform(curl);
	printf("returned from curl_easy_perform\n");
    if(res != CURLE_OK){
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
              curl_easy_strerror(res));
    }
    *responseLength = responseCursor;
    responseCursor = 0;
    return 0;
}
void PlatformPoster::interrupt() {
	cleanup();
}
void PlatformPoster::cleanup(void) {
	printf("PlatformPoster::cleanup\n");
	if (this->curl) {
		curl_easy_cleanup(this->curl);
	}
	this->curl = NULL;
}
PlatformPoster::~PlatformPoster(void) {
   	printf("PlatformPoster dtor\n");
   	curl_slist_free_all(chunk);
	cleanup();
}
