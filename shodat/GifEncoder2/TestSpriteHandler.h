#include "ISpriteHandler.h"

class TestSpriteHandler : public ISpriteHandler {
public:
	//TestSpriteHandler();
    virtual ~TestSpriteHandler();
    virtual void handleSprite(unsigned char * buffer, int length, std::vector<MappedRegion*>* mappedRegions);
};