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
#pragma once
// MS' C++ compiler will warn because all ancestors of WVMActiveX inherit
// from the same base class, but this is on purpose here and can't
// be avoided due to Microsofts dual-interface design
#pragma warning(disable:4584) 

#include "Win32ScraperPoster.h"
#include "activex_config.h"

#include "GuidUtil.h"


class IEEntry :
	//public IUnknown,
	public IDispatch,
	public IClassFactory,
	public IObjectWithSite,
	public IObjectSafety,
	public IPersistPropertyBag
{
private:
	IUnknown *site;
	ULONG     ref;
	HWND      msgHwnd;
	//void	doScreenScrape();
	Win32ScraperPoster *win32ScraperPoster;
	ScraperPosterMetaData *scraperPosterMetaData;
	IDispatch* mDoneScrapingDispatch;	
public:
	IEEntry();
	~IEEntry();

	static HRESULT Callback(IDispatch *disp);
	static void statusFunc(LPVOID context, const char* status);
	void handleStatus(const char* status);
public:
	//////////////////////////////////////////////////////////////////
	// INTERNAL USE ONLY

	//////////////////////////////////////////////////////////////////
	// IUnknown
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void __RPC_FAR *__RPC_FAR *ppvObject);

	ULONG STDMETHODCALLTYPE AddRef(void);

	ULONG STDMETHODCALLTYPE Release(void);
	//////////////////////////////////////////////////////////////////
	// IDispatch
	HRESULT STDMETHODCALLTYPE GetTypeInfoCount(UINT *pctinfo);

	HRESULT STDMETHODCALLTYPE GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo);

	HRESULT STDMETHODCALLTYPE GetIDsOfNames(REFIID riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId);

	HRESULT STDMETHODCALLTYPE Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr);

	//////////////////////////////////////////////////////////////////
	// IClassFactory
	HRESULT STDMETHODCALLTYPE CreateInstance(IUnknown *pUnkOuter, REFIID riid, void **ppvObject);

	HRESULT STDMETHODCALLTYPE LockServer(BOOL fLock);

	//////////////////////////////////////////////////////////////////
	// IObjectWithSite
	HRESULT STDMETHODCALLTYPE SetSite(IUnknown *pUnkSite);

	HRESULT STDMETHODCALLTYPE GetSite(REFIID riid, void **ppvSite);
	
	HRESULT STDMETHODCALLTYPE doAlert();

	//////////////////////////////////////////////////////////////////
	// IObjectSafety
	HRESULT STDMETHODCALLTYPE GetInterfaceSafetyOptions(REFIID riid, DWORD *pdwSupportedOptions, DWORD *pdwEnabledOptions);

	HRESULT STDMETHODCALLTYPE SetInterfaceSafetyOptions(REFIID riid, DWORD dwOptionSetMask, DWORD dwEnabledOptions);
		
	// IPersistPropertyBag
	HRESULT STDMETHODCALLTYPE InitNew();
	HRESULT STDMETHODCALLTYPE Load(IPropertyBag* pPropertyBag, IErrorLog* pErrorLog);
	HRESULT STDMETHODCALLTYPE Save(IPropertyBag* pPropertyBag, BOOL bool1, BOOL bool2);
	HRESULT STDMETHODCALLTYPE GetClassID(CLSID* pclsid); 
};
