#include "FileReader.h"


int main(int argc, char** argv){
	if(argc > 1){
		FileReader fr(argv[1]);
		printf("%s is %d bytes long\n", argv[1], fr.getLength());
	}else{
		printf("usage: %s <filename> \n", argv[0]); 
	}
}