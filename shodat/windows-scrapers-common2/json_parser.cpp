#include "json_parser.h"

std::string JsonParser::getValue(std::string key){
		std::size_t found = _source.find(key);
		if(found!=std::string::npos){
			size_t valueStart = found + key.length() + 2; // " and :
			if(valueStart < _source.length()){
				if(_source[valueStart] == '"'){
					valueStart++;
				}
				// find fist index of " or , or }
				for(size_t i = 0; i < _source.length() - valueStart; i++){
					char c = _source[valueStart + i];
					if(c == '"' || c == ',' || c == '}'){
						return _source.substr(valueStart, i);
					}
				}
			}
		}
		return std::string("");
}
/*
int main(int argc, char** argv){
    std::string keys[] = {"id", "key", "name", "blastId", "bogo"};

    JsonParser jsonParser(TEST_STRING);

    for(int i = 0; i < 5; i++){
        std::string value = jsonParser.getValue(keys[i]);
        if(value.length() > 0){
            printf("%s is %s\n", keys[i].c_str(), value.c_str());
        }else{
            printf("key %s not found\n", keys[i].c_str());
        }
    }


}
*/
