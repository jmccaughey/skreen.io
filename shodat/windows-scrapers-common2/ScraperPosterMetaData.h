#ifndef SCRAPER_POSTER_META_DATA_H
#define SCRAPER_POSTER_META_DATA_H

#include <stdio.h>
#include <string>
class ScraperPosterMetaData {

public:
    ScraperPosterMetaData(std::string schemHostPort, std::string blastId, std::string blasterId, std::string key, std::string name);
    std::string buildPath(const char* servlet);
    std::string getHost() { return _host; }
	int getPort();
    std::string getContext();
	bool getSsl();
    std::string getBlastId();
	std::string getBlasterId();
	std::string getKey();
	std::string getName();
    std::string getViewUrl();
    static const std::string HTTPS_SCHEME;
    static const std::string HTTP_SCHEME;

private:
    //std::string schemHostPort;
    std::string _host;
    int _port;
    std::string _context;
    bool _bSsl;
    std::string _blastId;
    std::string _blasterId;
    std::string _key;
    std::string _name;
};

#endif //SCRAPER_POSTER_META_DATA_H
