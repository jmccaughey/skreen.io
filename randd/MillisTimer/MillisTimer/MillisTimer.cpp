//
//  PlatformTimer.cpp
//  PlatformTimer
//
//  Created by John McCaughey on 10/17/13.
//  Copyright (c) 2013 John McCaughey. All rights reserved.
//

#include "PlatformTimer.h"


PlatformTimer::PlatformTimer(){
    reset();
}

unsigned long PlatformTimer::getElapsedMillis(){
    Duration time = AbsoluteDeltaToDuration(UpTime(), lastTime);
    double timef = time < 0 ? time / -1000.0 : time;
    return timef;
}
void PlatformTimer::reset(){
    lastTime = UpTime();
}

