#include "Win32ScraperPoster.h"

#include <windows.h>
#include <windowsx.h>
//#ifdef NEEDS_NETINET
////#include <netinet/in.h>
//#include <winsock2.h>
//#endif
#include "Trace.h"
#include <resource.h>


extern "C" {
    #include "md5.h"
    #define _BASETSD_H_
    #include "jpeglib.h"
    #include "jpeg_mem_dest.h"
}

const char LOG[] = "skreen.io_scrape.log";

void s_trace(const char *msg) {
    trace(LOG, msg);
}

DWORD WINAPI ScraperThreadFunction(LPVOID lpParam);

Win32ScraperPoster::Win32ScraperPoster(
    ScraperPosterMetaData* pScraperPosterMetaData, const char* moduleName) {
    s_trace("Win32ScraperPoster ctor");
    s_trace(__DATE__);
    s_trace(__TIME__);
    sentHashes = new std::set<HashString>();//new HashTree();
    subtileMap = new std::map<HashString, SubtileInfo>();
    poster = NULL;
    mScraperThreadHandle = NULL;
    mUiThreadHandle = NULL;
    networkError = false;
    this->pScraperPosterMetaData = pScraperPosterMetaData;
    mHwndUi = NULL;
    mSendKeyframe = true;
    //char postUrl[4096];
    //const char* servlet = "scrapes";
    //buildUrlFromMetaData(pScraperPosterMetaData, postUrl, servlet);
    if(moduleName != NULL){
    	strcpy(this->moduleName, moduleName);
    }else{
    	HMODULE hmodule = GetModuleHandle(NULL);
    	// TODO: error checking
    	GetModuleFileName(hmodule, this->moduleName, MAX_PATH);
    }
    s_trace("module_name:");
    s_trace(this->getModuleName());
    //s_trace(postUrl);
    //gifEncoder = NULL;
    gifSpriter = NULL;
    this->poster = new Poster(pScraperPosterMetaData->getHost(),
                              pScraperPosterMetaData->getPort(), pScraperPosterMetaData->buildPath("scrapes"),
                              pScraperPosterMetaData->getSsl());
}
Win32ScraperPoster::~Win32ScraperPoster() {
    s_trace("Win32ScraperPoster dtor");
    stopScraping();
    if (this->poster) {
        delete (this->poster);
        this->poster = NULL;
        cleanUpHash();
    }
    if(gifSpriter) delete gifSpriter;
}
char* Win32ScraperPoster::getModuleName() {
    s_trace("Win32ScraperPoster::getModuleName");
   	return this->moduleName;
}
/*
void Win32ScraperPoster::buildUrlFromMetaData(
    ScraperPosterMetaData* pScraperPosterMetaData, char* buffer,
    const char* servlet) {
    char contextPart[4096];
    if (strlen(pScraperPosterMetaData->getContext())) {
        wsprintf(contextPart, "%s/", pScraperPosterMetaData->getContext());
    } else {
        contextPart[0] = 0;
    }
    wsprintf(buffer, "%s%s?blastId=%s&blasterId=%s&key=%s", contextPart,
             servlet, pScraperPosterMetaData->getBlastId(),
             pScraperPosterMetaData->getBlasterId(),
             pScraperPosterMetaData->getKey());
}
*/
DWORD WINAPI UiThreadFunction(LPVOID lpParam);

HANDLE Win32ScraperPoster::resumeScraping(void(*pStatusFunc)(LPVOID, const char*),
                                        LPVOID context) {
    s_trace("entering resumeScraping...");
    if (mUiThreadHandle) {
        return mUiThreadHandle;
    }
    if (pStatusFunc && context) {
        this->pStatusFunc = pStatusFunc;
        this->statusContext = context;
    } else {
        this->pStatusFunc = NULL;
        this->statusContext = NULL;
    }
    ticks = 0;
    ticksLastCommand = 0;
    currentBuffer = 0;
    mShutting = FALSE;
    displayChanged = FALSE;
    initDesktops();
    mContinueScraping = TRUE;
    mShutting = FALSE;
    mUiThreadHandle = CreateThread(NULL, NULL, UiThreadFunction, this, NULL,
                                   NULL);
    s_trace("...exiting resumeScraping.");
    return mUiThreadHandle;
}

std::string Win32ScraperPoster::getViewUrl(){
    return pScraperPosterMetaData->getViewUrl();
}

void Win32ScraperPoster::invokeStatusCallback(const char* status) {
    s_trace("Win32ScraperPoster::invokeStatusCallback");
    if (this->pStatusFunc && this->statusContext) {
        this->pStatusFunc(this->statusContext, status);
    }
}
void Win32ScraperPoster::clearStatusContext() {
    s_trace("Win32ScraperPoster::clearStatusContext");
    this->statusContext = NULL;
    this->pStatusFunc = NULL;
}
INT_PTR CALLBACK ShowingProc(HWND hDlg, UINT message, WPARAM wParam,
                             LPARAM lParam);

