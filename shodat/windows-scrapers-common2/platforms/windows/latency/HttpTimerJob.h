#pragma once

#include "HttpConnection.h"

#define BUFFER_LENGTH 512

class HttpTimerJob
{
public:

	HttpTimerJob(std::string host, unsigned int port, std::string path, bool ssl){
		durationInMillis = 0;
		responseLength = 0;
		this->pHttpConnection = new HttpConnection(host, port, ssl);
		this->path = path;
	}

	static void run(void * context){
		static_cast<HttpTimerJob*>(context)->doJob();
	}

	unsigned int getDurationInMillis(){
		return this->durationInMillis;
	}

	unsigned int getResponseLength(){
		return this->responseLength;
	}

	std::string getHost(){
		return this->pHttpConnection->getHost();
	}

	void doJob(){
		// do request twice to remove DNS variability
		responseLength = this->pHttpConnection->doGetRequest(path, buffer, BUFFER_LENGTH);
		// start timer
		LARGE_INTEGER start, finish, elapsedMicroseconds;
		LARGE_INTEGER frequency;
		QueryPerformanceFrequency(&frequency);
		QueryPerformanceCounter(&start);
		// do request again
		this->pHttpConnection->doGetRequest(path, buffer, BUFFER_LENGTH);
		// stop timer
		QueryPerformanceCounter(&finish);
		elapsedMicroseconds.QuadPart = finish.QuadPart - start.QuadPart;
		elapsedMicroseconds.QuadPart *= 1000000;
		elapsedMicroseconds.QuadPart /= frequency.QuadPart;
		durationInMillis = elapsedMicroseconds.QuadPart / 1000;
		//std::cout << "response length: " << responseLength << std::endl;
	}

	~HttpTimerJob(){
		//std::cout << "in HttpTimerJob dtor" << std::endl;
		delete pHttpConnection;
	}
private:
	unsigned int durationInMillis;
	unsigned int responseLength;
	std::string path;
	unsigned char buffer[BUFFER_LENGTH];
	HttpConnection* pHttpConnection;
};

