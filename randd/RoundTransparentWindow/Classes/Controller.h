#import <Cocoa/Cocoa.h>

#import "MacScraperPoster.h"


@interface Controller : NSWindowController <NSWindowDelegate>{
    NSTextField *showingText;
    MacScraperPoster * scraperPoster;
    pthread_t scraperThread;
    boolean bStopping;
    boolean bConsentGiven;
}

//- (void)windowDidLoad;
- (IBAction)stopClicked:(id)sender;
@property (assign) IBOutlet NSTextField *showingText;
//- (IBAction)quitAction:(id)sender;

@end
