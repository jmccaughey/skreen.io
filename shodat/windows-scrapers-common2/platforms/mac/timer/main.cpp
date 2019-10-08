//
//  main.cpp
//  Created by John McCaughey on 10/17/13.
//  Copyright (c) 2013 John McCaughey. All rights reserved.
//

#include <iostream>
#include "PlatformTimer.h"

int main(int argc, const char * argv[])
{

    PlatformTimer mt;
    sleep(1);
    printf("elapsed: %d\n", mt.getElapsedMillis());
    sleep(1);
    printf("elapsed: %d\n", mt.getElapsedMillis());
    mt.reset();
    printf("elapsed: %d\n", mt.getElapsedMillis());
    sleep(1);
    printf("elapsed: %d\n", mt.getElapsedMillis());
    return 0;
}

