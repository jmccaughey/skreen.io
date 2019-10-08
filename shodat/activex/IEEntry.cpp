/**
 * Copyright 2007-2008 Aplix Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * Contact: dev@webvm.net
 */
#include "IEEntry.h"
//#include "Registry.h"
#include "Trace.h"


extern "C" {
//#include "IEEntry.h"
#define _BASETSD_H_
#include "jpeglib.h"
}

#include "Registry.h"

static HINSTANCE this_instance;
static IEEntry *afx = NULL;
static int OutstandingObjects = 0;
static int LockCount = 0;

static HMODULE g_hModule = NULL ;   // DLL module handle

const char LOG[] = "\\deskblast.log";
const char MODULE_NAME[] = "shodat";


//const CATID CLSID_SimpleActiveX = {0xCF36AE54,0x27B0,0x445D,{0x80,0x49,0x13,0x1A,0x1D,0x47,0xBF,0xC6}};
const CATID CLSID_SimpleActiveX = {0xCF11AE46,0x27B0,0x475D,{0x81,0x87,0x13,0x3D,0x1D,0x47,0xBF,0xA4}};
const WCHAR kGet42[]            = L"get42";
const WCHAR kResumeScraping[]	= L"resumeScraping";
const WCHAR kStopScraping[]	= L"stopScraping";
const WCHAR kCallback[]         = L"callback";
const WCHAR kThrowException[]   = L"throwException";
const DISPID ID_GET42           = 42;
const DISPID ID_CALLBACK        = 43;
const DISPID ID_THROWEXCEPTION  = 44;
const DISPID ID_RESUMESCRAPING	= 45;
const DISPID ID_STOPSCRAPING	= 46;

void trace(const char *msg) {
	trace(LOG, msg);
}
UINT BSTR2CSTR(BSTR source, char* target, int length){
	if(!source || !target){
		return 0;
	}
	UINT cchar = SysStringLen(source);
	UINT cbyte = SysStringByteLen(source);
	if(cchar < 1 || (cchar*2) != cbyte || cbyte + 2 > length){
		return 0;
	}
	return WideCharToMultiByte(CP_UTF8, NULL, (LPCWSTR)source, 
		-1//SysStringLen(bstrSource)
		, target, length, NULL, NULL);
}
int /* case insensitive wstrcmp */
ciwstrcmp(const WCHAR *s1, const WCHAR *s2) {
	int c1, c2;

	while(*s1){
		c1 = *(WCHAR*)s1++;
		c2 = *(WCHAR*)s2++;
		if(c1 == c2) {
			continue;
		}
		if(c1 >= L'A' && c1 <= L'Z') {
			c1 -= L'A' - L'a';
		}
		if(c2 >= L'A' && c2 <= L'Z') {
			c2 -= L'A' - L'a';
		}
		if(c1 != c2) {
			return c1 - c2;
		}
	}
	return -*s2;
}

typedef struct {
	HWND       win;
	IDispatch *disp;
} ThreadData;

typedef __RPC_FAR IObjectWithSite *LPOBJECTWITHSITE;
typedef __RPC_FAR IObjectSafety *LPOBJECTSAFETY;
//typedef __RPC_FAR IPersistPropertyBag *LPPERSISTPROPERTYBAG;

