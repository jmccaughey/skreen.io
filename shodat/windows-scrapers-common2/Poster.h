#pragma once

#include <windows.h>
#include <wininet.h>
#include <string>
#define BUFFSIZE 500

class Poster {
public:
	Poster(std::string host, int port, std::string path, BOOL ssl);
	BOOL sendPost(unsigned char * data, DWORD length, BYTE * response,
			DWORD responseBufferLength, DWORD* responseLength);
	void interrupt();
	~Poster(void);
	bool interrupted;
private:
	BOOL UseHttpSendReq(HINTERNET hRequest, BYTE* data, DWORD dwPostSize);
	BOOL StatusSuccessful(HINTERNET hRequest);
	void cleanup(void);
	BOOLEAN ssl;
	char host[2048];
	int port;
	char path[2048];
	HINTERNET hSession;
	HINTERNET hConnect;
	HINTERNET hRequest;
};
