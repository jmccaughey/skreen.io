//
//  AppDelegate.m
//  BackgroundUrlFetcher
//
//  Created by John McCaughey on 12/4/13.
//  Copyright (c) 2013 John McCaughey. All rights reserved.
//

#import "AppDelegate.h"

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    // Insert code here to initialize your application
}

- (IBAction)goClicked:(id)sender {
    
    // start threadd
    
    
    /*
    NSLog(@"goClicked");
    NSString* url = [_urlInput stringValue];
    [_textLabel setStringValue:@"fetching..."];
    NSOperationQueue* backgroundQueue = [[NSOperationQueue alloc] init];
    [backgroundQueue addOperationWithBlock:^{
        // Construct a URL for the website's icon
        NSURL* iconURL = [NSURL URLWithString:url];
        // Construct a URL request for this URL
        NSURLRequest* request = [NSURLRequest requestWithURL:iconURL];
        // Load the data
        NSData* loadedData = [NSURLConnection
                              sendSynchronousRequest:request returningResponse:nil error:nil];
        if (loadedData != nil) {
            // We got image data! Convert it to an image.
            //UIImage* loadedImage = [UIImage imageWithData:loadedData];
            // If the data wasn't able to be turned into an image, stop
            //if (loadedImage == nil) { return;
            //}
            // If it was, insert the image into the
            // table view on the main queue.
            [[NSOperationQueue mainQueue] addOperationWithBlock:^{
                [_textLabel setStringValue:@"got data"];
                //[websiteIcons replaceObjectAtIndex:websiteNumber
                //                        withObject:loadedImage];
                //[self.tableView reloadData];
            }];
        }else{
            [[NSOperationQueue mainQueue] addOperationWithBlock:^{
                [_textLabel setStringValue:@"error"];
                //[websiteIcons replaceObjectAtIndex:websiteNumber
                //                        withObject:loadedImage];
                //[self.tableView reloadData];
            }];
        }
    
    }];
    */
    
}
@end
