//
//  LatencyChecker.cpp
//  TimeUrl
//
//  Created by John McCaughey on 10/2/15.
//  Copyright © 2015 John McCaughey. All rights reserved.
//

#include "LatencyChecker.hpp"

CurlConnection* LatencyChecker::findLowestLatencyConnection(std::vector<CurlConnection*> * candidateServers){
    
    std::vector<CurlConnection*>::iterator connIter = candidateServers->begin();
    this->_hostLatencyInfos = new std::vector<HostLatencyInfo*>;
    for( ; connIter !=  candidateServers->end(); connIter++){
        HostLatencyInfo* hli = new HostLatencyInfo(*connIter);
        this->_hostLatencyInfos->push_back(hli);
        if(hli->start()){
            printf("error starting thread\n");
            return NULL;
        }
    }
    
    HostLatencyInfo * fastestHost = NULL;
    double fastestTime = 0.0;
    std::vector<HostLatencyInfo*>::iterator hliIter = this->_hostLatencyInfos->begin();
    for( ; hliIter != this->_hostLatencyInfos->end();  hliIter++){
        (*hliIter)->join();
        printf("%f for %s\n", (*hliIter)->getConnectSeconds(), (*hliIter)->gethost()->getServer().c_str());
        if((*hliIter)->wasSuccessful()){
            if(fastestTime == 0.0 || (*hliIter)->getConnectSeconds() < fastestTime){
                fastestTime = (*hliIter)->getConnectSeconds();
                fastestHost = (*hliIter);
            }
        }
    }
    return fastestHost->gethost();
}

LatencyChecker::~LatencyChecker(){
    if(_hostLatencyInfos){
        std::vector<HostLatencyInfo*>::iterator hliIter = this->_hostLatencyInfos->begin();
        for( ; hliIter != this->_hostLatencyInfos->end();  hliIter++){
            printf("deleting HostLatencyInfo\n");
            delete (*hliIter);
        }
        delete this->_hostLatencyInfos;
    }
}
