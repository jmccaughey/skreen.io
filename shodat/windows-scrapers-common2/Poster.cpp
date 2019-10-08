#include "Poster.h"
#include <stdio.h>

void p_trace(const char *msg) {
	HANDLE hFile = CreateFile("skreen.io_poster.log", FILE_APPEND_DATA, 0,
			NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	DWORD dwBytesWritten = 0;
	WriteFile(hFile, msg, lstrlen(msg), &dwBytesWritten, NULL);
	WriteFile(hFile, "\r\n", 2, &dwBytesWritten, NULL);
	CloseHandle(hFile);
}

Poster::Poster(std::string host, int port, std::string path, BOOL ssl) {
	strcpy(this->host, host.c_str());
	this->port = port;
	strcpy(this->path, path.c_str());
	this->ssl = ssl;
	this->hConnect = NULL;
	this->hRequest = NULL;
	this->hSession = NULL;
	this->interrupted = false;
	p_trace(__DATE__);
	p_trace(__TIME__);
}
BOOL Poster::sendPost(unsigned char * data, DWORD length, BYTE * response,
		DWORD responseBufferLength, DWORD* responseLength) {
	
	DWORD start = GetTickCount();
	if (this->hSession == NULL || this->hConnect == NULL) {
		this->hSession = InternetOpenA("skreen.io",
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
			, this->path, NULL, NULL, NULL, INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE, 0);
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
		response[*responseLength] = NULL;
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
BOOL Poster::UseHttpSendReq(HINTERNET hRequest, BYTE* data, DWORD dwPostSize) {
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
	return StatusSuccessful(hRequest);
}
BOOL Poster::StatusSuccessful(HINTERNET hRequest){
	DWORD statusCode = 0;
	DWORD statusCodeSize = sizeof(statusCode);
	if(!HttpQueryInfo(hRequest, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER,
      (LPVOID)&statusCode, &statusCodeSize, NULL)){
      	p_trace("unable to get response code");
		return FALSE;
	}
	if(statusCode != 200){
		char logBuff[512];
		sprintf(logBuff, "response code: %d", statusCode);
		p_trace(logBuff);
		return false;
	}
	return true;
}
void Poster::interrupt() {
	interrupted = true;
	cleanup();
}
void Poster::cleanup(void) {
	if (!InternetCloseHandle(hRequest)) {
		p_trace("Failed to close Request handle\n");
	}
	hRequest = NULL;
	if (!InternetCloseHandle(hConnect)) {
		p_trace("Failed to close Connect handle\n");
	}
	hConnect = NULL;
	if (!InternetCloseHandle(hSession)) {
		p_trace("Failed to close Session handle\n");
	}
	hSession = NULL;
}

Poster::~Poster(void) {
	cleanup();
}
