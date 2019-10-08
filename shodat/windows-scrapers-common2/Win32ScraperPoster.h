#include <set>
#include <vector>
#include <map>

#include "Region.h"
#include "Poster.h"
#include "ScraperPosterMetaData.h"
#include "resource.h"
#include "HashString.h"
#include "SubtileInfo.h"
#include "../GifEncoder2/GifSpriter.h"
extern "C" {
    #include "jpeglib.h"
}


class Win32ScraperPoster{

public:
  const static int SCRAPE_THROTTLE_MS = 333;
  const static int COMMAND_EXPIRATION_MS = 20000;
  bool mContinueScraping;
  bool mShutting;
  bool mSendKeyframe;
  bool networkError;
  void doScreenScrape();
  void expireDisplay();
  HANDLE resumeScraping(void (*pStatusFunc)(LPVOID context, const char* status), LPVOID context);
  void stopScraping();
  void stopScrapingThread();
  Win32ScraperPoster(ScraperPosterMetaData* pScraperPosterMetaData, const char* moduleName);
  void invokeStatusCallback(const char* status);
  void clearStatusContext();
  ~Win32ScraperPoster();
  HANDLE mScraperThreadHandle;
  HANDLE mUiThreadHandle;
  HWND mHwndUi;
  void sendScrapingStoppedMessage();
  char* getModuleName();
  std::string  getBlastId(){
    if(pScraperPosterMetaData){
      return pScraperPosterMetaData->getBlastId();
    }
    return NULL;
  }
  std::string getViewUrl();
private:
  
  void FAR* lpvpxldata_0;
  void FAR* lpvpxldata_1;
  int compressRegionToBuffer(deskblast::Region* region, int offset, byte* lpvpxldata);
  void initDesktops();
  int writeHeader(int tileCount);
  bool writeHash(void *pixels, deskblast::Region* sourceRegion, int destOffset);
  unsigned char mSendBuffer[1024*1024];
  int screenWidth;
  int screenHeight;
  std::set<HashString> * sentHashes;
  std::map<HashString, SubtileInfo> * subtileMap;
  void cleanUpHash(void);

  void hashPixels(void *pixels, deskblast::Region* sourceRegion, char* dest);
  void hashBytes(unsigned char *pixels, int length, char* dest);
  BITMAPINFO bmInfo;
  void initBMInfo();
  DWORD ticks;
  DWORD ticksLastCommand;
  void updateTicks();
  DWORD getSleepTime();
  void FAR* getNextBuffer();
  void FAR* getCurrentBuffer();
  void FAR* getPreviousBuffer();
  int currentBuffer;
  deskblast::Region ** regions;
  int regionCount;
  int mouseX;
  int mouseY;
  void initRegions();
  int checkForChanges();
  bool updateMouse();
  bool commandExpired();
  void doMedallion(RECT * rect, HDC hmemDC);
  Poster* poster;
  //void writeScrapeToFile(BYTE* buffer, int length);
  static const int HASH_LENGTH_BYTES = 32;
  bool displayChanged;
  bool displayExpired();
  void freeDeskBuffers();
  void allocateDeskBuffers(int size);
  void resetDesktops();
  void (*pStatusFunc)(LPVOID context, const char* status);
  LPVOID statusContext;
  ScraperPosterMetaData* pScraperPosterMetaData;
  //static void buildUrlFromMetaData(ScraperPosterMetaData* pScraperPosterMetaData, char* buffer, const char* servlet);
  char moduleName[MAX_PATH];
  //GifEncoder2* gifEncoder;
  GifSpriter* gifSpriter;
  int compressIfNeededAndWriteTile(
          deskblast::Region * region, int cursor, int hashOffset, int spriteX, int spriteY, bool compress);
  int writeCompressedTile(deskblast::Region* region, int cursor, HashString* hs, int spriteX, int spriteY,
						unsigned char * gifStart, int gifLength);  		
  void doScreenShot();		
};
