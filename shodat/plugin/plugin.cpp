/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is mozilla.org code.
 *
 * The Initial Developer of the Original Code is
 * Netscape Communications Corporation.
 * Portions created by the Initial Developer are Copyright (C) 1998
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */


#include <windows.h>
#include <windowsx.h>

#include "plugin.h"
#include "Trace.h"

//static const char* LOG_NAME = "\\deskblast_plugin.log";
static const char* MODULE_NAME = "npshodat"; 

//////////////////////////////////////
//
// general initialization and shutdown
//
NPError NS_PluginInitialize()
{
  trace(LOG_NAME, "NS_PluginInitialize");
  return NPERR_NO_ERROR;
}

void NS_PluginShutdown(){
	trace(LOG_NAME, "NS_PluginShutdown");
}

/////////////////////////////////////////////////////////////
//
// construction and destruction of our plugin instance object
//
nsPluginInstanceBase * NS_NewPluginInstance(nsPluginCreateData * aCreateDataStruct)
{
  trace(LOG_NAME, "NS_NewPluginInstance");
  if(!aCreateDataStruct)
    return NULL;
  // https://developer.mozilla.org/en/Gecko_Plugin_API_Reference/Plug-in_Basics#Using_the_embed_Element_for_Plug-in_Display	
  nsPluginInstance * plugin = new nsPluginInstance(aCreateDataStruct->instance);
  plugin->initParms(aCreateDataStruct->argc, aCreateDataStruct->argn, aCreateDataStruct->argv);
  return plugin;
}

void NS_DestroyPluginInstance(nsPluginInstanceBase * aPlugin)
{
    trace(LOG_NAME, "NS_DestroyPluginInstance");
	if(aPlugin){		
		delete (nsPluginInstance *)aPlugin;
                aPlugin = NULL;
	}
}

////////////////////////////////////////
//
// nsPluginInstance class implementation
//
nsPluginInstance::nsPluginInstance(NPP aInstance) : nsPluginInstanceBase(),
  mInstance(aInstance),
  mInitialized(FALSE)
{
  trace(LOG_NAME, "nsPluginInstance::nsPluginInstance");
  mhWnd = NULL;
  scraperPosterMetaData = NULL;
  win32ScraperPoster = NULL;
  mShutting = FALSE;
}

nsPluginInstance::~nsPluginInstance(){
  trace(LOG_NAME, "nsPluginInstance::~nsPluginInstance");
}

static LRESULT CALLBACK PluginWinProc(HWND, UINT, WPARAM, LPARAM);
static WNDPROC lpOldProc = NULL;

void nsPluginInstance::initParms(int count, char** argn, char** argv){
 trace(LOG_NAME, "nsPluginInstance::initParms"); 
	char* blastId = NULL;
	char* blasterId = NULL;
	bool ssl = false;
	const char* host = "localhost";
	const char* port = "8080";
	char* key = NULL; 
    const char* context = "";
	char  contextPart[512];
	contextPart[0] = 0;
  for(int i = 0; i < count; i++){
		if(strlen(argv[i]) < 50 && strlen(argn[i]) < 20 ){
			if(strcmpi("blastId", argn[i]) == 0){
				blastId = argv[i];
			}else if(strcmpi("blasterId", argn[i]) == 0){
				blasterId = argv[i];
			}else if(strcmpi("key", argn[i]) == 0){
				key = argv[i];
			}else if(strcmpi("host", argn[i]) == 0){
				host = argv[i];
			}else if(strcmpi("port", argn[i]) == 0){
				port = argv[i];
			}else if(strcmpi("context", argn[i]) == 0){
                context = argv[i];
            }else if(strcmpi("ssl", argn[i]) == 0){
                ssl = (strcmpi("true", argv[i]) == 0);
            }
		}
	}
	if(blastId && blasterId && key){
		if(strlen(context)){
			sprintf(contextPart, "%s/", context);
		}
		this->scraperPosterMetaData = new ScraperPosterMetaData(
			host,
			atoi(port),
			contextPart,
			ssl,
			blastId,
			blasterId,
			key	
		);
		HMODULE hModule = GetModuleHandle(MODULE_NAME);
		if(hModule){
			this->win32ScraperPoster = new Win32ScraperPoster(
				this->scraperPosterMetaData,
				MODULE_NAME	
			);
		}
	}else{
		trace(LOG_NAME, "required parm(s) missing");
	}  
}
NPBool nsPluginInstance::init(NPWindow* aWindow){
  trace(LOG_NAME, "init");
  if(aWindow == NULL)
    return FALSE;

  mhWnd = (HWND)aWindow->window;
  if(mhWnd == NULL)
    return FALSE;

  // subclass window so we can intercept window messages and
  // do our drawing to it
  lpOldProc = SubclassWindow(mhWnd, (WNDPROC)PluginWinProc);
  trace(LOG_NAME, "returned from SubclassWindow");
  // associate window with our nsPluginInstance object so we can access 
  // it in the window procedure
  SetWindowLong(mhWnd, GWL_USERDATA, (LONG)this); 
  trace(LOG_NAME, "returned from SetWindowLong"); 
  if(this->win32ScraperPoster){
  	trace(LOG_NAME, "this->win32ScraperPoster is non null");
  	this->win32ScraperPoster->resumeScraping(NULL, NULL);
  	mInitialized = TRUE;
  }else{
  	trace(LOG_NAME, "this->win32ScraperPoster is null");
	//return FALSE; crashes if you return false here
  }  
  return TRUE;
}

