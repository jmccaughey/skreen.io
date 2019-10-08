#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <sstream>
#include <tchar.h>
#include "json_parser.h"
#include "Win32ScraperPoster.h"
#include "HostLatencyChecker.h"
#include "StringSplitter.h"

// /usr/bin/i586-mingw32msvc-g++ main.cpp ../../windows-scrapers-common2/ScraperPosterMetaData.cpp -I../../windows-scrapers-common2

int WINAPI WinMain(HINSTANCE hInstance,
		HINSTANCE hPrevInstance,
		LPSTR lpCmdLine,
		int nCmdShow) {

	ScraperPosterMetaData* scraperPosterMetaData = NULL;
	//MessageBox(NULL, __DATE__, "skreen.io", NULL);	
	if(__argc == 1){
		// use defaults
		static DWORD RESPONSE_BUFFER_LENGTH = 4096;
		unsigned char response[RESPONSE_BUFFER_LENGTH];
		Poster poster("skreen.io", 80, "/listServers", false);
		DWORD responseLength = 0;
		bool result = poster.sendPost(NULL, (DWORD)0, response, RESPONSE_BUFFER_LENGTH, &responseLength);
		if(!result){
			MessageBox(NULL, "unable to get server list", "skreen.io", NULL);
			return EXIT_FAILURE;
		}
		//MessageBox(NULL, (const CHAR*)response, "skreen.io", NULL);
		// TODO: test hosts for latency. For now, grab first one
	 	std::string resp((const CHAR*)response, responseLength);
		std::vector<std::string> hosts;
		StringSplitter stringSplitter;
		stringSplitter.split(hosts, resp);
		std::string hostItem;
		//std::stringstream ss(resp);
		HostLatencyChecker hostLatencyChecker;
		for(int i = 0; i < hosts.size(); i++){
			hostLatencyChecker.addHost(hosts[i]);
		}
		std::string fastestHost = hostLatencyChecker.getFastestHost("/index.html", 80, false);
		//MessageBox(NULL, fastestHost.c_str(), "skreen.io", NULL);
		//char host[2048];
		//char* newlinePtr  = strstr((const CHAR*)response, "\n");
		//*newlinePtr = NULL;
		//strcpy(host, 
			//(const CHAR*)response);
		//	fastestHost.c_str());
		Poster jsonPoster(fastestHost, 80, "/createAndStartBlast", false);
		result = jsonPoster.sendPost(NULL, (DWORD)0, response, RESPONSE_BUFFER_LENGTH, &responseLength);
	        if(!result){
			MessageBox(NULL, "unable to start blast", "skreen.io", NULL);
                        return EXIT_FAILURE;
                }
		JsonParser jsonParser((const CHAR*)response);
		std::string blastId = jsonParser.getValue("blastId");
		std::string blasterId = jsonParser.getValue("blasterId");
		std::string key = jsonParser.getValue("key");
		scraperPosterMetaData = new ScraperPosterMetaData(
                                "http://" + fastestHost, // host
                                blastId,// blast id
                                blasterId,// blaster id
                                key,// key
				blastId
                );
	}else if(__argc != 5) {
		MessageBox(NULL, "4 required arguments: schemehostport blastid blasterid key",
				"shodat",
				NULL);
		return EXIT_FAILURE;
	}else{
		const char* const str = __argv[2];
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
				std::string(__argv[1]), // host
				//__argc == 7 ? __argv[6] : "",// context
				//false,// ssl
				std::string(__argv[2]),// blast id
				std::string(__argv[3]),// blaster id
				std::string(__argv[4]),// key
				std::string(__argv[2])
		);
	}
//	MessageBox(NULL, "returned from new metadata obj",
//			"shodat",
//			NULL);
	
	Win32ScraperPoster* win32ScraperPoster = new Win32ScraperPoster(scraperPosterMetaData, NULL);
	HANDLE uiThreadHandle = win32ScraperPoster->resumeScraping(NULL, NULL);
//	MessageBox(NULL, "returned from resumeScraping",
//				"shodat",
//				NULL);
	DWORD res = WaitForSingleObject(uiThreadHandle, INFINITE);
	char buff[2048];
	//MessageBox(NULL, "returned from WaitForSingleObject", "shodat", NULL);
	res =  GetModuleFileName(NULL, buff, 2048);
	//MessageBox(NULL, "returned from GetModuleFileName", "shodat", NULL);
	//MessageBox(NULL, buff, "shodat", NULL);
	res = DeleteFile(buff);
	//MessageBox(NULL, "returned from DeleteFile", "shodat", NULL);
	if(res == 0){
		// TODO: deleteFile does not seem to work even if the
		// file is opened with share_delete permission

		//MessageBox(NULL, "DeleteFile failed", "shodat", NULL);
		//MessageBox(NULL, buff, "shodat", NULL);
		res = GetLastError();
		sprintf(buff, "%d", res);
		//MessageBox(NULL, buff, "shodat", NULL);
	}else{
		//MessageBox(NULL, "DeleteFile success", "shodat", NULL);
	}
	return 0;

}
