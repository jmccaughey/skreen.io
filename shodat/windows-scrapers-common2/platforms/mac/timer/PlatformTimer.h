#include <iostream>
#include "ApplicationServices/ApplicationServices.h"
#include <mach/mach.h>
#include <mach/mach_time.h>

class PlatformTimer{

public:
    PlatformTimer();
    unsigned long getElapsedMillis();
    void reset();
    
private:
    //AbsoluteTime lastTime;
    uint64_t        lastTime;
    //static mach_timebase_info_data_t    sTimebaseInfo;
};

