#include <shlwapi.h>


const int CLSID_STRING_SIZE = 39 ;

// Convert a CLSID into a char string.
class GuidUtil
{
  public:
	static void CLSIDtochar(const CLSID& clsid,
                 char* szCLSID,
                 int length) ;
};
