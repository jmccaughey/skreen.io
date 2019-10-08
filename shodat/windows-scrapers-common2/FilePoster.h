#pragma once
#include "stdio.h"
#define BUFFSIZE 500

class FilePoster {
public:
	FilePoster(char* host, int port, char* path, bool ssl);
	bool sendPost(char * data, int length, char * response,
			int responseBufferLength, int* responseLength);
	~FilePoster(void);
private:
	char path[2048];
	FILE* fp;
	void cleanup(void);
};
