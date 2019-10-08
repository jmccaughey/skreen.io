//
//  main.cpp
//  LibLoad
//
//  Created by John McCaughey on 10/21/13.
//  Copyright (c) 2013 John McCaughey. All rights reserved.
//

#include <dlfcn.h>
//#include "ApplicationServices/ApplicationServices.h"

#include <iostream>
#include <curl/curl.h>

int main(int argc, const char * argv[])
{
    CURL* curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, "https://www.webhuddle.com");
    curl_easy_perform(curl);
    // TODO: does libcurl on Mac OSX support HTTPS? Is there a certificate store?
    void* pLib = dlopen(
                        "/usr/lib/libcurl.dylib"
                        //"curl"
                        , 0);
    printf("%u\n", pLib);
    // insert code here...
    void* pFunc = dlsym(pLib, "curl_easy_init");
//    CURL* pCurl = ((curl_easy_init*)pFunc)();
    printf("%u\n", pFunc);
    return 0;
}

