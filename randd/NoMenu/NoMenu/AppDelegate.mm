//
//  AppDelegate.m
//  NoMenu
//
//  Created by John McCaughey on 11/1/13.
//  Copyright (c) 2013 John McCaughey. All rights reserved.
//

#import "AppDelegate.h"
#include "BogoInfo.h"

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    // Insert code here to initialize your application
    BogoInfo bogoInfo(2, 4);
    bogoInfo.getMonth();
    //[self.window makeKeyAndOrderFront:nil];
    //[self.window setLevel:NSStatusWindowLevel];
}

- (void)windowDidLoad:(NSNotification *)notification
{
    
   // [[self window] setLevel:NSStatusWindowLevel];
}

- (void)windowDidResignMain:(NSNotification *)notification
{
    // It's always nicer if the user has a choice
    //if ([[NSUserDefaults standardUserDefaults] boolForKey:@"DefaultAlwaysOnTop"] == YES) {
     //   [[self window] setLevel:NSFloatingWindowLevel];
    //} else {
    //    [[self window] setLevel:NSNormalWindowLevel];
    //}
}

@end
