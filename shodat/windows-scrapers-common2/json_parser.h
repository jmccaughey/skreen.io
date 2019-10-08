#include <string>
#include <stdio.h>

static const std::string TEST_STRING("{\"blasterInfo\":{\"id\":2037391463,\"name\":\"512993\",\"javaClass\":\"com.deskblast.server.json.BlasterInfo\"},\"owner\":true,\"javaClass\":\"com.deskblast.server.json.BlasterCredentials\",\"blastId\":22722509,\"key\":\"fa0a0a60-b9d6-437e-a147-562ed08e64c0\"}");

class JsonParser{

public:
    JsonParser(std::string source){ _source = source; }
	std::string getValue(std::string key);

private:
	std::string _source;

};
