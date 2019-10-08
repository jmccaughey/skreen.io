#pragma once

#include <windows.h>
#include <wininet.h>
#include <string>

#define BACKUP_BUFFER_LENGTH 256

class HttpConnection
{
public:

	HttpConnection(std::string host, unsigned int port, bool ssl){
		hSession = NULL;
		hConnect = NULL;
		this->host = host;
		this->port = port;
		this->ssl = ssl;
	}

	~HttpConnection(){
		//std::cout << "in HttpConnection dtor" << std::endl;
		if (hConnect){
			InternetCloseHandle(hConnect);
		}
		if (hSession){
			InternetCloseHandle(hConnect);
		}
	}

	std::string getHost(){
		return this->host;
	}

	unsigned int doGetRequest(std::string path, unsigned char* responseBuffer, unsigned int bufferLength){
		if (!hSession){
			hSession = InternetOpenA("skreen.io client", INTERNET_OPEN_TYPE_PRECONFIG,
				NULL, NULL, 0);
			if (!hSession){
				return 0;
			}
		}
		if (!hConnect){
			hConnect = InternetConnectA(hSession, host.c_str(), port, 
				NULL, NULL, INTERNET_SERVICE_HTTP, NULL, NULL);
			if (!hConnect){
				return 0;
			}
		}
		HINTERNET hRequest = HttpOpenRequestA(hConnect, "GET", path.c_str(), NULL, NULL, NULL,
			INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE, 0);
		if (!hRequest){
			return 0;
		}
		BOOL result = HttpSendRequest(hRequest, "Connection: close", -1, NULL, 0);
		if (!result){
			return 0;
		}
		DWORD bytesRead = 0;
		DWORD totalBytesRead = 0;
		unsigned char backupBuffer[BACKUP_BUFFER_LENGTH];
		bool bufferTooSmall = false;
		unsigned char* writeCursor = responseBuffer;
		unsigned int writeRoom = bufferLength;
		while (InternetReadFile(hRequest, writeCursor, writeRoom, &bytesRead)
			&& bytesRead > 0){
			if (!bufferTooSmall){
				totalBytesRead += bytesRead;
				writeCursor += bytesRead;
				writeRoom -= bytesRead;
				if (totalBytesRead >= bufferLength){
					//std::cout << "buffer too small, discarding rest" << std::endl;
					bufferTooSmall = true;
					writeCursor = backupBuffer;
					writeRoom = BACKUP_BUFFER_LENGTH;
				}
			}else{
				//std::cout << "buffer too small, still discarding" << std::endl;
				totalBytesRead += bytesRead;
			}
		}
		if (hRequest){
			InternetCloseHandle(hRequest);
		}
		return totalBytesRead;
	}

private:
	HINTERNET hSession;
	HINTERNET hConnect;
	std::string host;
	unsigned int port;
	bool ssl;
};

