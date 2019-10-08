//
//  main.cpp
//  TimeTest
//
//  Created by John McCaughey on 10/15/13.
//  Copyright (c) 2013 John McCaughey. All rights reserved.
//

#include <iostream>

#define NANO_SECONDS_PER_MILLI 1000000

int main(int argc, const char * argv[])
{
    
    unsigned long millis = 999;///getSleepTime();
	if(millis > 0){
        getchar();
		printf("sleeping %lu...\n", millis);
		timespec sleepValue = {0};
        // TODO: nanosleep does not seem to work when nanoseconds are passed
        // only seconds
        // update: seems to work with values below this:
		sleepValue.tv_nsec = NANO_SECONDS_PER_MILLI *millis;//999990000;//100000000000000;//millis * NANO_SECONDS_PER_MILLI;
		sleepValue.tv_sec = 0;
		//struct timespec[] ts = {{0, millis * 1000000}};
		//nanosleep((struct timespec[]){{0, millis * 1000000}}, NULL);
		int nanosleepResult = nanosleep(&sleepValue, NULL);
		//nano
        printf("...unsleeping. Result: %d\n", nanosleepResult);
	}else{
		printf("not sleeping\n");
	}


    // insert code here...
    std::cout << "Hello, World!\n";
    return 0;
}

