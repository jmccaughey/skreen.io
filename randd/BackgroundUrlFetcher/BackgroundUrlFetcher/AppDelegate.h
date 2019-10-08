//
//  AppDelegate.h
//  BackgroundUrlFetcher
//
//  Created by John McCaughey on 12/4/13.
//  Copyright (c) 2013 John McCaughey. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface AppDelegate : NSObject <NSApplicationDelegate>{
    
}

@property (assign) IBOutlet NSWindow *window;
@property (weak) IBOutlet NSTextField *urlInput;
- (IBAction)goClicked:(id)sender;
@property (weak) IBOutlet NSTextField *textLabel;

@end
