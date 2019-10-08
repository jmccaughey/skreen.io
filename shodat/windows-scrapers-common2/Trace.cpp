#include "Trace.h"
#if defined(_WINDOWS)
#include <windows.h>
#else
#include "stdio.h"
#endif

void trace(const char* logName, const char* line){
#if defined(_WINDOWS)
        HANDLE hFile = CreateFile(logName,
                FILE_APPEND_DATA,
                FILE_SHARE_WRITE,
                NULL,
                OPEN_ALWAYS,
                FILE_ATTRIBUTE_NORMAL,
                NULL
        );
        DWORD dwBytesWritten = 0;
        WriteFile(hFile, line, lstrlen(line), &dwBytesWritten, NULL);
        WriteFile(hFile, "\r\n", 2, &dwBytesWritten, NULL);
        CloseHandle(hFile);
#else
        FILE * pFile = fopen (logName,"ab");
        if (pFile!=NULL){
          fputs (line, pFile);
          fclose (pFile);
        }
#endif
}
