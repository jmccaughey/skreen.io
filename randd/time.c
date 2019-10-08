#include <time.h>
#include <sys/time.h>
#include "stdio.h"
#include <unistd.h>


int main(int argc, char** argv){

	timeval tv;
	long start, end;
	gettimeofday(&tv, 0);
	start = (tv.tv_sec * 1000)  +  (tv.tv_usec / 1000);
	//usleep(1000*100);
	//nanosleep(1000*100*1000);
	nanosleep((struct timespec[]){{0, 100*1000000}}, NULL);
	gettimeofday(&tv, 0);
	end = (tv.tv_sec * 1000)  +  (tv.tv_usec / 1000);
	long diff = end - start;
	printf("elapsed: %ld\n", diff);
}
