#import "Controller.h"

@implementation Controller
@synthesize showingText;
- (void)connectDone:(NSString *)meetingCode{
    [showingText setStringValue:meetingCode];
    CFOptionFlags cfRes;
    NSString *uriText = [NSString stringWithFormat:@"http://skreen.io/%@", meetingCode];
    NSString *message = [NSString stringWithFormat:@"Your screen will be shown at %@. This link will be copied to your clipboard.", uriText];
    CFUserNotificationDisplayAlert(0, kCFUserNotificationNoteAlertLevel,
                                   NULL, NULL, NULL,
                                   CFSTR("Show your screen?"),
                                   (__bridge CFStringRef)message,
                                   CFSTR("OK"),
                                   CFSTR("Cancel"),
                                   NULL,
                                   &cfRes);
    switch (cfRes)
    {
        case kCFUserNotificationDefaultResponse:
            bConsentGiven = true;
            break;
        default:
            bConsentGiven = false;
            break;
    }
    if(bConsentGiven){
        NSPasteboard *pasteboard = [NSPasteboard generalPasteboard];
        [pasteboard declareTypes:[NSArray arrayWithObject:NSStringPboardType] owner:nil];
        [pasteboard setString:uriText forType:NSStringPboardType];
    }
}

- (void)consentNotGranted:(NSString *)message{
    [showingText setStringValue:message];
    
    CFUserNotificationRef pDlg = NULL;
    SInt32 nRes = 0;
    const void* keys[] = {
        kCFUserNotificationAlertHeaderKey,
        kCFUserNotificationAlertMessageKey,
        kCFUserNotificationDefaultButtonTitleKey};
    //NSString* stringFromUTFString = [[NSString alloc] initWithUTF8String:scraperPoster->THE_HOST];
    NSString* alertMessage = @"Screen showing cancelled";
    const void* values[] = { alertMessage, @"Your screen will not be shown.", CFSTR("OK") };
    CFDictionaryRef parameters = CFDictionaryCreate(0, keys, values,
                                                    sizeof(keys)/sizeof(*keys), &kCFTypeDictionaryKeyCallBacks,
                                                    &kCFTypeDictionaryValueCallBacks);
    SInt32 err = 0;
    pDlg = CFUserNotificationCreate(kCFAllocatorDefault, 0,
                                    kCFUserNotificationCautionAlertLevel
                                    | kCFUserNotificationNoDefaultButtonFlag,
                                    &err, parameters);
    do{
        nRes = CFUserNotificationReceiveResponse (pDlg, 2, // seconds
                                                  0);
    }while(nRes != 0);
}


- (void)networkError:(NSString *)message{
    [showingText setStringValue:message];
    
    CFUserNotificationRef pDlg = NULL;
    SInt32 nRes = 0;
    const void* keys[] = {
        kCFUserNotificationAlertHeaderKey,
        kCFUserNotificationAlertMessageKey,
        kCFUserNotificationDefaultButtonTitleKey};
    NSString* stringFromUTFString = [[NSString alloc] initWithUTF8String:scraperPoster->THE_HOST];
    NSString* alertMessage = [NSString stringWithFormat:@"Error communicating with %@", stringFromUTFString];
    const void* values[] = { alertMessage, @"Please check your network.", CFSTR("OK") };
    CFDictionaryRef parameters = CFDictionaryCreate(0, keys, values,
                                                    sizeof(keys)/sizeof(*keys), &kCFTypeDictionaryKeyCallBacks,
                                                    &kCFTypeDictionaryValueCallBacks);
    SInt32 err = 0;
    pDlg = CFUserNotificationCreate(kCFAllocatorDefault, 0,
                                    kCFUserNotificationCautionAlertLevel
                                    | kCFUserNotificationNoDefaultButtonFlag,
                                    &err, parameters);
    do{
        nRes = CFUserNotificationReceiveResponse (pDlg, 2, // seconds
                                                  0);
    }while(nRes != 0);
}

