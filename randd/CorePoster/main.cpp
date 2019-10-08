#include <CoreFoundation/CoreFoundation.h>
#include <CoreServices/CoreServices.h>
#include <Poster.h>

int main (int argc, const char * argv[]) {
    UInt8 body[4096]; //= "key=value&id=30293";
	UInt8 response[2048];
	int responseLength = 0;
	Poster* poster = new Poster();
	int result = 0;
	result = poster->postData(CFSTR("http://192.168.0.107:8888/scrapes"), 
		body, sizeof(body), response, sizeof(response), &responseLength);
	printf("request success %d\n", result);
	result = poster->postData(CFSTR("http://192.168.0.107:8888/scrapes"), 
		body, sizeof(body), response, sizeof(response), &responseLength);
	printf("request success %d\n", result);	
    return 0;
}