DWORD WINAPI UiThreadFunction(LPVOID lpParam) {
    s_trace("entering UiThreadFunction");
    Win32ScraperPoster* instance = (Win32ScraperPoster*) lpParam;
    HMODULE hMod = GetModuleHandle(instance->getModuleName());
    instance->mHwndUi = CreateDialogParam(hMod,
                  MAKEINTRESOURCE(IDD_ABOUTBOX), NULL, ShowingProc, (LPARAM) instance);
    ShowWindow(instance->mHwndUi, SW_SHOW);
    //std::string showUrl = instance->getViewUrl();
    std::string blastId = instance->getBlastId();
    std::string showUrl = "http://skreen.io/" + blastId;
    std::string consentMessage = "Your screen will be shown at " + showUrl + ". This link will be copied to your clipboard.";
    int consentResult = MessageBox(instance->mHwndUi, consentMessage.c_str(), "Show your screen", MB_OKCANCEL | MB_ICONWARNING);
    if(consentResult != IDOK){
        return 0;
    }
    BOOL bRet;
    MSG msg;
    BOOL bClip = OpenClipboard(instance->mHwndUi);
    if(bClip){
        s_trace("clipboard opened");
    }
    s_trace("Win32ScraperPoster emptying clipboard");
    EmptyClipboard();
    LPTSTR lptstrCopy;
    int strLength = strlen(showUrl.c_str());
    HGLOBAL link = GlobalAlloc(GMEM_MOVEABLE, (strLength + 1) * sizeof(TCHAR));
    if(link){
        lptstrCopy = (LPTSTR)GlobalLock(link);
        memcpy(lptstrCopy, showUrl.c_str(), strLength  * sizeof(TCHAR));
        lptstrCopy[strLength] = (TCHAR) 0;
        GlobalUnlock(link);
        SetClipboardData(CF_TEXT, link);
        s_trace("copied to clipboard");
        CloseClipboard();
    }
    instance->mScraperThreadHandle = CreateThread(NULL, NULL,
                                     ScraperThreadFunction, instance, NULL, NULL);
    while ((bRet = GetMessage(&msg, NULL, 0, 0)) != 0) {
        if (bRet == -1) {
            // Handle the error and possibly exit
        } else if (!IsWindow(instance->mHwndUi) || !IsDialogMessage(
                       instance->mHwndUi, &msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    s_trace("exiting UiThreadFunction");
}

INT_PTR CALLBACK ShowingProc(HWND hDlg, UINT message, WPARAM wParam,
                             LPARAM lParam) {
    RECT rectDialog;
    RECT rectDesktop;
    int width;
    int height;
    Win32ScraperPoster* instance = NULL;
    char buff[1024];
    POINTS points;
    static POINTS clientOffset;
    RECT rect;

    switch (message) {
    case WM_INITDIALOG:
        s_trace("entering WM_INITDIALOG");
        instance = (Win32ScraperPoster*) lParam;
        SetWindowLongPtr(hDlg, GWLP_USERDATA, (LONG_PTR) instance);
        GetWindowRect(GetDesktopWindow(), &rectDesktop);
        GetWindowRect(hDlg, &rectDialog);
        width = rectDialog.right - rectDialog.left;
        height = rectDialog.bottom - rectDialog.top;
        SetFocus(hDlg);
        SetWindowPos(hDlg, HWND_TOPMOST, (rectDesktop.right/2) - (width/2),
                     -4, 0, 0,
                     SWP_SHOWWINDOW | SWP_NOSIZE);
        HANDLE hIcon;
        s_trace("mod name:");
        s_trace(instance->getModuleName());
        hIcon = LoadImage(GetModuleHandle(instance->getModuleName()),
                          MAKEINTRESOURCE(IDI_SMALL), IMAGE_ICON, GetSystemMetrics(
                              SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), 0);
        if (hIcon) {
            SendMessage(hDlg, WM_SETICON, ICON_SMALL, (LPARAM) hIcon);
        }
        HANDLE hIconBig;
        hIconBig = LoadImage(GetModuleHandle(instance->getModuleName()),
                             MAKEINTRESOURCE(IDI_BIG), IMAGE_ICON, 32, 32, 0);
        if (hIconBig) {
            SendMessage(hDlg, WM_SETICON, ICON_BIG, (LPARAM) hIconBig);
        }
	    SendMessage(hDlg, WM_SETTEXT, NULL, (LPARAM)"skreen.io");
	    HWND hwndStatic;
	    hwndStatic = FindWindowEx(hDlg, NULL, NULL, "shoing");
        SendMessage(hwndStatic, WM_SETTEXT, NULL, (LPARAM)instance->getBlastId().c_str());
        s_trace("returning from WM_INITDIALOG");
        return (INT_PTR) TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
            s_trace("calling stopScrapingThread...");
            instance = (Win32ScraperPoster*) GetWindowLongPtr(hDlg,
                       GWLP_USERDATA);
            instance->stopScrapingThread();
            s_trace("...returned from stopScrapingThread.");
            s_trace("in WM_COMMAND calling DestroyWindow...");
            DestroyWindow(hDlg);
            s_trace("...returned from DestroyWindow");
            instance->mHwndUi = NULL;
            instance->mUiThreadHandle = NULL;
            instance->invokeStatusCallback(
                "status callback: showing was stoppped");
            instance->clearStatusContext();
            if(!instance->networkError){
                instance->sendScrapingStoppedMessage();
            }
            return (INT_PTR) TRUE;
        } else {
            s_trace("other WM_COMMAND");
        }
        break;
    case WM_LBUTTONDOWN:
        SetCapture(hDlg);
        clientOffset = MAKEPOINTS(lParam);
        break;
    case WM_MOUSEMOVE:
        if (hDlg == GetCapture()) {
            GetWindowRect(hDlg, &rect);
            points = MAKEPOINTS(lParam);
            MoveWindow(hDlg, (rect.left+points.x)-clientOffset.x, (rect.top+points.y)-clientOffset.y,
                       rect.right-rect.left, rect.bottom-rect.top, true);
        }
        break;
    case WM_LBUTTONUP:
        ReleaseCapture();
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        sprintf(buff, "other WM: %d", message);
        s_trace(buff);
        break;
    }
    return (INT_PTR) FALSE;
}
void Win32ScraperPoster::stopScraping() {
    s_trace("entering stopScraping");
    if (this->mHwndUi) {
        PostMessage(this->mHwndUi, WM_COMMAND, IDCANCEL, NULL);
        //SendMessage(this->mHwndUi, WM_COMMAND, IDCANCEL, NULL);
    }
    s_trace("leaving stopScraping");
}
void Win32ScraperPoster::stopScrapingThread() {
    s_trace("entering stopScrapingThread...");
    mShutting = TRUE;
    if (mScraperThreadHandle) {
        this->poster->interrupt();
        DWORD result = WaitForSingleObject(mScraperThreadHandle, INFINITE);
        mScraperThreadHandle = NULL;
    }
    freeDeskBuffers();
    s_trace("...exiting stopScrapingThread.");
}
DWORD WINAPI ScraperThreadFunction(LPVOID lpParam) {
    s_trace("entering ScraperThreadFunction.");
    Win32ScraperPoster * instance = (Win32ScraperPoster*) lpParam;
    while (true) {
        if (instance->mContinueScraping && instance->mShutting == FALSE) {
            instance->mContinueScraping = false;
            instance->doScreenScrape();
        } else {
            break;
        }
    }
    s_trace("exiting ScraperThreadFunction.");
    return 0;
}
void Win32ScraperPoster::allocateDeskBuffers(int iByteCount) {
    lpvpxldata_0 = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, iByteCount);
    lpvpxldata_1 = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, iByteCount);
}
void Win32ScraperPoster::freeDeskBuffers() {
    s_trace("freeing desk buffers");
    if (lpvpxldata_0) {
        HeapFree(GetProcessHeap(), 0, lpvpxldata_0);
        lpvpxldata_0 = NULL;
    }
    if (lpvpxldata_1) {
        HeapFree(GetProcessHeap(), 0, lpvpxldata_1);
        lpvpxldata_1 = NULL;
    }
}
void Win32ScraperPoster::resetDesktops() {
    freeDeskBuffers();
    initDesktops();
}
void Win32ScraperPoster::initDesktops() {
    // TODO: handle case where user re-sizes display
    s_trace("Win32ScraperPoster::initDesktops");
    HDC hScreenDC;
    hScreenDC = GetWindowDC(0);
    screenWidth = GetDeviceCaps(hScreenDC, HORZRES);
    screenHeight = GetDeviceCaps(hScreenDC, VERTRES);
    ReleaseDC(0, hScreenDC);

    // https://msdn.microsoft.com/en-us/library/windows/desktop/dd318229(v=vs.85).aspx
    // Calculating Surface Stride
    // In an uncompressed bitmap, the stride is the number of bytes needed to go from the start of one row of pixels to the start of the next row. The image format defines a minimum stride for an image. In addition, the graphics hardware might require a larger stride for the surface that contains the image.
    // For uncompressed RGB formats, the minimum stride is always the image width in bytes, rounded up to the nearest DWORD. You can use the following formula to calculate the stride:
    // stride = ((((biWidth * biBitCount) + 31) & ~31) >> 3)

    int iPaddingBytes = (screenWidth * deskblast::Region::BYTES_PER_PIXEL) % 4;
    if(iPaddingBytes){
        s_trace("Win32ScraperPoster::initDesktops padding required");
    }else{
        s_trace("Win32ScraperPoster::initDesktops padding not required");
    }
    int iByteCount = (screenWidth * deskblast::Region::BYTES_PER_PIXEL + iPaddingBytes) * screenHeight;
    allocateDeskBuffers(iByteCount);
    initBMInfo();
    this->initRegions();
}
void Win32ScraperPoster::initBMInfo() {
    s_trace("Win32ScraperPoster::initBMInfo");
    bmInfo.bmiHeader.biSize = 40;
    bmInfo.bmiHeader.biWidth = screenWidth;
    bmInfo.bmiHeader.biHeight = -screenHeight;
    bmInfo.bmiHeader.biPlanes = 1;
    bmInfo.bmiHeader.biBitCount = deskblast::Region::BYTES_PER_PIXEL * 8;
    bmInfo.bmiHeader.biCompression = 0;
    bmInfo.bmiHeader.biSizeImage = 0;
    bmInfo.bmiHeader.biXPelsPerMeter = 0;
    bmInfo.bmiHeader.biYPelsPerMeter = 0;
    bmInfo.bmiHeader.biClrUsed = 0;
    bmInfo.bmiHeader.biClrImportant = 0;
}
bool Win32ScraperPoster::commandExpired(void) {
    DWORD now = GetTickCount();
    if (now < this->ticksLastCommand) {
        return true;
    } else if ((now - this->ticksLastCommand) > COMMAND_EXPIRATION_MS) {
        return true;
    } else {
        return false;
    }
}
DWORD Win32ScraperPoster::getSleepTime(void) {
    if (ticks == 0)
        return 0;
    DWORD now = GetTickCount();
    DWORD diff = now - ticks;
    if (diff < 0 || diff > SCRAPE_THROTTLE_MS) {
        return 0;
    } else {
        return SCRAPE_THROTTLE_MS - diff;
    }
}
void Win32ScraperPoster::updateTicks() {
    ticks = GetTickCount();
}
void FAR* Win32ScraperPoster::getNextBuffer() {
    if (this->currentBuffer == 0) {
        this->currentBuffer = 1;
    } else {
        this->currentBuffer = 0;
    }
    return this->getCurrentBuffer();
}
void FAR* Win32ScraperPoster::getCurrentBuffer() {
    if (this->currentBuffer == 0) {
        return this->lpvpxldata_0;
    } else {
        return this->lpvpxldata_1;
    }
}
void FAR* Win32ScraperPoster::getPreviousBuffer() {
    if (this->currentBuffer == 0) {
        return this->lpvpxldata_1;
    } else {
        return this->lpvpxldata_0;
    }
}
void Win32ScraperPoster::doMedallion(RECT * rect, HDC hmemDC) {
    // draw over 'You are showing your desktop' dialog
    HBITMAP hbmp = (HBITMAP)LoadImage(GetModuleHandle(this->getModuleName()),
                                      MAKEINTRESOURCE(IDBMP_MEDALLION),
                                      IMAGE_BITMAP,
                                      0, 0,
                                      LR_SHARED
                                     );
    HDC mark = CreateCompatibleDC(hmemDC);
    HGDIOBJ original = NULL;
    original = SelectObject(mark, hbmp);
    BitBlt(hmemDC, rect->left, rect->top,
           rect->right - rect->left, rect->bottom - rect->top,
           mark, 0, 0, SRCCOPY);
    SelectObject(mark, original);
    DeleteDC(mark);
    DeleteObject(hbmp);
}

