#include "FilePoster.h"
#include <stdio.h>

void p_trace(const char *msg) {
	HANDLE hFile = CreateFile("deskblast_poster.log", FILE_APPEND_DATA, 0,
			NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	DWORD dwBytesWritten = 0;
	WriteFile(hFile, msg, lstrlen(msg), &dwBytesWritten, NULL);
	WriteFile(hFile, "\r\n", 2, &dwBytesWritten, NULL);
	CloseHandle(hFile);
}

FilePoster::FilePoster(char* host, int port, char* path, bool ssl) {
    fp = 0;
}
bool FilePoster::sendPost(char * data, int length, char * response,
		int responseBufferLength, int* responseLength) {
	printf("address data: %x\n", data);		
	printf("address response: %x\n", response);
	printf("data length: %d\n", length);
	if(fp == 0){
	   fp = fopen("posted.bin", "wb");
	}
	int written = fwrite(data, 1, length, fp);
	response[0] = 1;
	*responseLength = 1;
	return true;
}
/*

	if (this->hSession == NULL || this->hConnect == NULL) {
		this->hSession = InternetOpenA("deskblast.com",
				INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
		if (!hSession) {
			p_trace("Failed to open session\n");
			cleanup();
			return FALSE;
		}
		this->hConnect = InternetConnectA(hSession, this->host, this->port,
				NULL, NULL, INTERNET_SERVICE_HTTP,
				this->ssl ? INTERNET_FLAG_SECURE : NULL, NULL);
		if (!this->hConnect) {
			p_trace("Failed to connect\n");
			cleanup();
			return FALSE;
		}
	}
	this->hRequest = HttpOpenRequestA(hConnect, "POST"
	//"PUT"
			, this->path, NULL, NULL, NULL, INTERNET_FLAG_NO_CACHE_WRITE, 0);
	if (!hRequest) {
		p_trace("Failed to open request handle\n");
		cleanup();
		return FALSE;
	}
	if (UseHttpSendReq(hRequest, data, length)) {
		char pcBuffer[BUFFSIZE];
		DWORD dwBytesRead;
		*responseLength = 0;
		//p_trace("\nThe following was returned by the server:\n");
		do {
			dwBytesRead = 0;
			if (InternetReadFile(hRequest, pcBuffer, BUFFSIZE, &dwBytesRead)) {
				//pcBuffer[dwBytesRead]=0x00; // Null-terminate buffer
				//ptrace("%s", pcBuffer);
				if (*responseLength < responseBufferLength) {
					int writeAmount = 
						dwBytesRead < (responseBufferLength - (*responseLength))
						? dwBytesRead 
						: (responseBufferLength - (*responseLength));
					memccpy((void*) (response + (*responseLength)),
							(void*) pcBuffer, 1, writeAmount);
				}
				*responseLength = dwBytesRead + (*responseLength);
			} else {
				p_trace("\nInternetReadFile failed");
				cleanup();
				return FALSE;
			}
		} while (dwBytesRead > 0);
		*responseLength = (*responseLength) < responseBufferLength
			? (*responseLength) : responseBufferLength;
		InternetCloseHandle( hRequest);
		char timeBuff[512];
		sprintf(timeBuff, "upload: %d millis", GetTickCount() - start); 
		p_trace(timeBuff);
		return TRUE;
	} else {
		cleanup();
		return FALSE;
	}
}
BOOL FilePoster::UseHttpSendReq(HINTERNET hRequest, BYTE* data, DWORD dwPostSize) {
	// TODO: use a post method that blocks less (like returns after each packet
	// or so, with a callback) so that an in-progress post doesn't hold up
	// anything (such as 'stop showing')
	char logBuff[512];
	sprintf(logBuff, "posting %d bytes", dwPostSize);
	p_trace(logBuff);
	//p_trace("using HttpSendRequest\n");
	if (!HttpSendRequest(hRequest, NULL, 0, (void*) data, dwPostSize)) {
		p_trace("Error on HttpSendRequest \n");
		return FALSE;
	}
	return true;
}
void FilePoster::interrupt() {
	cleanup();
}
*/
void FilePoster::cleanup(void) {
    if(fp){
        fclose(fp);
    }
}

FilePoster::~FilePoster(void) {
	cleanup();
}
