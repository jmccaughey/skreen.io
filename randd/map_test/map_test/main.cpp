//
//  main.cpp
//  map_test
//
//  Created by John McCaughey on 10/7/13.
//  Copyright (c) 2013 John McCaughey. All rights reserved.
//

#include <string>
#include <unordered_map>
#include <iostream>

int main(int argc, const char * argv[])
{

    std::unordered_map<std::string, std::string> theMap;
    theMap.insert(std::unordered_map<std::string, std::string>::value_type(std::string("testKey1"), std::string("testValue1")));
    // insert code here...
//    std::cout << "Hello, World!\n";
    
    //std::unordered_map<std::string, std::string>::iterator it;
    //it = theMap.find(std::string("testKey1"));
    size_t count = theMap.count(
                                //std::string(
                                            "testKey1"
                                  //          )
                                );
    if(count < 1){//it == theMap.end()){
        printf("not found\n");
    }else{
        printf("found\n");
    }
    
    return 0;
}

