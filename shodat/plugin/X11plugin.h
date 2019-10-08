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

#ifndef __X11PLUGIN_H__
#define __X11PLUGIN_H__

#include <iostream>
#include <pthread.h>
#include <gtk/gtk.h>

#ifndef FALSE
#define FALSE (0)
#endif
#ifndef TRUE
#define TRUE (!FALSE)
#endif
#ifndef NULL
#define NULL (FALSE)
#endif

#include "Trace.h"
#include "pluginbase.h"
#include "../windows-scrapers-common2/X11ScraperPoster.h"

char* NS_GetMIMEDescription(){
  trace(LOG_NAME, "NS_GetMIMEDescription\n");
  return (char*)"application/x-shodat-plugin::show your screen over the internet;";
}

class nsX11PluginInstance : public nsPluginInstanceBase
{
public:

  nsX11PluginInstance(NPP aInstance);
  ~nsX11PluginInstance();

  NPBool init(NPWindow* aWindow);
  void shut();  
  NPBool isInitialized();
  void initParms(int count, char** argn, char** argv);
  NPError NewStream(NPMIMEType type, NPStream* stream, 
                            NPBool seekable, uint16_t* stype);
  int32_t   WriteReady(NPStream *stream);       
  NPError DestroyStream(NPStream *stream, NPError reason);
  // locals
  const char * getVersion();
  NPError GetValue(NPPVariable variable, void *value);
  //HWND mhWnd;
  bool mShutting;
  pthread_t scraperThread;
  bool shouldContinue();
  void doScreenScrape();
  int exposed;
//  static void runWindow(void *context);
private:
  NPP mInstance;
  NPBool mInitialized;
  X11ScraperPoster* win32ScraperPoster;
  ScraperPosterMetaData* scraperPosterMetaData;  
};

#endif // __PLUGIN_H__