- (void)connectFailed:(NSString *)message{
    [showingText setStringValue:message];
    // TODO: cleanup dupicated code for popup dialogs
    CFUserNotificationRef pDlg = NULL;
    SInt32 nRes = 0;
    const void* keys[] = {
        kCFUserNotificationAlertHeaderKey,
        kCFUserNotificationAlertMessageKey,
        kCFUserNotificationDefaultButtonTitleKey};
    NSString* stringFromUTFString = [[NSString alloc] initWithUTF8String:scraperPoster->THE_HOST];
    NSString* alertMessage = [NSString stringWithFormat:@"Could not connect to %@", stringFromUTFString];
    const void* values[] = { alertMessage, @"Please check your network.", CFSTR("OK") };
    CFDictionaryRef parameters = CFDictionaryCreate(0, keys, values,
                                                    sizeof(keys)/sizeof(*keys), &kCFTypeDictionaryKeyCallBacks,
                                                    &kCFTypeDictionaryValueCallBacks);
    SInt32 err = 0;
    pDlg = CFUserNotificationCreate(kCFAllocatorDefault, 0,
                                    kCFUserNotificationCautionAlertLevel
                                    | kCFUserNotificationNoDefaultButtonFlag,
                                    &err, parameters);
    do{
        nRes = CFUserNotificationReceiveResponse (pDlg, 2, // seconds
                                                  0);
    }while(nRes != 0);
}

- (void)scraperExit:(NSString *)message{
    [showingText setStringValue:message];
    [NSApp terminate: nil];
}

- (void)scrapingThread:(id)inObject{
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    char idBuffer[512];
    idBuffer[0] = 0;
    if(scraperPoster != NULL && scraperPoster->connect()){
        scraperPoster->getMeetingCode(idBuffer);
        NSString* string = [NSString stringWithFormat:@"%s", idBuffer];
        [self performSelectorOnMainThread:@selector(connectDone:)
                           withObject:string
                        waitUntilDone:YES];
        while(bConsentGiven && scraperPoster->shouldContinue()){
            scraperPoster->doScreenScrape();
            printf("ScraperThreadFunction (in loop, returned from scrape)\n");
        }
        printf("returned from scrape loop\n");
        if(bStopping){
            printf("shoudlContinue returned false and stopping is true (do showingStopped call here)\n");
            scraperPoster->sendScrapingStoppedMessage();
            scraperPoster = NULL;
        }else if(!bConsentGiven){
            NSLog(@"consent not given");
            [self performSelectorOnMainThread:@selector(consentNotGranted:)
                                   withObject:@"Cancelled"
                                waitUntilDone:YES];
        }else{
            // we are not here due to user clicking stop, so show error message
            [self performSelectorOnMainThread:@selector(networkError:)
                                   withObject:@"error"
                                waitUntilDone:YES];
        }
    }else{
        [self performSelectorOnMainThread:@selector(connectFailed:)
                               withObject:@"error"
                            waitUntilDone:YES];
    }
    printf("about to delete scraperPoster\n");
    delete scraperPoster;
    scraperPoster = NULL;
    printf("exiting thread function\n");
    [pool release];
    [self performSelectorOnMainThread:@selector(scraperExit:)
                           withObject:@"error"
                        waitUntilDone:NO];
}

- (void)awakeFromNib{
    [super awakeFromNib];
    [[self window] setDelegate:self];
    printf("view loaded (awake from nib)\n");
    bStopping = false;
    bConsentGiven = false;
    [showingText setStringValue:@"connecting..."];
    scraperPoster = new MacScraperPoster();
    [NSThread detachNewThreadSelector:@selector(scrapingThread:)
                             toTarget:self      // we are the target
                           withObject:nil];
}

- (void)windowWillClose:(NSNotification *)notification {
    [showingText setStringValue:@"stopping..."];
    printf("windowWillClose\n");
    [self stopScraper];
    for(int i = 0; i < 30; i++){
        if(scraperPoster != NULL){
            printf("waiting for scrape thread\n");
            [NSThread sleepForTimeInterval:0.1f];
        }else{
            break;
        }
    }
}

//- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)sender{
//    return YES;
//}

- (void)stopScraper{
    // TODO: join scraping thread
    printf("stopScraper\n");
    bStopping = true;
    [showingText setStringValue:@"stopping..."];
    if(scraperPoster){
        printf("about to call stopScraping\n");
        scraperPoster->stopScraping();
        printf("...returned from stopScraping\n");
    }else{
        printf("scraperPoster is null\n");
    }
    [showingText setStringValue:@"stopped"];
    scraperThread = NULL;
}

- (IBAction)stopClicked:(id)sender {
    [showingText setStringValue:@"stopping..."];
    [self stopScraper];
}

@end
