/* 
 test2.c 
 Mac OS X Message Box using only CoreFoundation framework.
 This code doesn't use Objective-C or Cocoa, just plain C.
 Build using commands:

 gcc -framework CoreFoundation -o Test2 test2.c
 mkdir -pv Test2.app/Contents/MacOS && mv -v Test2 Test2.app/Contents/MacOS

 Run the resulting by double-clicking Test2 from the Finder or by issuing
 command:
 open Test2.app
 from the Terminal.

 */

#include <CoreFoundation/CoreFoundation.h>
#include "../../shodat/windows-scrapers-common2/MacScraperPoster.h"
//#include "../../shodat/windows-scrapers-common2/platforms/mac/poster/PlatformPoster.h"
#include "../../shodat/windows-scrapers-common2/json_parser.h"
#include <pthread.h>

//static std:string gCode = "";

static void *scrapeThreadFunction(void *context){
  //trace(LOG_NAME, "entering ScraperThreadFunction.\n");
  //ScraperPosterMetaData* scraperPosterMetaData = (ScraperPosterMetaData*) context;
  MacScraperPoster* scraperPoster = (MacScraperPoster*)(context);
  //trace(LOG_NAME, "ScraperThreadFunction (cast context complete)\n");
  while(scraperPoster->shouldContinue()){
  	//trace(LOG_NAME, "ScraperThreadFunction (in loop, pluginInstance->shouldContinue returned true)\n");
    //trace(LOG_NAME, "ScraperThreadFunction (in loop, about to scrape)\n");
    scraperPoster->doScreenScrape();
    printf("ScraperThreadFunction (in loop, returned from scrape)\n");
  }
  printf("exiting ScraperThreadFunction.\n");
  pthread_exit(NULL);
  return 0;
}

static ScraperPosterMetaData * getAnonymousCredentials(){
    printf("getting anonymous credentials\n");
//	PlatformPoster* platformPoster = new PlatformPoster();
    CurlPoster* platformPoster = new CurlPoster();
	char* response = (char*)malloc(2049);
	int responseLength = 0;
	int result = platformPoster->sendPost(
                                          "http://skreen.io/createAndStartBlast"
//                                          "http://192.168.0.107:8888/createAndStartBlast"
//                                          "http://localhost:8080/"
                                          ,
                                          (char*)"{}", 2, response, 2048, &responseLength);
    response[responseLength] = '\0';
	printf("got %d from poster\n", result);
    if(responseLength > 0){
		JsonParser* jsonParser = new JsonParser(std::string(response));
        std::string blastId = jsonParser->getValue("blastId");
        std::string id = jsonParser->getValue("id");
        std::string key = jsonParser->getValue("key");
        std::string name = jsonParser->getValue("name");
        if(!blastId.length() || !id.length() || !key.length() || !name.length()){
            printf("expected parameters not reeived from server (got:\n %s)\n", response);
            return NULL;
        }
		ScraperPosterMetaData* scraperPosterMetaData = new ScraperPosterMetaData(
			"skreen.io"
            //"192.168.0.107"
            ,
            //8888
            80
            ,
                                                                            
            "",
                                                                                 false,
            //"3434",
                                                                                 blastId.c_str(),
        //"4d9934",
                                                                                 id.c_str(),
			//"342353457345345jsdfsdf",
                                                                                 key.c_str(),
			//"name"
                                                                                 name.c_str()
		);
        free(response);
		return scraperPosterMetaData;
    }
    free(response);
    delete platformPoster;
	return NULL;
}

int main(int argc, const char** argv){
 SInt32 nRes = 0;
 CFUserNotificationRef pDlg = NULL;
  
 const void* keys[] = {kCFUserNotificationAlertHeaderKey,
                      //kCFUserNotificationProgressIndicatorValueKey,
                      kCFUserNotificationDefaultButtonTitleKey};
 
// flag for scraper to watch for user stopped
// flag for us to watch for scraper error 
    ScraperPosterMetaData* scraperPosterMetaData = NULL;
   
    char buffer[1024];
    //read(0, &buffer, 1024);
    
    
	if(argc == 1){
		// do post to get anonymous credentials
		scraperPosterMetaData = getAnonymousCredentials();
        if(scraperPosterMetaData == NULL){
            printf("error connecting to server\n");
            return 1;
        }
	}else if(argc < 6 || argc > 7) {
		fprintf(stderr, "5 required arguments: host port blastid blasterid key [context]");
		return EXIT_FAILURE;
	}else{
		const char* const str = argv[2];
		char *end;
		int port;
		
		errno = 0;
		
		const long sl = strtol(str, &end, 10);
		
		if (end == str) {
			fprintf(stderr, "%s: not a decimal number\n", str);
		}	else if ('\0' != *end) {
			fprintf(stderr, "%s: extra characters at end of input: %s\n", str, end);
		}	else if ((LONG_MIN == sl || LONG_MAX == sl) && ERANGE == errno) {
			fprintf(stderr, "%s out of range of type long\n", str);
		}	else if (sl > INT_MAX) {
			fprintf(stderr, "%ld greater than INT_MAX\n", sl);
		}	else if (sl < INT_MIN) {
			fprintf(stderr, "%ld less than INT_MIN\n", sl);
		}	else {
			port = (int)sl;
		}
		
		scraperPosterMetaData = new ScraperPosterMetaData(
																				 argv[1], // host
																				 port,// port
																				 argc == 7 ? argv[6] : "",// context
																				 false,// ssl
																				 argv[3],// blast id
																				 argv[4],// blaster id
																				 argv[5]// key
																				 );
		
	}
	const void* values[] = {
						CFStringCreateWithCString(NULL, scraperPosterMetaData->getName(), kCFStringEncodingUTF8),
						//CFSTR("Showing"),
                        //kCFBooleanTrue,
						  CFSTR("Cancel")};


		CFDictionaryRef parameters = CFDictionaryCreate(0, keys, values,
        sizeof(keys)/sizeof(*keys), &kCFTypeDictionaryKeyCallBacks,
        &kCFTypeDictionaryValueCallBacks);
SInt32 err = 0;
pDlg = CFUserNotificationCreate(kCFAllocatorDefault, 0,
        kCFUserNotificationPlainAlertLevel | kCFUserNotificationNoDefaultButtonFlag,
        &err, parameters);
		
//	MessageBox(NULL, "returned from new metadata obj",
//			"shodat",
//			NULL);
	if(argc < 2) {
		printf("no args found\n");
	} else {
//		MessageBox(NULL, "args found",
//				"shodat",
//				NULL);
	}
	MacScraperPoster* scraperPoster = new MacScraperPoster(scraperPosterMetaData);
	//HANDLE uiThreadHandle = win32ScraperPoster->resumeScraping(NULL, NULL);
//	MessageBox(NULL, "returned from resumeScraping",
//				"shodat",
//				NULL);
	//DWORD res = WaitForSingleObject(uiThreadHandle, INFINITE);
	//char buff[2048];
	//return 0;

 // start scraping thread
 pthread_t scraperThread;
 int rc = pthread_create(&scraperThread, NULL, scrapeThreadFunction, scraperPoster);
 if(rc){
        		printf("ERROR; return code from pthread_create() is %d\n", rc);
			}else{
		printf("initParms (thread started)\n");
 }
 do{ 
	nRes = CFUserNotificationReceiveResponse (
		pDlg,
		1, // seconds
		0
	);
	//scraperPoster->doScreenScrape();
}while(nRes != 0);
scraperPoster->stopScraping();
pthread_join(scraperThread, NULL);
printf("returned from joining ScraperThreadFunction.\n");

// kill scraper
  return 0;
}