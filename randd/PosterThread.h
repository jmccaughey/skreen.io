#include "../windows-scrapers-common/Poster.h"
#include <deque>
#include <pthread.h>

class PosterThread{
  public:
    PosterThread(unsigned char * data, int dataLength, char * host, int port, char * path){
		responseMaxLength = 1024 * 1024;
		response = malloc(responseMaxLength);
    	poster = new Poster(host, port, path, 0);
    } 
    void start(){
    	//pthread_create(&pthread, 0, &CurlPosterThread::thread_function, (void*)this);
    	CreateThread(NULL, NULL, &PosterThread::thread_function, this, 0, NULL);
    }
    void stop(){
    	stopFlag = 1;
    }
    void * join(){
    	void * status;
    	int rc = pthread_join(pthread, &status);
		if (rc) {
        	printf("ERROR; return code from pthread_join() is %d\n", rc);
        }
        return status;
    }
    ~PosterThread(){
    	printf("PosterThread dtor\n");
    	delete response;
    	delete cp;
    }
    int isWorking(){
    	return working;
    }
  
  private:
    unsigned char* data;
    int dataLength;
    unsigned char* response;
    int responseMaxLength;
    int responseLength;
  	pthread_t pthread;
	pthread_mutex_t* jobs_mutex;
	pthread_cond_t* jobs_cv;
	int stopFlag;
	int working;
	Poster* poster;
  	void threadLoop(){
  		printf("thread loop\n");
  		while(!stopFlag){
  			//PostJob  postJob = NULL;
  			pthread_mutex_lock(jobs_mutex);
  			if(!jobs->empty()){
  				PostJob postJob = jobs->back();
  				jobs->pop_back();
  				working = 1;
  				pthread_mutex_unlock(jobs_mutex);
  				cp->sendPost(postJob.data, postJob.length, NULL, NULL, NULL);  				
  				pthread_mutex_lock(jobs_mutex);
  				working = 0;  				
  				pthread_cond_broadcast(jobs_cv);
  				pthread_mutex_unlock(jobs_mutex);
  			}else{  				
  				pthread_cond_wait(jobs_cv, jobs_mutex);
  				pthread_mutex_unlock(jobs_mutex);
  			}
  		
  		}
  		printf("exiting thread loop %d\n", pthread);
  	}
  
  	static void * thread_function(void *obj){
        reinterpret_cast<PosterThread *>(obj)->threadLoop();
    }

};