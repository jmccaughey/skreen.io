/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) 1998 - 2012, Daniel Stenberg, <daniel@haxx.se>, et al.
 *
 * This software is licensed as described in the file COPYING, which
 * you should have received as part of this distribution. The terms
 * are also available at http://curl.haxx.se/docs/copyright.html.
 *
 * You may opt to use, copy, modify, merge, publish, distribute and/or sell
 * copies of the Software, and permit persons to whom the Software is
 * furnished to do so, under the terms of the COPYING file.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ***************************************************************************/
/* An example source code that issues a HTTP POST and we provide the actual
 * data through a read callback.
 */
#include <stdio.h>
#include <string.h>
#include <curl/curl.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <stdint.h>
#include <pthread.h>
#include <stdlib.h>
#include <deque>

//#define NUM_THREADS     4

pthread_mutex_t deque_mutex;
pthread_cond_t deque_cv;
int stopFlag = 0;
std::deque<struct threadInfo*> dq;
char * url;

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

struct threadInfo{
    int length;
    char* url;
    char* data;
};

void * threadFunc(void * threadId){
  CURL *curl;
  CURLcode res;
  curl = curl_easy_init();
  if(!curl){
    fprintf(stderr, "curl_easy_init() failed: %s\n",  
      curl_easy_strerror(res));
    return NULL;
  }
  while(stopFlag == 0){
  	printf("stopFlag is %d %d\n", stopFlag, threadId);
    pthread_mutex_lock(&deque_mutex);
    if(dq.empty()){
      //stop = true;
      pthread_cond_wait(&deque_cv, &deque_mutex);
      printf("returned from wait %d\n", threadId);
      pthread_mutex_unlock(&deque_mutex);
    }else{
      struct threadInfo* ti = dq.front();
      printf("got job %d\n", threadId);
      dq.pop_front();
      printf("returned from pop %d\n", threadId);
      pthread_mutex_unlock(&deque_mutex);
      printf("returned from unlock %d\n", threadId);
      curl_easy_setopt(curl, CURLOPT_URL, url);
      curl_easy_setopt(curl, CURLOPT_POST, 1L);
      curl_easy_setopt(curl, CURLOPT_POSTFIELDS, ti->data);
      curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE,
                     (curl_off_t)ti->length);
      //curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
      curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (curl_off_t)ti->length);
      //curl_easy_setopt(curl, CURLOPT_TCP_NODELAY, 1L);
      {
        struct curl_slist *chunk = NULL;
        chunk = curl_slist_append(chunk, "Expect:");
        res = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
        /* use curl_slist_free_all() after the *perform() call to free this
         list again */
      }
      printf("about to curl_easy_perform %d\n", threadId);
      res = curl_easy_perform(curl);
	  printf("returned from curl_easy_perform %d\n", threadId);
      if(res != CURLE_OK)
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
              curl_easy_strerror(res));
    }
  }
  printf("exited loop %d\n", threadId);
  curl_easy_cleanup(curl);
}

int main(int argc, char **argv)
{

  char *file;
  //char *url;
  int res;
  FILE * hd_src ;
  int hd ;
  struct stat file_info;
  struct threadInfo info;
  //timeval tp;
  //clock_t start, end;
  uint32_t         start, stop;

  if(argc < 4)
    return 1;

  file= argv[1];
  url = argv[2];
  int threadCount = ::atoi(argv[3]);
  int uploadCount = ::atoi(argv[4]);
  info.url = argv[2];

  hd = open(file, O_RDONLY) ;
  if(!hd){
  	printf("could not read %s\n", argv[1]);
  	return 1; 
  }else{
  	printf("opened file\n");
  }
  fstat(hd, &file_info);
  close(hd) ;
  info.length = file_info.st_size;
  hd_src = fopen(file, "rb");

  char * buffer;
  buffer = (char*)malloc(file_info.st_size);
  info.data = buffer;
  fread(buffer, 1, file_info.st_size, hd_src);

  pthread_t threads[threadCount];

  res = curl_global_init(CURL_GLOBAL_DEFAULT);
  /* Check for errors */
  if(res != CURLE_OK) {
    fprintf(stderr, "curl_global_init() failed: %s\n",
            curl_easy_strerror((CURLcode)res));
    return 1;
  }

  pthread_mutex_init(&deque_mutex, NULL);
  pthread_cond_init (&deque_cv, NULL);

  start = stampstart();

  // start the threads
  int rc;
  long t;
  for(t=0; t<threadCount; t++){
      printf("In main: creating thread %ld\n", t);
      rc = pthread_create(&threads[t], NULL, threadFunc, (void *)t);
      if (rc){
         printf("ERROR; return code from pthread_create() is %d\n", rc);
         exit(-1);
      }
  }
  
  for(int i = 0; i < uploadCount; i++){
  	pthread_mutex_lock(&deque_mutex);
    dq.push_back(&info);
    pthread_cond_broadcast(&deque_cv);
    pthread_mutex_unlock(&deque_mutex);
  }
  getchar();
  printf("returned from getchar\n");
  pthread_mutex_lock(&deque_mutex);
  stopFlag = 1;
  pthread_cond_broadcast(&deque_cv);
  pthread_mutex_unlock(&deque_mutex);
  
  void *status;
  for(t=0; t<threadCount; t++) {
      rc = pthread_join(threads[t], &status);
      if (rc) {
         printf("ERROR; return code from pthread_join() is %d\n", rc);
         exit(-1);
      }
      printf("Main: completed join with thread %ld having a status of %ld\n",t,(long)status);
  }

  stop = stampstop(start);
  printf("%4.2fkbps\n", (float)(uploadCount*info.length*8)/(double)(stop-start));
  free(buffer);
  curl_global_cleanup();
  pthread_mutex_destroy(&deque_mutex);
  pthread_cond_destroy(&deque_cv);
  pthread_exit(NULL);
}
