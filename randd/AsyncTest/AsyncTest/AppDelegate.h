//
//  AppDelegate.h
//  AsyncTest
//
//  Created by John McCaughey on 11/26/13.
//  Copyright (c) 2013 John McCaughey. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface AppDelegate : NSObject <NSApplicationDelegate>
@property (weak) IBOutlet IKImageView *imageView;

@property (assign) IBOutlet NSWindow *window;

@end