static DWORD WINAPI
asyncCall(LPVOID data) {
	ThreadData *t = (ThreadData *)data;

	Sleep(3000);
	trace("asyncCall -> PostMessage");
#ifdef WINCE
	PostMessage(t->win, WM_FLUSH_EVENTS, 0, (LPARAM)t->disp);
#else
	IEEntry::Callback(t->disp);
#endif
	free(t);
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
// IUnknown
ULONG IEEntry::AddRef(void) {
	trace("IEEntry::AddRef");
	return ++ref;
}

ULONG IEEntry::Release(void) {
	trace("IEEntry::Release");
	if(ref == 1){
		trace("refs are 0");
		if(afx == this){
			trace("not deleting factory");
			//afx == NULL;
		}else{
			delete(this);
			InterlockedDecrement((long int*)&OutstandingObjects);
			return 0;	
		}
	}
	return ref > 0 ? --ref : 0;
}

HRESULT IEEntry::QueryInterface(REFIID riid, void __RPC_FAR *__RPC_FAR *ppvObject)
{
	*ppvObject = NULL;

	if(riid == IID_IUnknown) {
		trace("IEEntry::QueryInterface - IUnknown");
		*ppvObject = (LPDISPATCH)this;
		((LPUNKNOWN)*ppvObject)->AddRef();
		return S_OK;
	}
	else if(riid == IID_IDispatch) {
		trace("IEEntry::QueryInterface - IDispatch");
		*ppvObject = (LPDISPATCH)this;
		((LPUNKNOWN)*ppvObject)->AddRef();
		return S_OK;
	}
	else if(riid == IID_IClassFactory) {
		trace("IEEntry::QueryInterface - IClassFactory");
		*ppvObject = (LPCLASSFACTORY)afx;//this;
		((LPUNKNOWN)*ppvObject)->AddRef();
		return S_OK;
	}
	else if(riid == IID_IObjectWithSite) {
		trace("IEEntry::QueryInterface - IObjectWithSite");
		*ppvObject = (LPOBJECTWITHSITE)this;
		((LPUNKNOWN)*ppvObject)->AddRef();
		return S_OK;
	}
	else if(riid == IID_IObjectSafety) {
		trace("IEEntry::QueryInterface - IObjectSafety");
		*ppvObject = (LPOBJECTSAFETY)this;
		((LPUNKNOWN)*ppvObject)->AddRef();
		return S_OK;
	}
	else if(riid == IID_IPersistPropertyBag) {
                trace("IEEntry::QueryInterface - IPersistPropertyBag");
                *ppvObject = (LPPERSISTPROPERTYBAG)this;
                ((LPUNKNOWN)*ppvObject)->AddRef();
                return S_OK;
        }

	trace("IEEntry::QueryInterface - unknown interface query:");
	char inter[1024];
	GuidUtil::CLSIDtochar(riid,
            inter,
            1024) ;	
	trace(inter);
	return E_NOINTERFACE;
}

///////////////////////////////////////////////////////////////////////////////
// IDispatch

HRESULT IEEntry::GetTypeInfoCount(UINT *pctinfo)
{
	trace("IEEntry::GetTypeInfoCount()");
	return E_NOTIMPL;
}

HRESULT IEEntry::GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo)
{
	trace("IEEntry::GetTypeInfo()");
	return E_NOTIMPL;
}

HRESULT IEEntry::GetIDsOfNames(REFIID riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId)
{
	trace("IEEntry::GetIDsOfNames()");
	if(!ciwstrcmp(rgszNames[0], kCallback)) {
		rgDispId[0] = ID_CALLBACK;
		return S_OK;
	}
	else if(!ciwstrcmp(rgszNames[0], kGet42)) {
		rgDispId[0] = ID_GET42;
		return S_OK;
	}
	else if(!ciwstrcmp(rgszNames[0], kThrowException)) {
		rgDispId[0] = ID_THROWEXCEPTION;
		return S_OK;
	}else if(!ciwstrcmp(rgszNames[0], kResumeScraping)){
		rgDispId[0] = ID_RESUMESCRAPING;
		return S_OK;
	}else if(!ciwstrcmp(rgszNames[0], kStopScraping)){
        	rgDispId[0] = ID_STOPSCRAPING;
	        return S_OK;
	}
	return DISP_E_MEMBERNOTFOUND;
}

void IEEntry::statusFunc(LPVOID context, const char* status){
        trace(status);
	// this is invoked when scraping stops, due to user cancellation
	// or network error or from browser shutdown or navigation
	// in other words all cases when scraping stops
	IEEntry *ieentryInstance = (IEEntry*)context;
	ieentryInstance->handleStatus(status);
}

void IEEntry::handleStatus(const char* status){
	trace("handling status");
	if(mDoneScrapingDispatch){
		// build variant arg with status string
		// invoke invoke on IDispatch
		trace("trying to invoke callback");
       		DISPPARAMS params;
		VARIANT dummy;
		HRESULT r;

		params.cArgs             = 0;
       		params.rgvarg            = 0;
        	params.cNamedArgs        = 0;
        	params.rgdispidNamedArgs = 0;

      		switch((r = mDoneScrapingDispatch->Invoke(0, IID_NULL, 0, 	
					DISPATCH_METHOD, &params, &dummy, NULL, NULL))) {
      			case S_OK:
              			trace("IEEntry::Callback succeeded");
              			break;
      			case E_UNEXPECTED:
              			trace("IEEntry::Callback error - unexpected");
              			break;
      			default:
              			trace("IEEntry::Callback error - other");
              			break;
      		}
		mDoneScrapingDispatch->Release();
		mDoneScrapingDispatch = NULL;
	}
}