void Win32ScraperPoster::doScreenShot() {
    HDC hScreenDC;
    HDC hmemDC;
    HBITMAP hmemBM;
    HGDIOBJ hOrigObj;
    hScreenDC = GetWindowDC(0);
    hmemDC = CreateCompatibleDC(hScreenDC);
    hmemBM = CreateCompatibleBitmap(hScreenDC, screenWidth, screenHeight);
    hOrigObj = SelectObject(hmemDC, hmemBM);
    RECT rcDlg;
    GetWindowRect(mHwndUi, &rcDlg);
    BitBlt(hmemDC, 0, 0, screenWidth, screenHeight, hScreenDC, 0, 0, SRCCOPY | CAPTUREBLT);
    doMedallion(&rcDlg, hmemDC);
    GetDIBits(hmemDC, hmemBM, 0, screenHeight, this->getNextBuffer(), &bmInfo, DIB_RGB_COLORS);
    SelectObject(hmemDC, hOrigObj);
    DeleteObject(hmemBM);
    DeleteDC(hmemDC);
    ReleaseDC(0, hScreenDC);
}

void Win32ScraperPoster::doScreenScrape() {
    DWORD wait = getSleepTime();
    if (wait > 0) {
        //fprintf(mlogfile, "sleeping %d milliseconds", wait);
        //fflush(mlogfile);
        Sleep(wait);
    } else {
        //fprintf(mlogfile, "entered doScreenScrape");
        //fflush(mlogfile);
    }
    updateTicks();
    if (displayExpired()) {
        resetDesktops();
    }
    doScreenShot();
    char logBuff[512];
    int changeCount = this->mSendKeyframe ? this->regionCount : this->checkForChanges();
    sprintf(logBuff, "%d regions (%d changed; %d x %d)", this->regionCount, changeCount, screenWidth, screenHeight);
    s_trace(logBuff);
    //fprintf(this->mlogfile, "%d changes found", changeCount);
    //fprintf(this->mlogfile, "postUrl: %s", postUrl);
    //fflush(this->mlogfile);
    long start = GetTickCount();
    bool mouseMoved = updateMouse();
    if (changeCount > 0 || mouseMoved) {
        int cursor = 0;
        cursor += writeHeader(changeCount);
        char hashBuffer[HASH_LENGTH_BYTES];
        // MD5 all tiles, check if already sent
        std::vector<deskblast::Region*> newRegions;
        std::map<HashString, SubtileInfo>::iterator it;
        std::map<deskblast::Region, HashString> rectHashes;
        std::map<deskblast::Region, HashString>::iterator rectHashesIt;
        for (int k = 0; k < this->regionCount; k++) {
            if (this->regions[k]->isChanged() || this->mSendKeyframe) {
                hashPixels((void*)(this->getCurrentBuffer()), this->regions[k], (char*)hashBuffer);
                HashString hs(hashBuffer);
                it = subtileMap->find(hs);
                if (it != subtileMap->end()) {
                    // this subtile has been sent before so write it as back ref
                    SubtileInfo found = it->second;
                    int hashOffset = cursor;
                    found.copyTo((char*)mSendBuffer, cursor);
                    cursor += (28 + HASH_LENGTH_BYTES); // 3 (7) ints (4 bytes each) plus hash
                    cursor = compressIfNeededAndWriteTile(this->regions[k], cursor, hashOffset, found.getX(), found.getY(), false);
                } else {
                    newRegions.push_back(this->regions[k]);
                    rectHashes.insert(std::pair<deskblast::Region,HashString>(*(this->regions[k]), hs));
                }
            }
        }

        if(!this->gifSpriter){
            this->gifSpriter = new GifSpriter(screenWidth, screenHeight);
        }
        std::vector<deskblast::Region*> ungiffableRegions;
        std::vector<GifSpriteInfo*> gifSpriteInfos;

        // TODO: we note the hash of the upload before we know it succeeded
        // so if it is interrupted, we find it among the 'sent' uploads but it
        // never actually uploaded

        gifSpriter->makeSprites(&newRegions, (unsigned char*) this->getCurrentBuffer(), &gifSpriteInfos, &ungiffableRegions);
        printf("%d ungiffable regions\n", ungiffableRegions.size());
        printf("%d GifSpriteInfos\n", gifSpriteInfos.size());
        for(int i = 0; i < gifSpriteInfos.size(); i++){
            GifSpriteInfo* gsiPtr = gifSpriteInfos.at(i);
            hashBytes(gsiPtr->getGifStart(), gsiPtr->getGifLength(), (char*)hashBuffer);
            HashString hs(hashBuffer);
            boolean spriteSent = false;
            for(int j = 0; j < gifSpriteInfos.at(i)->getMappedRegions()->size(); j++){
                MappedRegion mr = gifSpriteInfos.at(i)->getMappedRegions()->at(j);
                if (spriteSent || this->sentHashes->count(hs)) {
                    int hashOffset = cursor;
                    hs.copyTo((char*)mSendBuffer, cursor);
                    cursor += (28 + HASH_LENGTH_BYTES); // 3 (7) ints (4 bytes each) plus hash
                    cursor = compressIfNeededAndWriteTile(mr.getRegion(), cursor, hashOffset,
                                    mr.getTranslateX(), mr.getTranslateY(), false);
                }else{
                    cursor = writeCompressedTile(mr.getRegion(), cursor, &hs, mr.getTranslateX(), mr.getTranslateY(),
                            gsiPtr->getGifStart(), gsiPtr->getGifLength());
                    this->sentHashes->insert(hs);
                    spriteSent = true;
                }
                rectHashesIt = rectHashes.find(*(mr.getRegion()));
                SubtileInfo si(hs.getHash(), mr.getTranslateX(), mr.getTranslateY());
                this->subtileMap->insert(std::pair<HashString, SubtileInfo>(rectHashesIt->second, si));
            }
        }

        std::vector<deskblast::Region> mergedRegions;
        for (int i = 0; i < ungiffableRegions.size(); i++) {
            mergedRegions.push_back(*(ungiffableRegions[i]));
        }
        deskblast::Region::doMergeRegions(mergedRegions,
                deskblast::Region::TILE_WIDTH * deskblast::Region::TILE_HEIGHT * deskblast::Region::MAX_MERGED_TILES);
        //char logBuff[512];
        sprintf(logBuff, "%d regions merged to %d", ungiffableRegions.size(), mergedRegions.size());
        s_trace(logBuff);
        for (int i = 0; i < mergedRegions.size(); i++) {
            hashPixels(this->getCurrentBuffer(), (deskblast::Region*)(&mergedRegions[i]), (char*)hashBuffer);
            HashString hs(hashBuffer);
            if (this->sentHashes->count(hs)) {
                for (int m = 0; m < ungiffableRegions.size(); m++) {
                    if (mergedRegions[i].contains(*(ungiffableRegions[m]))) {
                        int hashOffset = cursor;
                        hs.copyTo((char*)mSendBuffer, cursor);
                        cursor += (28 + HASH_LENGTH_BYTES); // 3 (7) ints (4 bytes each) plus hash
                        cursor = compressIfNeededAndWriteTile(ungiffableRegions[m], cursor, hashOffset,
                                   ungiffableRegions[m]->getX() - mergedRegions[i].getX(), ungiffableRegions[m]->getY() - mergedRegions[i].getY(),
                                                              false);
                    }
                }
            } else {
                this->sentHashes->insert(hs);
                boolean spriteSent = false;
                for (int m = 0; m < ungiffableRegions.size(); m++) {
                    if (mergedRegions[i].contains(*(ungiffableRegions[m]))) {
                        if (!spriteSent) {
                            int hashOffset = cursor;
                            hs.copyTo((char*)mSendBuffer, cursor);
                            cursor += (28 + HASH_LENGTH_BYTES); // 3 (7) ints (4 bytes each) plus hash
                            cursor = compressIfNeededAndWriteTile(&(mergedRegions[i]), cursor, hashOffset,
                                                                  ungiffableRegions[m]->getX() - mergedRegions[i].getX(),
                                                                  ungiffableRegions[m]->getY() - mergedRegions[i].getY(),
                                                                  true);
                            spriteSent = true;
                        } else {
                            int hashOffset = cursor;
                            hs.copyTo((char*)mSendBuffer, cursor);
                            cursor += (28 + HASH_LENGTH_BYTES); // 3 (7) ints (4 bytes each) plus hash
                            cursor = compressIfNeededAndWriteTile(ungiffableRegions[m], cursor, hashOffset,
                                                                  ungiffableRegions[m]->getX() - mergedRegions[i].getX(),
                                                                  ungiffableRegions[m]->getY() - mergedRegions[i].getY(),
                                                                  false);
                        }
                        rectHashesIt = rectHashes.find(*(ungiffableRegions[m]));
                        SubtileInfo si(hs.getHash(),
                                       ungiffableRegions[m]->getX() - mergedRegions[i].getX(),
                                       ungiffableRegions[m]->getY() - mergedRegions[i].getY());
                        this->subtileMap->insert(std::pair<HashString, SubtileInfo>(rectHashesIt->second, si));
                    }
                }
            }
            //}
        }
        sprintf(logBuff, "compressing %d tiles took %d milliseconds",
                this->regionCount, (int)(GetTickCount() - start));
        s_trace(logBuff);
        this->mSendKeyframe = false;
        if (!mShutting) {
            BYTE response[4096];
            DWORD responseLength = 0;
            BOOL result = this->poster->sendPost(mSendBuffer, cursor, response,
                                                 4096, &responseLength);
            //writeScrapeToFile(mSendBuffer, cursor);
            //fprintf(this->mlogfile, "...done posting:%d", result);
            //fflush(this->mlogfile);
            if (result) {
                char* bytes = (char*) response;
                char command = bytes[0];
                this->ticksLastCommand = GetTickCount();
                //fprintf(this->mlogfile, "got data %d", command);
                //fflush(this->mlogfile);
                this->mContinueScraping = (command == 1 || command == 2);
                this->mSendKeyframe = (command == 2);
            } else {
                this->mContinueScraping = false;
                s_trace("sendPost failed");
                if(!this->poster->interrupted){
                    s_trace("sendPost failed: not interrupted (netowrk error)");
		    this->networkError = true;
                    MessageBox(this->mHwndUi, "Error connecting to skreen.io. Please check your network.", 
                        NULL, MB_OK | MB_ICONEXCLAMATION | MB_APPLMODAL);
                    this->stopScraping();
                }else{
                    s_trace("sendPost failed: interrupted");
                }
                //this->networkError = true;
                //fprintf(this->mlogfile, "got stop command");
                //fflush(this->mlogfile);
            }
        }
    } else {
        if (this->commandExpired()) {
            this->mSendKeyframe = true;
            //fprintf(this->mlogfile, "command expired -- forcing keyframe");
        }
        this->mContinueScraping = true;
        //SetEvent(this->mScrapeControlEvent);
        //fprintf(this->mlogfile, "no changes");
        //fflush(this->mlogfile);
    }
}
int Win32ScraperPoster::writeCompressedTile(deskblast::Region* region, int cursor,
        HashString* hs, int spriteX, int spriteY,
        unsigned char * gifStart, int gifLength){
    s_trace("starting writeCompressedTile...");
    hs->copyTo((char*)mSendBuffer, cursor);
    int *now_index_as_int = (int*) (mSendBuffer + cursor + HASH_LENGTH_BYTES);
    now_index_as_int[0] = htonl(gifLength);
    now_index_as_int[1] = htonl(region->getX());
    now_index_as_int[2] = htonl(region->getY());
    now_index_as_int[3] = htonl(region->getWidth());
    now_index_as_int[4] = htonl(region->getHeight());
    now_index_as_int[5] = htonl(spriteX);
    now_index_as_int[6] = htonl(spriteY);
    cursor += (28 + HASH_LENGTH_BYTES); // 3 (7) ints (4 bytes each) plus hash
    memcpy(&mSendBuffer[cursor], gifStart, gifLength);
    return cursor + gifLength;
}
int Win32ScraperPoster::compressIfNeededAndWriteTile(
        deskblast::Region * region,
    int cursor, int hashOffset, int spriteX, int spriteY, bool compress) {
    s_trace("starting compressIfNeededAndWriteTile...");
    int imageSize = 0;
    if(compress){
        // TODO: if screen size changes GifEncoder2 will blow up
        if (imageSize < 1) {
            imageSize = compressRegionToBuffer(region, cursor,
                                               (byte*) this->getCurrentBuffer());
            s_trace("compressIfNeededAndWriteTile returned from jpeg encode");
        }
        cursor += imageSize;
    }
    int *now_index_as_int = (int*) (mSendBuffer + hashOffset + HASH_LENGTH_BYTES);
    now_index_as_int[0] = htonl(imageSize);
    now_index_as_int[1] = htonl(region->getX());
    now_index_as_int[2] = htonl(region->getY());
    now_index_as_int[3] = htonl(region->getWidth()); // TODO: this is the width and height of the merged rect but
    now_index_as_int[4] = htonl(region->getHeight());// it should be the width and height of the first region mapped to it
    now_index_as_int[5] = htonl(spriteX);
    now_index_as_int[6] = htonl(spriteY);
    s_trace("compressIfNeededAndWriteTile returning");
    return cursor;
}

