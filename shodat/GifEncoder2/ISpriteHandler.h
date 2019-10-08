#ifndef ISPRITEHANDLER_H
#define ISPRITEHANDLER_H

#include "MappedRegion.h"
#include <vector>

class ISpriteHandler {
public:
    virtual ~ISpriteHandler() {}; // destructor, use it to call destructor of the inherit classes
    virtual void handleSprite(unsigned char * buffer, int length, std::vector<MappedRegion*>* mappedRegions) = 0; // pure virtual method
};

#endif