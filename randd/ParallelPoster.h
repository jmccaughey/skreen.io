#include <pthread.h>
#include "CurlPosterThread.h"


class ParallelPoster {
public:
	ParallelPoster(char* url, int threadCount);
	int sendPost(int itemCount, char ** datum, int* lengths, char ** responses,
			int responseBufferLength, int** responseLengths);
	void interrupt();
	~ParallelPoster(void);
private:
    char url[4096];
	void cleanup(void);
	CurlPosterThread** posterThreads;
	int threadCount;
	std::deque<PostJob>* jobs;
    pthread_mutex_t jobs_mutex;
    pthread_cond_t jobs_cv;
	
};
