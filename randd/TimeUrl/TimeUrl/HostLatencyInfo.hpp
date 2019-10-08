//
//  HostLatencyInfo.hpp
//  TimeUrl
//
//  Created by John McCaughey on 9/30/15.
//  Copyright © 2015 John McCaughey. All rights reserved.
//

#ifndef HostLatencyInfo_hpp
#define HostLatencyInfo_hpp

#include <stdio.h>
#include "CurlConnection.h"
#include <pthread.h>

class HostLatencyInfo
{
public:
    HostLatencyInfo(CurlConnection* connection);
    //~HostLatencyInfo();
    const double getConnectSeconds() const;
    CurlConnection* gethost();
    void setConnectSeconds(double seconds);
    const double getTransferSeconds() const;
    void setTransferSeconds(double seconds);
    void pull_one_url();
    int handleProgress();
    bool wasSuccessful(){
        return _success;
    }
    static void* threadFunc(void* instance){
        static_cast<HostLatencyInfo*>(instance)->pull_one_url();
        return NULL;
    }
    int start(){
        return ::pthread_create(&_tid,
                               NULL,
                                   HostLatencyInfo::threadFunc,
                               (void*)this);
    }
    void stop(){
        _stopRequested = true;
    }
    int join(){
        return pthread_join(this->_tid, NULL);
    }
    bool isDone(){
        return _done;
    }
    static int progress_cb(void *p,
                          double dltotal, double dlnow,
                       double ultotal, double ulnow){
        return static_cast<HostLatencyInfo*>(p)->handleProgress();
    }
    static size_t noop_cb(void *ptr, size_t size, size_t nmemb, void *data){
        return size * nmemb;
    }
    
private:
    CurlConnection* curlConnection;
    double _connectSeconds;
    double _transferSeconds;
    bool _success;
    pthread_t _tid;
    bool _stopRequested;
    bool _done;
};

#endif /* HostLatencyInfo_hpp */