HRESULT IEEntry::Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr)
{
	trace("IEEntry::Invoke()");
	if(wFlags & DISPATCH_METHOD) {
		switch(dispIdMember) {
		case ID_CALLBACK:
			if(pDispParams->rgvarg[0].vt == VT_DISPATCH) {
				DWORD tid;
				int result = MessageBox(
							GetForegroundWindow()	
						//NULL
						, 	
						"Create Thread", "Create Thead", MB_OKCANCEL);
				if(result == IDOK){
					ThreadData *t = (ThreadData *)malloc(sizeof(ThreadData));
					t->disp = pDispParams->rgvarg[0].pdispVal;
					// remove next line? msgHwnd is not set anywhere	
					t->win = msgHwnd;
					t->disp->AddRef();
					CreateThread(NULL, 0, asyncCall, t, 0, &tid);
					pVarResult->vt     = VT_INT;
					pVarResult->intVal = 0;
				}	
				return S_OK;
			}
			break;
		case ID_GET42:
			pVarResult->vt = VT_INT;
			pVarResult->intVal = 666;
			//doScreenScrape();
			return S_OK;
			break;
		case ID_THROWEXCEPTION:
			pExcepInfo->wCode = 666;
			pExcepInfo->bstrSource = SysAllocString(L"SimpleActiveX");
			pExcepInfo->bstrDescription = SysAllocString(L"SimpleActiveX throwed an exception");
			pExcepInfo->dwHelpContext = 0;
			pExcepInfo->pvReserved = NULL;
			pExcepInfo->pfnDeferredFillIn = NULL;
			pExcepInfo->scode = 0;
			return DISP_E_EXCEPTION;
			break;
		case ID_RESUMESCRAPING:
			trace("IEEntry::Invoke (resumeScraping)");
			if(win32ScraperPoster){
				//doAlert();
				// TODO: store pdisp as above, to be invoked on callback
				// from win32scraperPoster
				if(pDispParams->rgvarg[0].vt == VT_DISPATCH) {
					mDoneScrapingDispatch = pDispParams->rgvarg[0].pdispVal;
					mDoneScrapingDispatch->AddRef();
					trace("set mDoneScrapingDispatch");
					if(pVarResult){
						pVarResult->vt = VT_INT;
						pVarResult->intVal = 0;
					}
				}else{
					mDoneScrapingDispatch = NULL;
				}
				win32ScraperPoster->resumeScraping(IEEntry::statusFunc, this);	
				return S_OK;
			}else{
				trace("IEEntry::Invoke returning E_FAIL");
				return E_FAIL;
			}
			break;
		case ID_STOPSCRAPING:
			trace("IEEntry::Invoke (stopScraping)");
			if(win32ScraperPoster){
				win32ScraperPoster->stopScraping();
				return S_OK;
			}else{
				return E_FAIL;
			}
			break;	
		}
		return DISP_E_MEMBERNOTFOUND;
	}
	else if(wFlags & DISPATCH_PROPERTYGET) {
		// TODO: return property
		return DISP_E_MEMBERNOTFOUND;
	}
	else if(wFlags & DISPATCH_PROPERTYPUT) {
		// TODO set property
		return DISP_E_MEMBERNOTFOUND;
	}
	else {
		return DISP_E_MEMBERNOTFOUND;
	}

	// Exception occured
	// return DISP_E_EXCEPTION;
	return S_OK;
}
///////////////////////////////////////////////////////////////////////////////
// IClassFactory
HRESULT IEEntry::CreateInstance(IUnknown *pUnkOuter, REFIID riid, void **ppvObject)
{
	trace("IEEntry::CreateInstance");
	*ppvObject = NULL;
	if(pUnkOuter != NULL && riid != IID_IUnknown) {
		return CLASS_E_NOAGGREGATION;
	}
	else {
		IEEntry* pieentry = new IEEntry();
		InterlockedIncrement((long int*)&OutstandingObjects);
		return pieentry->QueryInterface(riid, ppvObject);
		//return this->QueryInterface(riid, ppvObject);
	}
}

HRESULT IEEntry::LockServer(BOOL fLock)
{
	trace("IEEntry::LockServer");
	if(fLock){
		InterlockedIncrement((long int*)&LockCount);
	}else{ 	
		InterlockedDecrement((long int*)&LockCount);
	}
	return S_OK;
}