/*
 void Win32ScraperPoster::writeScrapeToFile(BYTE* buffer, int length){
 FILE * outfile;
 if ((outfile = fopen("c://scrapes//scrape.bin", "wb")) == NULL) {
 fprintf(stderr, "can't open scrape.bin");
 exit(1);
 }
 for(int i = 0; i < length; i++){
 fputc(buffer[i], outfile);
 }
 fclose(outfile);
 }
 */
bool Win32ScraperPoster::updateMouse() {
    POINT mouse;
    BOOL res = GetCursorPos(&mouse);
    if(!res){
        return false;
    }
    if (mouse.x > screenWidth)
        mouse.x = screenWidth;
    if (mouse.y > screenHeight)
        mouse.y = screenHeight;
    if (mouse.x != mouseX || mouse.y != mouseY) {
        mouseX = mouse.x;
        mouseY = mouse.y;
        return true;
    } else {
        return false;
    }
}
void Win32ScraperPoster::initRegions(void) {
    s_trace("Win32ScraperPoster::initRegions");
    this->regions = deskblast::Region::allocateRegionsForBounds(screenWidth, screenHeight, &regionCount);
}
int Win32ScraperPoster::checkForChanges() {
    int changes = 0;
    for (int i = 0; i < this->regionCount; i++) {
        bool result = this->regions[i]->checkForChange(
                          this->getCurrentBuffer(), this->getPreviousBuffer());
        if (result)
            changes++;
    }
    return changes;
}
void Win32ScraperPoster::hashBytes(unsigned char* pixels, int length, char* dest) {
    MD5_CTX ctx;
    MD5Init(&ctx);
    MD5Update(&ctx, pixels, length);
    MD5Final(&ctx);
    for (int i = 0; i < 16; i++) {
        wsprintf((char*) &(dest[(i * 2)]), "%02x", ctx.digest[i]);
    }

}
void Win32ScraperPoster::hashPixels(void* pixels, deskblast::Region* sourceRegion, char* dest) {
    MD5_CTX ctx;
    MD5Init(&ctx);
    int scanlines = sourceRegion->getHeight();
    int scanlineLengthInBytes = sourceRegion->getWidth() * deskblast::Region::BYTES_PER_PIXEL;
    for (int i = 0; i < scanlines; i++) {
        MD5Update(&ctx, sourceRegion->getScanline(i, pixels), scanlineLengthInBytes);
    }
    MD5Final(&ctx);
    for (int i = 0; i < 16; i++) {
        wsprintf((char*) &(dest[(i * 2)]), "%02x", ctx.digest[i]);
    }
}
int Win32ScraperPoster::writeHeader(int changedTileCount) {
    int *now_index_as_int = (int*) mSendBuffer;
    now_index_as_int[0] = htonl(changedTileCount == this->regionCount ? 1 : 0); // keyframe
    now_index_as_int[1] = htonl(screenWidth);
    now_index_as_int[2] = htonl(screenHeight);
    now_index_as_int[3] = htonl(mouseX);
    now_index_as_int[4] = htonl(mouseY);
    now_index_as_int[5] = htonl(changedTileCount);
    return 6 * 4;
}
int Win32ScraperPoster::compressRegionToBuffer(deskblast::Region* region, int offset,
        byte* lpvpxldata) {
    int jpegSize;
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);
    my_jpeg_mem_dest(&cinfo, &mSendBuffer[offset], (1024 * 1024) - offset);
    cinfo.input_components = deskblast::Region::BYTES_PER_PIXEL;
    cinfo.in_color_space = JCS_EXT_BGRA;
    cinfo.image_width = region->getWidth();
    cinfo.image_height = region->getHeight();
    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, 65, TRUE);
    jpeg_start_compress(&cinfo, TRUE);
    JSAMPROW row_pointer[1];
    while (cinfo.next_scanline < cinfo.image_height) {
        row_pointer[0] = region->getScanline(cinfo.next_scanline, lpvpxldata);
        jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }
    jpeg_finish_compress(&cinfo);
    jpegSize = my_jpeg_mem_size(&cinfo);
    jpeg_destroy_compress(&cinfo);
    return jpegSize;
}
/*
 void Win32ScraperPoster::compressRegionToFile(Region* region, int index, byte* lpvpxldata){
 struct jpeg_compress_struct cinfo;
 struct jpeg_error_mgr jerr;
 cinfo.err = jpeg_std_error(&jerr);
 jpeg_create_compress(&cinfo);

 FILE * outfile;
 char szFile[512];
 sprintf(szFile, "c://scrapes//plugin_out_%d.jpg", index);
 if ((outfile = fopen(szFile, "wb")) == NULL) {
 fprintf(stderr, "can't open %s", szFile);
 exit(1);
 }
 jpeg_stdio_dest(&cinfo, outfile);

 cinfo.input_components = 3;
 cinfo.in_color_space = JCS_RGB;

 cinfo.image_width = region->getWidth();
 cinfo.image_height = region->getHeight();

 jpeg_set_defaults(&cinfo);
 jpeg_start_compress(&cinfo, TRUE);

 JSAMPROW row_pointer[1];

 while (cinfo.next_scanline < cinfo.image_height) {
 row_pointer[0] = region->getScanline(cinfo.next_scanline, lpvpxldata);
 jpeg_write_scanlines(&cinfo, row_pointer, 1);
 }

 jpeg_finish_compress(&cinfo);
 fclose(outfile);
 jpeg_destroy_compress(&cinfo);
 }
 */
