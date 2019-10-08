//
//  BogoInfo.h
//  NoMenu
//
//  Created by John McCaughey on 11/1/13.
//  Copyright (c) 2013 John McCaughey. All rights reserved.
//

#ifndef __NoMenu__BogoInfo__
#define __NoMenu__BogoInfo__

//#include <iostream>




class BogoInfo{
    
public:
    
    BogoInfo(int month, int day){
        this->month = month;
        this->day = day;
    }
    
    int getMonth();
    int getDay();
    
private:
    int month;
    int day;
};

#endif /* defined(__NoMenu__BogoInfo__) */
