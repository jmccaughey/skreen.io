//
//  LatencyChecker.hpp
//  TimeUrl
//
//  Created by John McCaughey on 10/2/15.
//  Copyright © 2015 John McCaughey. All rights reserved.
//

#ifndef LatencyChecker_hpp
#define LatencyChecker_hpp

#include "HostLatencyInfo.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <vector>

class LatencyChecker{
public:
    CurlConnection* findLowestLatencyConnection(std::vector<CurlConnection*> * candidateServers);
    void stop();
    //LatencyChecker(std::vector<CurlConnection*>);
    ~LatencyChecker();
private:
    //bool shouldWaitMore(HostLatencyInfo** hostLatencyinfos, int hostCount);
    std::vector<HostLatencyInfo*> * _hostLatencyInfos;
    //int _hostCount;
};

#endif /* LatencyChecker_hpp */