///////////////////////////////////////////////////////////////////////////////
// IObjectWithSite

HRESULT IEEntry::SetSite(IUnknown *pUnkSite)
{
	trace("IEEntry::SetSite");
	if(!pUnkSite) {
		trace("site is null");
		site->Release();
		site = pUnkSite;
	}else{
		trace("site is non-null");
		site = pUnkSite;
		site->AddRef();

		IServiceProvider* pISP;
		HRESULT hr = pUnkSite->QueryInterface(IID_IServiceProvider, (void **)&pISP);
		if(SUCCEEDED(hr)){
			IWebBrowser2* pBrowser;
      			hr = pISP->QueryService(IID_IWebBrowserApp, 
					IID_IWebBrowser2, (void**)&pBrowser);
	      		if (SUCCEEDED(hr)) {
			  BSTR pbstr;
       			  pBrowser->get_LocationURL(&pbstr);
			  char url[512];
			  BSTR2CSTR(pbstr, url, 512);
			  SysFreeString(pbstr);
			  trace(url);

			  IDispatch* pDoc = NULL;
			  hr = pBrowser->get_Document(&pDoc);
			  if(SUCCEEDED(hr)){
				trace("get_Document success");
				IHTMLDocument2* pHtmlDoc = NULL;
			  	hr = pDoc->QueryInterface(IID_IHTMLDocument2, (void **)&pHtmlDoc);	
				if(SUCCEEDED(hr)){
					trace("query interface success IHTMLDocument2");
					IHTMLWindow2* pWindow = NULL;
					hr = pHtmlDoc->get_parentWindow(&pWindow);	
					if(SUCCEEDED(hr)){
						trace("success get_parentWindow");
						VARIANT varValue;
						VariantInit(&varValue);
						VariantClear(&varValue);
						hr = pWindow->execScript(L"alert(\'SetSite\');", L"jscript", &varValue);
						pWindow->Release();	
					}else{
						trace("failed: get_parentWindow");
						char result[128];
						sprintf(result, "%d", hr);
						trace(result);
					}
					pHtmlDoc->Release();	
				}
				pDoc->Release();
			  }
			  pBrowser->Release();
      			}
			pISP->Release();
		}

		if(win32ScraperPoster){
			win32ScraperPoster->resumeScraping(IEEntry::statusFunc, this);
		}else{
			return E_FAIL;
		}
	}
	return S_OK;
}

HRESULT IEEntry::GetSite(REFIID riid, void **ppvSite)
{
	trace("IEEntry::GetSite");
	*ppvSite = NULL;
	return E_FAIL;
}

HRESULT IEEntry::doAlert(){
	if(!site) return S_OK;
	        IServiceProvider* pISP;
                HRESULT hr = site->QueryInterface(IID_IServiceProvider, (void **)&pISP);
                if(SUCCEEDED(hr)){
                        IWebBrowser2* pBrowser;
                        hr = pISP->QueryService(IID_IWebBrowserApp,
                                        IID_IWebBrowser2, (void**)&pBrowser);
                        if (SUCCEEDED(hr)) {
                          BSTR pbstr;
                          pBrowser->get_LocationURL(&pbstr);
                          char url[512];
                          BSTR2CSTR(pbstr, url, 512);
                          SysFreeString(pbstr);
                          trace(url);

                          IDispatch* pDoc = NULL;
                          hr = pBrowser->get_Document(&pDoc);
                          if(SUCCEEDED(hr)){
                                trace("get_Document success");
                                IHTMLDocument* pHtmlDoc = NULL;
                                hr = pDoc->QueryInterface(IID_IHTMLDocument, (void **)&pHtmlDoc);
                                if(SUCCEEDED(hr)){
                                        trace("query interface success IHTMLDocument");
                                        IHTMLWindow2* pWindow = NULL;
					IDispatch* pScript = NULL;
                                        hr = pHtmlDoc->get_Script(&pScript);
                                        if(SUCCEEDED(hr)){
                                                trace("success get_Script");
						//pWindow = pScript;
                                                //VARIANT varValue;
                                                //VariantInit(&varValue);
                                                //VariantClear(&varValue);
                                                //hr = pWindow->execScript(L"alert(\'SetSite\');", L"jscript", &varValue);
                                                //pWindow->Release();
						pScript->Release();
                                        }else{
                                                trace("failed: get_Script");
                                                char result[128];
                                                sprintf(result, "%d", hr);
                                                trace(result);
                                        }
                                        pHtmlDoc->Release();
                                }
                                pDoc->Release();
                          }
                          pBrowser->Release();
                        }
                        pISP->Release();
                }



}


