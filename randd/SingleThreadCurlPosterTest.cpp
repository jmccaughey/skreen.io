#include "CurlPoster.h"
//#include "PostJob.h"

#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <stdint.h>
//#include <pthread.h>
#include <chrono>
#include <thread>

// g++ -std=c++0x SingleThreadCurlPosterTest.cpp CurlPoster.cpp $(pkg-config --libs --cflags libcurl) -o curltest

int stampstart()
{
        struct timeval  tv;
        struct timezone tz;
        struct tm      *tm;
        uint32_t         start;
        gettimeofday(&tv, &tz);
        tm = localtime(&tv.tv_sec);
        printf("TIMESTAMP-START\t  %d:%02d:%02d:%d (~%d ms)\n", tm->tm_hour,
               tm->tm_min, tm->tm_sec, tv.tv_usec,
               tm->tm_hour * 3600 * 1000 + tm->tm_min * 60 * 1000 +
               tm->tm_sec * 1000 + tv.tv_usec / 1000);
        start = tm->tm_hour * 3600 * 1000 + tm->tm_min * 60 * 1000 +
                tm->tm_sec * 1000 + tv.tv_usec / 1000;
        return (start);
}

int stampstop(uint32_t start)
{
        struct timeval  tv;
        struct timezone tz;
        struct tm      *tm;
        uint32_t         stop;
        gettimeofday(&tv, &tz);
        tm = localtime(&tv.tv_sec);
        stop = tm->tm_hour * 3600 * 1000 + tm->tm_min * 60 * 1000 +
                tm->tm_sec * 1000 + tv.tv_usec / 1000;
        printf("TIMESTAMP-END\t  %d:%02d:%02d:%d (~%d ms) \n", tm->tm_hour,
               tm->tm_min, tm->tm_sec, tv.tv_usec,
               tm->tm_hour * 3600 * 1000 + tm->tm_min * 60 * 1000 +
               tm->tm_sec * 1000 + tv.tv_usec / 1000);

        printf("ELAPSED\t  %d ms\n", stop - start);
        return (stop);
}
/*
extern "C" {
	
  void* doPost(void * info){
    PostJob* pj = (PostJob*)info;
	CurlPoster* cp = pj->cp;
	cp->sendPost(pj->data, pj->length, NULL, 0, NULL);
	return NULL;
  }
	
}
*/

int main(int argc, char **argv){
  printf("%s got %d args\n", argv[0], argc); 
  if(argc < 4)
    return 1;
  char *file;
  int length, hd;
  FILE * hd_src ;
  struct stat file_info;
  uint32_t         start, stop;

  file= argv[1];
  CurlPoster cp;
  int uploadCount = ::atoi(argv[3]);
  //int threadCount = ::atoi(argv[3]);
  hd = open(file, O_RDONLY) ;
  if(!hd){
  	printf("could not read %s\n", argv[1]);
  	return 1; 
  }else{
  	printf("opened file\n");
  }
  fstat(hd, &file_info);
  close(hd) ;
  length = file_info.st_size;
  hd_src = fopen(file, "rb");
  char * buffer;
  buffer = (char*)malloc(file_info.st_size);
  fread(buffer, 1, file_info.st_size, hd_src);
  start = stampstart();
  char response[4096];
  
  //pthread_t threads[uploadCount];
  
  int rc;  
  int responseLength = 0;
  for(int i = 0; i < uploadCount; i++){    
    rc = cp.sendPost(argv[2], buffer, length, response, 4096, &responseLength);
    printf("got result %d response length %d\n", rc, responseLength);  
    printf("%d\n", response[0]);
    std::this_thread::sleep_for(std::chrono::milliseconds(10000));
    //PostJob* pj = new PostJob(cp, length, buffer);
	//printf("In main: creating thread %ld\n", i);
    //rc = pthread_create(&threads[i], NULL, doPost, (void *)pj);
    //if (rc){
    //     printf("ERROR; return code from pthread_create() is %d\n", rc);
    //     exit(-1);
    //}
  }
  /*
  void *status;
  for(int t=0; t<uploadCount; t++) {
      rc = pthread_join(threads[t], &status);
      if (rc) {
         printf("ERROR; return code from pthread_join() is %d\n", rc);
         exit(-1);
      }
      printf("Main: completed join with thread %ld having a status of %ld\n",t,(long)status);
  }
  */
  stop = stampstop(start);
  printf("%4.0fkbps\n", (float)(uploadCount*length*8)/(double)(stop-start));
  free(buffer);
}