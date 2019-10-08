#include "GuidUtil.h"


void GuidUtil::CLSIDtochar(const CLSID& clsid,
                 char* szCLSID,
                 int length)
{
        //assert(length >= CLSID_STRING_SIZE) ;
        // Get CLSID
        LPOLESTR wszCLSID = NULL ;
        HRESULT hr = StringFromCLSID(clsid, &wszCLSID) ;
        //assert(SUCCEEDED(hr)) ;

        // Covert from wide characters to non-wide.
        wcstombs(szCLSID, wszCLSID, length) ;

        // Free memory.
        CoTaskMemFree(wszCLSID) ;
}
