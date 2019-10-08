#include "CurlPosterThread.h"
#include <stdio.h>
#include "FileReader.h"


int main(int argc, char** argv){

    if(argc != 4){
		printf("usage: %s <filename> <url> <upload times>\n", argv[0]);
		return 1; 
	}
  FileReader fr(argv[1]);
  printf("%s is %d bytes long\n", argv[1], fr.getLength());

  int times = atoi(argv[3]);

  std::deque<PostJob> jobs;
  pthread_t pthread;
  pthread_mutex_t jobs_mutex;
  pthread_cond_t jobs_cv;

  pthread_mutex_init(&jobs_mutex, NULL);
  pthread_cond_init (&jobs_cv, NULL);

  PostJob pj(fr.getData(), fr.getLength());
  for(int i = 0; i < times; i++){
  	jobs.push_back(pj);
  }

  CurlPosterThread cpt(&jobs, &jobs_mutex, &jobs_cv, argv[2]);
  cpt.start();
  getchar();


}