///////////////////////////////////////////////////////////////////////////////
// IObjectSafety
//
// Note, this interface needs to be implemented that we are able to run
//       JScript functions through IDispatch.
//
HRESULT IEEntry::GetInterfaceSafetyOptions(REFIID riid, DWORD *pdwSupportedOptions, DWORD *pdwEnabledOptions)
{
	trace("IEEntry::GetetInterfaceSafetyOptions");
	*pdwSupportedOptions = *pdwEnabledOptions = INTERFACESAFE_FOR_UNTRUSTED_CALLER|INTERFACESAFE_FOR_UNTRUSTED_DATA;
	return S_OK;
}

HRESULT IEEntry::SetInterfaceSafetyOptions(REFIID riid, DWORD dwOptionSetMask, DWORD dwEnabledOptions)
{
	trace("IEEntry::SetInterfaceSafetyOptions");
	return S_OK;
}

// IPersistPropertyBag
HRESULT IEEntry::InitNew(){
		trace("IEEntry::InitNew");	
		return S_OK;
}
HRESULT IEEntry::Load(IPropertyBag* pPropertyBag, IErrorLog* pErrorLog){
		trace("IEEntry::Load");		
		char parmHost[1024];
		char parmPort[1024];
		char parmContext[1024];		
		char parmBlastId[1024];
		char parmBlasterId[1024];
		char parmKey[1024];
		VARIANT varParm;
		VariantInit(&varParm);
		varParm.vt = VT_BSTR;
		HRESULT hr = pPropertyBag->Read(L"blastId",&varParm, pErrorLog);	
		UINT stringResult;	
		if(SUCCEEDED(hr)){ 
			stringResult = BSTR2CSTR(varParm.bstrVal, parmBlastId, 1024);
			if(stringResult == 0) return E_FAIL;
		}else{
			trace("did not get param");
			return E_FAIL;
		}
		trace(parmBlastId); 	
		hr = pPropertyBag->Read(L"blasterId",&varParm, pErrorLog); 
		if(SUCCEEDED(hr)){
			stringResult = BSTR2CSTR(varParm.bstrVal, parmBlasterId, 1024);
			if(stringResult == 0) return E_FAIL;
		}else{
	                trace("did not get param");
                        return E_FAIL;
		}	
		hr = pPropertyBag->Read(L"key",&varParm, pErrorLog);
                if(SUCCEEDED(hr)){
			stringResult = BSTR2CSTR(varParm.bstrVal, parmKey, 1024);
			if(stringResult == 0) return E_FAIL;
		}else{
			trace("did not get param");
			return E_FAIL;
		}
		hr = pPropertyBag->Read(L"host",&varParm, pErrorLog); 
		if(SUCCEEDED(hr)){
			stringResult = BSTR2CSTR(varParm.bstrVal, parmHost, 1024);
			if(stringResult == 0) return E_FAIL;
		}else{
			trace("did not get param");
			return E_FAIL;
		}
		hr = pPropertyBag->Read(L"context",&varParm, pErrorLog); 
		if(!hr && varParm.vt == VT_BSTR && SysStringLen(varParm.bstrVal)){
			stringResult = BSTR2CSTR(varParm.bstrVal, parmContext, 1024);
			if(stringResult == 0) return E_FAIL;
		}else{
		        trace("did not get context param");
			parmContext[0] = 0;
		}
                hr = pPropertyBag->Read(L"port",&varParm, pErrorLog);
                if(SUCCEEDED(hr)){
			 stringResult = BSTR2CSTR(varParm.bstrVal, parmPort, 1024);
			if(stringResult == 0) return E_FAIL;
		}else{
			trace("did not get port param");
			return E_FAIL;
		}	
		int port = atoi(parmPort);
		if(port < 1) return E_FAIL;
		this->scraperPosterMetaData = new ScraperPosterMetaData(
			parmHost,
			port,
			parmContext,
			false,
			parmBlastId,
			parmBlasterId,
			parmKey	
		);
		this->win32ScraperPoster = new Win32ScraperPoster(
			this->scraperPosterMetaData,
			MODULE_NAME	
		);
		return S_OK;
	}
        HRESULT IEEntry::Save(IPropertyBag* pPropertyBag, BOOL bool1, BOOL bool2){
		trace("IEEntry::Save");
		return S_OK;
	}
	HRESULT IEEntry::GetClassID(CLSID* pclsid){
		trace("IEEntry::GetClassID");	
		return E_NOTIMPL;
	}
	 

