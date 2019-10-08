#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>
#include "ScraperPosterMetaData.h"

// /usr/bin/i586-mingw32msvc-g++ main.cpp ../../windows-scrapers-common2/ScraperPosterMetaData.cpp -I../../windows-scrapers-common2


int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow){

	ScraperPosterMetaData* scraperPosterMetaData = new ScraperPosterMetaData(
				"parmHost",
				80,
				"parmContext",
				false,
				"parmBlastId",
				"parmBlasterId",
				"parmKey"
			);
	MessageBox(NULL, "returned from new metadata obj",
            "Win32 Guided Tour",
            NULL);
//			this->win32ScraperPoster = new Win32ScraperPoster(
//				this->scraperPosterMetaData,
//				MODULE_NAME
//			);
	return 0;

}
