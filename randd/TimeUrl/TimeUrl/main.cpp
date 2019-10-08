//
//  main.cpp
//  TimeUrl
//
//  Created by John McCaughey on 9/28/15.
//  Copyright © 2015 John McCaughey. All rights reserved.
//

#include "LatencyChecker.hpp"
#include <iostream>

int main(int argc, const char * argv[]) {
    int hostCount = argc - 1;
    char** hosts = (char**)malloc((hostCount)*sizeof(char*));
    for(int i = 0; i < hostCount; i++){
        hosts[i] = strdup(argv[i + 1]);
    }
    LatencyChecker latencyChecker(hosts, hostCount);
    int fastestHostIndex = latencyChecker.findLowestLatencyConnection();
    if(fastestHostIndex == -1) {
        printf("unable to connect to any host\n");
    }else{
        printf("%s is the fastest host\n", argv[fastestHostIndex + 1]);
    }
}
