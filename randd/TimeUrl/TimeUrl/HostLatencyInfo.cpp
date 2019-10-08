//
//  HostLatencyInfo.cpp
//  TimeUrl
//
//  Created by John McCaughey on 9/30/15.
//  Copyright © 2015 John McCaughey. All rights reserved.
//

#include "HostLatencyInfo.hpp"
#include <string.h>

HostLatencyInfo::HostLatencyInfo(CurlConnection* connection){
    this->curlConnection = connection;
    this->_success = false;
    this->_done = false;
    this->_stopRequested = false;
}

int HostLatencyInfo::handleProgress(){
    return _stopRequested;
}

void HostLatencyInfo::pull_one_url(){
    char response[256];
    size_t responseLength = 0;
    printf("about to send timed get to %s\n", curlConnection->getServer().c_str());
    double seconds = 0.0;
    bool success = curlConnection->sendTimedHead("", response, 256, &responseLength, &seconds);
    if(success){
        printf("returned success from timed get %f\n", seconds);
        this->_success = true;
    }else{
        printf("returned fail from timed get %f\n", seconds);
    }
    setConnectSeconds(seconds);
    this->_done = true;
}

const double HostLatencyInfo::getConnectSeconds() const{
    return this->_connectSeconds;
}
CurlConnection* HostLatencyInfo::gethost(){
    return this->curlConnection;
}

void HostLatencyInfo::setConnectSeconds(double seconds){
    this->_connectSeconds = seconds;
}

const double HostLatencyInfo::getTransferSeconds() const{
    return this->_transferSeconds;
}
void HostLatencyInfo::setTransferSeconds(double seconds){
    this->_transferSeconds = seconds;
}

