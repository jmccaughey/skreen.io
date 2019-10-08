//
//  main.cpp
//  CurlTest
//
//  Created by John McCaughey on 12/7/13.
//  Copyright (c) 2013 John McCaughey. All rights reserved.
//

#include <iostream>
#include <curl/curl.h>

int progress_callback(void *clientp,
                              double dltotal,
                              double dlnow,
                              double ultotal,
                           double ulnow){
    printf("total: %f now: %f\n", dltotal, dlnow);
    return 0;
}

size_t writeDataCallback(void * buffer, size_t size, size_t number, void* context){
    printf("write: %lu\n", size * number);
	return size * number;
}

int main(int argc, const char * argv[])
{
    CURL *curl;
    CURLcode res;
    const char* DEFAULT_URL =
            "https://www.google.com"
            // "http://localhost:8080/adm/login.cfm"
    ;
    
    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, DEFAULT_URL);
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0);
        curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, progress_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeDataCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, 0);
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 60);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30);
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);

        /* example.com is redirected, so we tell libcurl to follow redirection */
        //curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        
        /* Perform the request, res will get the return code */
        res = curl_easy_perform(curl);
        /* Check for errors */
        if(res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                    curl_easy_strerror(res));
        
        /* always cleanup */
        curl_easy_cleanup(curl);
    }
    return 0;

}

