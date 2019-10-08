#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

class FileReader{

public:

  FileReader(char* path){
    buffer = NULL;
  	FILE * hd_src ;
  	int hd = NULL;
  	struct stat file_info;
    hd = open(path, O_RDONLY) ;
    if(!hd){
	  	printf("could not read %s\n", path);
    }else{
	  	printf("opened file\n");
		fstat(hd, &file_info);
		close(hd) ;
		length = file_info.st_size;
		hd_src = fopen(path, "rb");
		buffer = (char*)malloc(file_info.st_size);
		fread(buffer, 1, file_info.st_size, hd_src);
		fclose(hd_src);
	}
  }
  char * getData(){
  	return buffer;
  }
  int getLength(){
  	return length;
  }
  ~FileReader(){
    printf("FileReader dtor\n");
  	if(buffer){
  		free(buffer);
  	}
  }
private:
  char* buffer;
  int length;
  
};

