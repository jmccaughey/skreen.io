#include "ParallelPoster.h"
//#include "PostJob.h"


ParallelPoster::ParallelPoster(char* url, int threadCount) {
	// TODO: validate input
	if(strlen(url) < 4096){
		strcpy(this->url, url);
	}else{
		this->url[0] = 0;
	}
	CURLcode res = curl_global_init(CURL_GLOBAL_DEFAULT);
  	if(res != CURLE_OK) {
    	fprintf(stderr, "curl_global_init() failed: %s\n",
            curl_easy_strerror((CURLcode)res));
            return;    	
  	}
  	this->threadCount = threadCount;
  	pthread_mutex_init(&jobs_mutex, NULL);
    pthread_cond_init (&jobs_cv, NULL);
    jobs = new std::deque<PostJob>; 
  	posterThreads = new CurlPosterThread* [threadCount];
  	for(int i = 0; i < threadCount; i++){
		posterThreads[i] = new CurlPosterThread(jobs, &jobs_mutex, &jobs_cv, this->url); 		
  		posterThreads[i]->start();
  	}
}
int ParallelPoster::sendPost(int itemCount, char ** datas, int * lengths, char ** responses,
		int responseBufferLength, int** responseLengths) {
	pthread_mutex_lock(&jobs_mutex);
	for(int i = 0; i < itemCount; i++){
	    PostJob pj(datas[i], lengths[i]);
		jobs->push_back(pj);
	}
	pthread_cond_broadcast(&jobs_cv);
	pthread_mutex_unlock(&jobs_mutex);
	// wait for all threads to be idle before returning
	int busy = 1;
	while(busy == 1){
		pthread_mutex_lock(&jobs_mutex);
		printf("waiting for empty deque and no busies\n");
		pthread_cond_wait(&jobs_cv, &jobs_mutex);
		if(jobs->empty()){
			busy = 0;
		    printf("empty deque, checking busies\n"); 
			for(int i = 0; i < threadCount; i++){
				if(posterThreads[i]->isWorking()){
				  busy = 1;	
				  printf("busy found\n");
				}
			}
		} 
		pthread_mutex_unlock(&jobs_mutex);
	}	
	printf("returning from sendPost\n");
	return 0;
}
void ParallelPoster::interrupt() {
	cleanup();
}
void ParallelPoster::cleanup(void) {
	printf("ParallelPoster::cleanup\n");
}
ParallelPoster::~ParallelPoster(void) {
	cleanup();
	delete jobs;
	void *status;
	int rc;
	for(int i = 0; i < threadCount; i++){
		pthread_mutex_lock(&jobs_mutex);
		posterThreads[i]->stop();
		pthread_mutex_unlock(&jobs_mutex);
	}
	pthread_mutex_lock(&jobs_mutex);
	pthread_cond_broadcast(&jobs_cv);
	pthread_mutex_unlock(&jobs_mutex);
	for(int i = 0; i < threadCount; i++){
		posterThreads[i]->join();
		delete posterThreads[i];
	}
	delete[] posterThreads;
	curl_global_cleanup();
}
