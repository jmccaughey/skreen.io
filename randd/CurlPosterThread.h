#include "CurlPoster.h"
#include "PostJob.h"
#include <deque>
#include <pthread.h>

class CurlPosterThread{
  public:
    CurlPosterThread(std::deque<PostJob> * jobs, pthread_mutex_t *jobs_mutex, 
    						pthread_cond_t* jobs_cv, char* url) 
    						: jobs(jobs), jobs_mutex(jobs_mutex), jobs_cv(jobs_cv){
    	this->stopFlag = 0;
    	working = 0;
    	cp = new CurlPoster(url);
    } 
    void start(){
    	pthread_create(&pthread, 0, &CurlPosterThread::thread_function, (void*)this);
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
    ~CurlPosterThread(){
    	printf("CurlPosterThread dtor\n");
    	delete cp;
    }
    int isWorking(){
    	return working;
    }
  
  private:
  	std::deque<PostJob> * jobs;
  	pthread_t pthread;
	pthread_mutex_t* jobs_mutex;
	pthread_cond_t* jobs_cv;
	int stopFlag;
	int working;
	CurlPoster* cp;
  	void threadLoop(){
  		printf("thread loop %d\n", pthread);
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
        reinterpret_cast<CurlPosterThread *>(obj)->threadLoop();
    }

};