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

#include <string.h>
#include <stdio.h>
#include "X11plugin.h" 

//////////////////////////////////////
//
// general initialization and shutdown
//
NPError NS_PluginInitialize()
{
  trace(LOG_NAME, "NS_PluginInitialize\n");
  return NPERR_NO_ERROR;
}

void NS_PluginShutdown(){
	trace(LOG_NAME, "NS_PluginShutdown\n");
}

/////////////////////////////////////////////////////////////
//
// construction and destruction of our plugin instance object
//
nsPluginInstanceBase * NS_NewPluginInstance(nsPluginCreateData * aCreateDataStruct)
{
  trace(LOG_NAME, "NS_NewPluginInstance\n");
  if(!aCreateDataStruct)
    return NULL;
  // https://developer.mozilla.org/en/Gecko_Plugin_API_Reference/Plug-in_Basics#Using_the_embed_Element_for_Plug-in_Display	
  nsX11PluginInstance * plugin = new nsX11PluginInstance(aCreateDataStruct->instance);
  plugin->initParms(aCreateDataStruct->argc, aCreateDataStruct->argn, aCreateDataStruct->argv);
  return plugin;
}

void NS_DestroyPluginInstance(nsPluginInstanceBase * aPlugin)
{
    trace(LOG_NAME, "NS_DestroyPluginInstance\n");
	if(aPlugin){		
		delete (nsX11PluginInstance *)aPlugin;
                aPlugin = NULL;
	}
}

////////////////////////////////////////
//
// nsX11PluginInstance class implementation
//
nsX11PluginInstance::nsX11PluginInstance(NPP aInstance) : nsPluginInstanceBase(),
  mInstance(aInstance),
  mInitialized(FALSE)
{
  trace(LOG_NAME, "nsX11PluginInstance::nsX11PluginInstance\n");
  //mhWnd = NULL;
  scraperPosterMetaData = NULL;
  win32ScraperPoster = NULL;
  mShutting = FALSE;
  scraperThread = 0;
  exposed = 0;
}

nsX11PluginInstance::~nsX11PluginInstance(){
  trace(LOG_NAME, "nsX11PluginInstance::~nsX11PluginInstance\n");
}