///////////////////////////////////////////////////////////////////////////////
// IEEntry
//
/* static */ HRESULT IEEntry::Callback(IDispatch *disp) {
	DISPPARAMS params;
	VARIANT dummy;
	HRESULT r;

	params.cArgs             = 0;
	params.rgvarg            = 0;
	params.cNamedArgs        = 0;
	params.rgdispidNamedArgs = 0;

	switch((r = disp->Invoke(0, IID_NULL, 0, DISPATCH_METHOD, &params, &dummy, NULL, NULL))) {
	case S_OK:
		trace("IEEntry::Callback succeeded");
		break;
	case E_UNEXPECTED:
		trace("IEEntry::Callback error - unexpected");
		break;
	default:
		trace("IEEntry::Callback error - other");
		break;
	}
	disp->Release();
	return r;
}

IEEntry::IEEntry() {
	trace("IEEntry");
	ref     = 0;
	site    = 0;
	scraperPosterMetaData = NULL;
        win32ScraperPoster = NULL;
	mDoneScrapingDispatch = NULL;
}
IEEntry::~IEEntry() {
	DestroyWindow(msgHwnd);
	trace("~IEEntry");
	if(win32ScraperPoster){
        	trace("deleting Win32ScraperPoster");
        	delete(win32ScraperPoster);
		win32ScraperPoster = NULL;
	}
	if(scraperPosterMetaData){
		delete(scraperPosterMetaData);
		scraperPosterMetaData = NULL;	
	}
}

///////////////////////////////////////////////////////////////////////

extern "C" {

BOOL WINAPI DllMain(HANDLE instance, DWORD reason, LPVOID reserved) {
	BOOL result = TRUE;

	if(this_instance == NULL) {
		this_instance = (HINSTANCE)instance;
	}

	switch(reason) {
	case DLL_PROCESS_ATTACH:
		trace("DllMain -- DLL_PROCESS_ATTACH");
		 g_hModule = (HMODULE)instance;	
		DisableThreadLibraryCalls((HINSTANCE)instance);	
		break;
	case DLL_PROCESS_DETACH:
		trace("DllMain -- DLL_PROCESS_DETACH");
		CoUninitialize();
		break;
	case DLL_THREAD_ATTACH:
		trace("DllMain -- DLL_THREAD_ATTACH");
		break;
	case DLL_THREAD_DETACH:
		trace("DllMain -- DLL_THREAD_DETACH");
		break;
	default:
		trace("DllMain -- unknown reason");
		break;
	}

	return result;
}

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID *ppv) {
	trace("DllGetClassObject");
	*ppv = NULL;
	if(rclsid == CLSID_SimpleActiveX) {
		trace("DllGetClassObject is SimpleActiveX");
		if(!afx) {
			trace("newing IEEntry (factory)");	
			afx = new IEEntry();
			InterlockedIncrement((long int*)&OutstandingObjects);	
		}
		return afx->QueryInterface(riid, ppv);
	}
	else {
		return CLASS_E_CLASSNOTAVAILABLE;
	}
}

STDAPI DllCanUnloadNow(void) {
	trace("new new DllCanUnloadNow");
	if(OutstandingObjects > 0 || LockCount > 0){
		trace("returning S_FALSE");
		return S_FALSE;
	}else{
		trace("returning S_OK");
		return S_OK;
	}
}

STDAPI DllRegisterServer(void) {
	trace("DllRegisterServer compiled at "); trace(__TIME__); trace(" on "); trace(__DATE__); 
	return RegisterServer(g_hModule, CLSID_SimpleActiveX, "shodat ActiveX Plug-In",
		"shodat.activex",
		"shodat.activex.1.0");	
	//return S_OK;
}

STDAPI DllUnregisterServer(void) {
	trace("DllUnregisterServer");
	return UnregisterServer(CLSID_SimpleActiveX, "shodat.activex",
		"shodat.activex.1.0");
}

}
