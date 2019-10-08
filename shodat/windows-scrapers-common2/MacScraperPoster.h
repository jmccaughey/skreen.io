#include "boost/unordered/unordered_set.hpp"
#include <vector>
#include "boost/unordered/unordered_map.hpp"
#include <map>
#include <string>

#include "Region.h"
#include "CurlConnection.h"
#include "PlatformTimer.h"
#include "ScraperPosterMetaData.h"
#include "resource.h"
#include "HashString.h"
#include "SubtileInfo.h"
#include "../GifEncoder2/GifSpriter.h"
#include "PlatformScreenNoCopy.h"
#include "json_parser.h"
extern "C" {
#include "jpeg-turbo/include/jpeglib.h"
}

#define XSTR(x) #x
#define STR(x) XSTR(x)

#ifndef MAIN_SCHEME_HOST_PORT
#define MAIN_SCHEME_HOST_PORT http:\/\/skreen.io
//#define MAIN_SCHEME_HOST_PORT localhost
#endif

class MacScraperPoster{
    
public:
    static const int  SEND_BUFFER_LENGTH = 1024 * 1024 * 2;
    static const int POST_PATH_BUFFER_LENGTH = 4096;
    static const int RESPONSE_BUFFER_LENGTH = 4096;
    const char * THE_HOST = STR(MAIN_SCHEME_HOST_PORT);
    const static unsigned long SCRAPE_THROTTLE_MS = 333;
    const static unsigned long COMMAND_EXPIRATION_MS = 20000;
    const static int MAX_SERVERS = 100;
    bool mContinueScraping;
    bool mShutting;
    bool mSendKeyframe;
    int connect();
    void getMeetingCode(char* buffer);
    void doScreenScrape();
    void expireDisplay();
    void stopScraping();
    MacScraperPoster();
    ~MacScraperPoster();
    void sendScrapingStoppedMessage();
    bool shouldContinue(){
        printf("shouldContinue returning %d\n", !mShutting);
        return mContinueScraping && !mShutting;
    }
private:
    PlatformScreen* screen;
    CurlConnection* poster;
    int compressRegionToBuffer(deskblast::Region* region, int offset);
    void initDesktops();
    int writeHeader(int tileCount);
    bool writeHash(void *pixels, deskblast::Region* sourceRegion, int destOffset);
    void resetClock();
    unsigned char* mSendBuffer;
    char postPath[POST_PATH_BUFFER_LENGTH];
    int screenWidth;
    int screenHeight;
    boost::unordered_set<std::string> * sentHashes;
    boost::unordered_map<std::string, SubtileInfo> * subtileMap;
    void cleanUpHash(void);
    void init();
    void initPoster();
    void hashPixels(void *pixels, deskblast::Region* sourceRegion, char* dest);
    void hashBytes(unsigned char *pixels, int length, char* dest);
    unsigned long millisElapsed;
    void updateTicks();
    unsigned int getSleepTime();
    deskblast::Region ** regions;
    int regionCount;
    int mouseX;
    int mouseY;
    void initRegions();
    int checkForChanges();
    bool commandExpired();
    static const int HASH_LENGTH_BYTES = 32;
    bool displayChanged;
    bool displayExpired();
    ScraperPosterMetaData* pScraperPosterMetaData;
    int connectToSessionHost();
    int getSessionServer();
    static void buildUrlFromMetaData(ScraperPosterMetaData* pScraperPosterMetaData, char* buffer, const char* servlet);
    //std::string getViewUrl();
    GifSpriter* gifSpriter;
    int writeBackReference(deskblast::Region * region,
                           int cursor, int hashOffset, int spriteX, int spriteY);
    int compressAndWriteTile(
                             deskblast::Region * region, int cursor, int hashOffset, int width, int height, int spriteX, int spriteY);
    int writeCompressedTile(deskblast::Region* region, int cursor, std::string* hs, int spriteX, int spriteY,
                            unsigned char * gifStart, int gifLength);
    void doScreenShot();
    void sleepAndUpdateTicks();
    void cleanup();
    PlatformTimer* millisTimer;
    std::vector<CurlConnection*> * servers;
};