void Win32ScraperPoster::expireDisplay() {
    this->displayChanged = TRUE;
}
bool Win32ScraperPoster::displayExpired() {
    bool res = this->displayChanged;
    this->displayChanged = FALSE;
    return res;
}
void Win32ScraperPoster::cleanUpHash(void) {
    //treeCleanup( root);
    //sentSet->clear();
    delete sentHashes;
    delete subtileMap;
}
void Win32ScraperPoster::sendScrapingStoppedMessage() {
    //char postUrl[4096];
    DWORD responseLength = 0;
    //const char* servlet = "stoppedShowing";
    //buildUrlFromMetaData(pScraperPosterMetaData, postUrl, servlet);
    std::string postUrl = pScraperPosterMetaData->buildPath("stoppedShowing");
    Poster stopPoster(pScraperPosterMetaData->getHost(),
                      pScraperPosterMetaData->getPort(), postUrl,
                      pScraperPosterMetaData->getSsl());
    const char* data = "bogodata";
    char response[4096];
    BOOL result = stopPoster.sendPost((BYTE*) data, 8, (BYTE*) response, 4096,
                                      &responseLength);
    s_trace(postUrl.c_str());
    if (result) {
        s_trace("stoppedShowing sent successfully");
    } else {
        s_trace("error posting stoppedShowing");
    }
}
