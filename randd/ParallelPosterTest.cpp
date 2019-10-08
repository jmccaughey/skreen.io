#include <time.h>
#include <sys/time.h>
#include <stdint.h>

#include "ParallelPoster.h"
#include "FileReader.h"

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

int main(int argc, char** argv){

	if(argc != 5){
		printf("usage: %s <file> <url> <threads> <uploads>\n", argv[0]);
		return 1;
	}
    FileReader fr(argv[1]);
    if(!fr.getLength()){
    	return 1;
    }
	ParallelPoster pp(argv[2], atoi(argv[3]));
	int uploads = atoi(argv[4]);


	char* datas[uploads];
	int lengths[uploads];
	for(int i = 0; i < uploads; i++){
		datas[i] = fr.getData();
		lengths[i] = fr.getLength();
	}
    uint32_t         start, stop;
    start = stampstart();
	pp.sendPost(uploads, datas, lengths, NULL, NULL, NULL);
	stop = stampstop(start);
    printf("%4.fkbps\n", (float)(uploads*fr.getLength()*8)/(double)(stop-start));

	getchar();

}