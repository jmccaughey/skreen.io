#include <windows.h>
#include <Objsafe.h>

int main( int argc, char* argv[] )
{
  //REFFMTID id1 = &FMTID_UserDefinedProperties;
  //REFFMTID id2 = &FMTID_SummaryInformation;
  REFIID riid = &IID_IObjectSafety;
  return 0;
}