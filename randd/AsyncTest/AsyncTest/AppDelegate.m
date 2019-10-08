//
//  AppDelegate.m
//  AsyncTest
//
//  Created by John McCaughey on 11/26/13.
//  Copyright (c) 2013 John McCaughey. All rights reserved.
//

#import "AppDelegate.h"

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    int i=53;
    //void (^someCode)() = ^{
        NSLog(@"The value of i is %i", i);
    //};
    
    // PlaceKitten.com URLs work like this:
    // http://placekitten.com/<width>/<height>
    NSInteger width = (int)self.imageView.bounds.size.width; NSInteger height = (int)self.imageView.bounds.size.height;
    NSString* urlString = [NSString
                           stringWithFormat:@"http://placekitten.com/%i/%i", width, height];
    NSURL* url = [NSURL URLWithString:urlString];
    NSURLRequest* request = [NSURLRequest requestWithURL:url];
    [NSURLConnection sendAsynchronousRequest:request
                                       queue:[NSOperationQueue mainQueue]
                           completionHandler:^(NSURLResponse * response,
                                               NSData * data,
                                               NSError * error) {
                               NSImage* image = [[NSImage alloc] initWithData:data];
                               self.imageView.image = image;
                           }];
}

@end