NPError nsX11PluginInstance::GetValue(NPPVariable variable, void *value){
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

static void *scrapeThreadFunction(void *context){
  trace(LOG_NAME, "entering ScraperThreadFunction.\n");
  nsX11PluginInstance * pluginInstance = (nsX11PluginInstance*) context;
  trace(LOG_NAME, "ScraperThreadFunction (cast context complete)\n");
  while (true) {  
  	if(pluginInstance->exposed == 0){
  		trace(LOG_NAME, "ScraperThreadFunction (waiting for dialog exposed)\n");
  		nanosleep((struct timespec[]){{0, 100 * 1000000}}, NULL);
  	}else{
  		break;
  	}
  }  
  trace(LOG_NAME, "ScraperThreadFunction (exposed == true) starting loop\n");
  while(pluginInstance->shouldContinue()){
  	//trace(LOG_NAME, "ScraperThreadFunction (in loop, pluginInstance->shouldContinue returned true)\n");
    //trace(LOG_NAME, "ScraperThreadFunction (in loop, about to scrape)\n");
    pluginInstance->doScreenScrape();
    trace(LOG_NAME, "ScraperThreadFunction (in loop, returned from scrape)\n");
  }
  trace(LOG_NAME, "exiting ScraperThreadFunction.\n");
  pthread_exit(NULL);
  return 0;
}

void nsX11PluginInstance::initParms(int count, char** argn, char** argv){
 trace(LOG_NAME, "nsX11PluginInstance::initParms\n"); 
	const char* blastId = NULL;
	const char* blasterId = NULL;
	bool ssl = false;
	const char* host = "localhost";
	const char* port = "80";
	const char* key = NULL; 
    const char* context = "";	
	char  contextPart[512];
	contextPart[0] = 0;
  for(int i = 0; i < count; i++){
  		trace(LOG_NAME, "nsX11PluginInstance::initParms reading parms\n");
  		trace(LOG_NAME, argn[i]);
  		trace(LOG_NAME, ": ");
  		trace(LOG_NAME, argv[i]);
  		trace(LOG_NAME, "\n");
  		// NOTE: chrome and firefox seem to 
  		// convert attribute names to lower-case
		if(strlen(argv[i]) < 100 && strlen(argn[i]) < 100 && strlen(argv[i]) > 0){
			if(strcmp("blastid", argn[i]) == 0){
				blastId = argv[i];
			}else if(strcmp("blasterid", argn[i]) == 0){
				blasterId = argv[i];
			}else if(strcmp("key", argn[i]) == 0){
				key = argv[i];
			}else if(strcmp("host", argn[i]) == 0){
				host = argv[i];
			}else if(strcmp("port", argn[i]) == 0){
				port = argv[i];
			}else if(strcmp("context", argn[i]) == 0){
                context = argv[i];
            }else if(strcmp("ssl", argn[i]) == 0){
                ssl = (strcmp("true", argv[i]) == 0);
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
		trace(LOG_NAME, "nsX11PluginInstance::initParms(constructed ScraperPosterMetaData)\n"); 
		this->win32ScraperPoster = new X11ScraperPoster(
			this->scraperPosterMetaData
		);
		trace(LOG_NAME, "nsX11PluginInstance::initParms(constructed X11ScraperPoster)\n");			
		if(!scraperThread){
        	trace(LOG_NAME, "initParms (starting thread)\n");
        	// TODO: create process instead of living in the plugin
        	// improvement over Webex's "DO NOT CLOSE THIS WINDOW..."
        	int rc = pthread_create(&scraperThread, NULL, scrapeThreadFunction, this);
        	if(rc){
        		printf("ERROR; return code from pthread_create() is %d\n", rc);
        	}else{
        		trace(LOG_NAME, "initParms (thread started)\n");
        	}
    	}
	}else{
		trace(LOG_NAME, "required parm(s) missing\n");
	}  
}

static void exposeEventHandler(GtkWidget *widget, GdkEvent* event,
                     gpointer   data ){
    trace(LOG_NAME, "exposeEventHandler handler\n");    
    nsX11PluginInstance * pluginInstance = (nsX11PluginInstance*) data;
    pluginInstance->exposed = 1;
    //gtk_window_set_keep_above ((GtkWindow*)widget, 1);
    trace(LOG_NAME, "exposeEventHandler handler (exposed set to true)\n");
    // TODO: start scraping thread here, where we are
    // sure the informative window is showing
    //nsX11PluginInstance * pluginInstance = (nsX11PluginInstance*) data;
    //pluginInstance->doScreenScrape();
    /*
    if(!pluginInstance->scraperThread){
        trace(LOG_NAME, "exposeEventHandler handler (starting thread)\n");
        pthread_create(&(pluginInstance->scraperThread), NULL, scrapeThreadFunction, data);
        trace(LOG_NAME, "exposeEventHandler handler (thread started)\n");
    }
    */
}

static void destroyWindow( GtkWidget *widget,
                     gpointer   data ){
    trace(LOG_NAME, "destroyWindow handler\n");
    // TODO: set flag for scraping thread to exit
    nsX11PluginInstance* instance = (nsX11PluginInstance*)data;
    char buff[512];
    sprintf(buff, "destroyWindow pluginInstance: %p\n", instance);
    trace(LOG_NAME, buff);
    instance->mShutting = true;
    trace(LOG_NAME, "destroyWindow handler (set shutting to TRUE)\n");
    gtk_widget_destroy(widget);
}

static void handleStopButton(GtkWidget *widget,
        gpointer   data ){
    trace(LOG_NAME, "handleStopButton\n");
    printf("handleStopButton\n");
    gtk_widget_destroy(widget);
}

static gboolean delete_event( GtkWidget *widget,
                              GdkEvent  *event,
                              gpointer   data ){
    trace(LOG_NAME, "delete_event handler\n");
    // TODO: set flag for scraping thread to exit
    // TODO: this does not get called when the browser closes
    // or navigates away from the plugin host page
    return false;
}

static gboolean on_button_press (GtkWidget* widget,
  GdkEventButton * event, GdkWindowEdge edge){
  printf("on_button_press\n");
  if (event->type == GDK_BUTTON_PRESS) {
    if (event->button == 1) {
      gtk_window_begin_move_drag(GTK_WINDOW(gtk_widget_get_toplevel(widget)),
          event->button,
      event->x_root,
      event->y_root,
      event->time);
    }
  }
  return FALSE;
}

static void* runWindow(void *context){
    trace(LOG_NAME, "runWindow thread\n");
    GtkWidget *window;
    GtkWidget *button;
    GtkWidget *label;
    GtkWidget *frame;
    GtkWidget *hbox;

    nsX11PluginInstance * pluginInstance = (nsX11PluginInstance*)context;
    char buff[512];
    sprintf(buff, "runWindow pluginInstance: %p\n", pluginInstance);
    trace(LOG_NAME, buff);

      gtk_init(NULL, NULL);
	  hbox = gtk_hbox_new(FALSE, 2);
	  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
      button = gtk_button_new_with_label ("stop");
      label = gtk_label_new ("showing");
      frame = gtk_frame_new(NULL);
	  //gtk_container_add (GTK_CONTAINER (window), button);
	  //gtk_window_set_decorated((GtkWindow*)window, false);
	  gtk_window_set_title((GtkWindow*)window, "shodat");
      gtk_frame_set_shadow_type((GtkFrame*)frame, GTK_SHADOW_OUT);
      gtk_container_add (GTK_CONTAINER (hbox), label);
      gtk_container_add(GTK_CONTAINER(hbox), button);
      gtk_widget_show (label);
      gtk_container_add (GTK_CONTAINER (frame), hbox);
      gtk_container_add (GTK_CONTAINER (window), frame);
      gtk_widget_show (button);
      gtk_widget_show (hbox);
      gtk_widget_show (frame);
      gtk_window_set_decorated((GtkWindow*)window, 0);
      gtk_window_set_title((GtkWindow*)window, "shodat");
      gtk_window_set_modal((GtkWindow*)window, 1);
      g_signal_connect_swapped(button, "clicked",
                    G_CALLBACK (gtk_widget_destroy), window);
      g_signal_connect (window, "delete-event",
                    G_CALLBACK (delete_event), pluginInstance);
      g_signal_connect (window, "destroy",
                    G_CALLBACK (destroyWindow), pluginInstance);
      gtk_widget_add_events(window, GDK_BUTTON_PRESS_MASK);
      g_signal_connect(G_OBJECT(window), "button-press-event",
            G_CALLBACK(on_button_press), NULL);
      g_signal_connect(window, "expose_event",
                    G_CALLBACK (exposeEventHandler), (gpointer)pluginInstance);
      gtk_window_set_keep_above ((GtkWindow*)window, 1);
      gtk_widget_show(window);
      gtk_window_set_resizable((GtkWindow*)window, 0);
      gtk_container_set_border_width (GTK_CONTAINER (window), 1);
      gtk_window_move((GtkWindow*)window, 500, 0);
      gtk_window_set_keep_above ((GtkWindow*)window, 1);
      return NULL;
}

bool nsX11PluginInstance::shouldContinue(){
	if(mShutting){
		trace(LOG_NAME, "shouldContinue mShutting: TRUE\n");
	}else{
		trace(LOG_NAME, "shouldContinue mShutting: FALSE\n");
	}
	if(win32ScraperPoster->mContinueScraping){
		trace(LOG_NAME, "shouldContinue win32ScraperPoster->mContinueScraping: TRUE\n");
	}else{
		trace(LOG_NAME, "shouldContinue win32ScraperPoster->mContinueScraping: FALSE\n");
	}
	if(win32ScraperPoster 
	        // TODO: uncomment next line,
		//&& win32ScraperPoster->mContinueScraping
		&& mShutting == false
		){
		return true;
	}
	return false;
}

void nsX11PluginInstance::doScreenScrape(){
	win32ScraperPoster->mContinueScraping = false;
    win32ScraperPoster->doScreenScrape();
}

NPBool nsX11PluginInstance::init(NPWindow* aWindow){
  trace(LOG_NAME, "init\n");
  if(aWindow == NULL){
    return FALSE;
  }
  runWindow((void*)this);
  mInitialized = TRUE;
  return TRUE;
}

void nsX11PluginInstance::shut(){
  trace(LOG_NAME, "nsX11PluginInstance::shut\n");
  mInitialized = FALSE;
}

NPBool nsX11PluginInstance::isInitialized(){
  trace(LOG_NAME, "nsX11PluginInstance::isInitialized\n");
  if(mInitialized){
  	trace(LOG_NAME, "nsX11PluginInstance::isInitialized returning true\n");
  }else{
  	trace(LOG_NAME, "nsX11PluginInstance::isInitialized returning false\n"); 
  }
  return mInitialized;
}

const char * nsX11PluginInstance::getVersion(){
    trace(LOG_NAME, "nsX11PluginInstance::getVersion");
	return NPN_UserAgent(mInstance);
}

NPError nsX11PluginInstance::NewStream(NPMIMEType type, NPStream* stream, 
				  NPBool seekable, uint16_t* stype){
    trace(LOG_NAME, "nsX11PluginInstance::NewStream");
	return NPERR_NO_ERROR;
}
NPError nsX11PluginInstance::DestroyStream(NPStream *stream, NPError reason){ 
  trace(LOG_NAME, "nsX11PluginInstance::DestroyStream");
  return NPERR_NO_ERROR; 
}
int32_t nsX11PluginInstance::WriteReady(NPStream *stream){
    trace(LOG_NAME, "nsX11PluginInstance::WriteReady");
	return 4096;
}
