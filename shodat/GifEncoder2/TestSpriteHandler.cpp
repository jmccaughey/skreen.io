#include <stdio.h>
#include "TestSpriteHandler.h"
#include <vector>

//TestSpriteHandler::TestSpriteHandler(){}

TestSpriteHandler::~TestSpriteHandler(){
    	printf("in TestSpriteHandler dtor\n");
}

void TestSpriteHandler::handleSprite(unsigned char * buffer, int length, std::vector<MappedRegion*>* mappedRegions) {
    	printf("in callback got %d byte gif for %d regions\n", length, mappedRegions->size());    	
}
