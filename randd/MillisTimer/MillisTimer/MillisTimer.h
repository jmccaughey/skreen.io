//
//  PlatformTimer.h
//  PlatformTimer
//
//  Created by John McCaughey on 10/17/13.
//  Copyright (c) 2013 John McCaughey. All rights reserved.
//

#ifndef __PlatformTimer__PlatformTimer__
#define __PlatformTimer__PlatformTimer__

#include <iostream>
//#include <sys/time.h>
#include "ApplicationServices/ApplicationServices.h"

class PlatformTimer{

public:
    PlatformTimer();
    unsigned long getElapsedMillis();
    void reset();
    
private:
    AbsoluteTime lastTime;
    
};

#endif /* defined(__PlatformTimer__PlatformTimer__) */
