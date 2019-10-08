#include "PlatformTimer.h"

PlatformTimer::PlatformTimer(){
    //(void) mach_timebase_info(&sTimebaseInfo);
    reset();
}

unsigned long PlatformTimer::getElapsedMillis(){
    uint64_t now = mach_absolute_time();
    //uint64_t        elapsedNano;
    uint64_t        elapsed = now - lastTime;
    Nanoseconds     elapsedNano;
    elapsedNano = AbsoluteToNanoseconds( *(AbsoluteTime *) &elapsed );
    //elapsedNano = elapsed * sTimebaseInfo.numer / sTimebaseInfo.denom;
    return (* (uint64_t *) &elapsedNano) / (1000000);
    
    //Nanoseconds     elapsedNano = AbsoluteToNanoseconds(<#AbsoluteTime absoluteTime#>)
    //Duration time = AbsoluteDeltaToDuration(UpTime(), lastTime);
    //double timef = time < 0 ? time / -1000.0 : time;
    //return timef;
}
void PlatformTimer::reset(){
    lastTime = mach_absolute_time();
}

