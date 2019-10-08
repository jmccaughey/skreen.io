class CurlPoster;

class PostJob{
  public:
    PostJob(char * data, int length){
    	_length = length;
    	_data = data;
    }
    ~PostJob(){
      printf("PostJob dtor\n");
    }
	int _length;
	char * _data;
};