void nsPluginInstance::shut(){
  // subclass it back
  trace(LOG_NAME, "nsPluginInstance::shut");
  SubclassWindow(mhWnd, lpOldProc);
  mhWnd = NULL;
  mInitialized = FALSE;
	if(this->win32ScraperPoster){
  		delete this->win32ScraperPoster;
  		this->win32ScraperPoster = NULL;
  	}
  	if(this->scraperPosterMetaData){
  		delete this->scraperPosterMetaData;
        this->scraperPosterMetaData = NULL;
  	}
}

NPBool nsPluginInstance::isInitialized(){
  trace(LOG_NAME, "nsPluginInstance::isInitialized");
  if(mInitialized){
  	trace(LOG_NAME, "nsPluginInstance::isInitialized returning true");
  }else{
  	trace(LOG_NAME, "nsPluginInstance::isInitialized returning false"); 
  }
  return mInitialized;
}

const char * nsPluginInstance::getVersion(){
    trace(LOG_NAME, "nsPluginInstance::getVersion");
	return NPN_UserAgent(mInstance);
}

NPError nsPluginInstance::NewStream(NPMIMEType type, NPStream* stream, 
				  NPBool seekable, uint16_t* stype){
    trace(LOG_NAME, "nsPluginInstance::NewStream");
	return NPERR_NO_ERROR;
}
NPError nsPluginInstance::DestroyStream(NPStream *stream, NPError reason){ 
  trace(LOG_NAME, "nsPluginInstance::DestroyStream");
  return NPERR_NO_ERROR; 
}
int32_t nsPluginInstance::WriteReady(NPStream *stream){
    trace(LOG_NAME, "nsPluginInstance::WriteReady");
	return 4096;
}

NPError nsPluginInstance::GetValue(NPPVariable variable, void *value){
    trace(LOG_NAME, "nsX11PluginInstance::GetValue\n");
    switch(variable) {
        case NPPVpluginNameString: {
          trace(LOG_NAME, "nsX11PluginInstance::GetValue(NPPVpluginNameString)\n");
          *((char **)value) = (char *)"shodat screen showing";
          break;
        }
        case NPPVpluginDescriptionString: {
              trace(LOG_NAME, "nsX11PluginInstance::GetValue(NPPVpluginDescriptionString)\n");
          *((char **)value) = (char *)"Plugin to allow showing your screen over the internet";
          break;
        }
        default: {
            trace(LOG_NAME, "nsX11PluginInstance::GetValue(NPERR_INVALID_PARAM)\n");
          return NPERR_INVALID_PARAM;
        }
  }
  return NPERR_NO_ERROR;
}

/*
int32 nsPluginInstance::Write(NPStream *stream, int32 offset, 
							  int32 len, void *buffer){
    char* bytes = (char*)buffer;
	char command = bytes[0];
	this->ticksLastCommand = GetTickCount();
	fprintf(this->mlogfile, "got data %d length %d", command, len);
	fflush(this->mlogfile);
	this->mContinueScraping = (command == 1 || command == 2);
	this->mSendKeyframe = (command == 2);
	SetEvent(this->mScrapeControlEvent);
	return len;
}
*/

static LRESULT CALLBACK PluginWinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

  trace(LOG_NAME, "winproc");
  switch (msg) {
  	/*
    case WM_DISPLAYCHANGE:
	{
	    nsPluginInstance *plugin = (nsPluginInstance *)GetWindowLong(hWnd, GWL_USERDATA);
	    if (plugin) {
		plugin->expireDisplay();
	    }
	}
	break;
	*/	    
    case WM_PAINT:
      {
	  trace(LOG_NAME, "winproc: WM_PAINT");
        break;
        // draw a frame and display the string
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        RECT rc;
        GetClientRect(hWnd, &rc);
        FrameRect(hdc, &rc, GetStockBrush(BLACK_BRUSH));

        // get our plugin instance object and ask it for the version string
        nsPluginInstance *plugin = (nsPluginInstance *)GetWindowLong(hWnd, GWL_USERDATA);
        if (plugin) {
          const char * string = plugin->getVersion();
          DrawTextA(hdc, string, lstrlenA(string), &rc, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
        }
        else {
          char string[] = "Error occured";
          DrawTextA(hdc, string, lstrlenA(string), &rc, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
        }

        EndPaint(hWnd, &ps);
      }
      break;
    default:
      char buff[1024];
      sprintf(buff, "winproc other WM: %d", msg);
        trace(LOG_NAME, buff);
      break;
  }
  return DefWindowProc(hWnd, msg, wParam, lParam);
}
