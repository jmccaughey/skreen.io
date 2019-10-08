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

int main(int argc, const char** argv)
{
 SInt32 nRes = 0;
 CFUserNotificationRef pDlg = NULL;
  
 const void* keys[] = {kCFUserNotificationAlertHeaderKey,
                      //kCFUserNotificationProgressIndicatorValueKey,
                      kCFUserNotificationDefaultButtonTitleKey};
const void* values[] = {CFSTR("Showing"),
                        //kCFBooleanTrue,
                        CFSTR("Cancel")};
CFDictionaryRef parameters = CFDictionaryCreate(0, keys, values,
        sizeof(keys)/sizeof(*keys), &kCFTypeDictionaryKeyCallBacks,
        &kCFTypeDictionaryValueCallBacks);
SInt32 err = 0;
pDlg = CFUserNotificationCreate(kCFAllocatorDefault, 0,
        kCFUserNotificationPlainAlertLevel | kCFUserNotificationNoDefaultButtonFlag,
        &err, parameters);
 
// flag for scraper to watch for user stopped
// flag for us to watch for scraper error 
 	
 // start scraping thread
do{ 
	nRes = CFUserNotificationReceiveResponse (
		pDlg,
		2, // seconds
		0
	);
}while(nRes != 0);

// kill scraper
  return 0;
}