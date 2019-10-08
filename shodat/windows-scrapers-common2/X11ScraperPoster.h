#include <set>
#include <vector>
#include <map>
#include <string>

#include "Region.h"
#include "PlatformPoster.h"
#include "ScraperPosterMetaData.h"
#include "resource.h"
#include "HashString.h"
#include "SubtileInfo.h"
#include "../GifEncoder2/GifSpriter.h"
#include "PlatformScreen.h"
extern "C" {
    #include "jpeglib.h"
}

class X11ScraperPoster{

public:
  static const int SEND_BUFFER_LENGTH = 1024 * 1024;
  static const int POST_URL_BUFFER_LENGTH = 4096;
  static const int RESPONSE_BUFFER_LENGTH = 4096;
  const static unsigned long SCRAPE_THROTTLE_MS = 333;
  const static unsigned long COMMAND_EXPIRATION_MS = 20000;
  bool mContinueScraping;
  bool mShutting;
  bool mSendKeyframe;
  void doScreenScrape();
  void expireDisplay();
  void stopScraping();
  void stopScrapingThread();
  X11ScraperPoster(ScraperPosterMetaData* pScraperPosterMetaData);
  ~X11ScraperPoster();
  void sendScrapingStoppedMessage();
private:
  PlatformScreen* screen;
  PlatformPoster* poster;
  int compressRegionToBuffer(deskblast::Region* region, int offset);
  void initDesktops();
  int writeHeader(int tileCount);
  bool writeHash(void *pixels, deskblast::Region* sourceRegion, int destOffset);
  unsigned char mSendBuffer[SEND_BUFFER_LENGTH];
  char postUrl[POST_URL_BUFFER_LENGTH];
  int screenWidth;
  int screenHeight;
  std::set<HashString> * sentHashes;
  std::map<HashString, SubtileInfo> * subtileMap;
  void cleanUpHash(void);

  void hashPixels(void *pixels, deskblast::Region* sourceRegion, char* dest);
  void hashBytes(unsigned char *pixels, int length, char* dest);
  unsigned long millisElapsed;
  void updateTicks();
  unsigned long getSleepTime();
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
  static void buildUrlFromMetaData(ScraperPosterMetaData* pScraperPosterMetaData, char* buffer, const char* servlet);
  GifSpriter* gifSpriter;
  int compressIfNeededAndWriteTile(
          deskblast::Region * region, int cursor, int hashOffset, int spriteX, int spriteY, bool compress);
  int writeCompressedTile(deskblast::Region* region, int cursor, HashString* hs, int spriteX, int spriteY,
						unsigned char * gifStart, int gifLength);  		
  void doScreenShot();		
  void sleepAndUpdateTicks();
};
