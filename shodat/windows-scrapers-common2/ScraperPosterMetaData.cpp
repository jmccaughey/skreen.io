#include <string.h>
#include "ScraperPosterMetaData.h"

const std::string ScraperPosterMetaData::HTTP_SCHEME = "http://";
const std::string ScraperPosterMetaData::HTTPS_SCHEME = "https://";

ScraperPosterMetaData::ScraperPosterMetaData(std::string schemeHostPort,
                            std::string blastId, std::string blasterId, std::string key, std::string name) {
    this->_bSsl = schemeHostPort.find(HTTPS_SCHEME) == 0 ? true : false;
    // TODO: support non-standard ports
    if(getSsl()) {
        this->_port = 443;
    }else{
        this->_port = 80;
    }
    this->_host = schemeHostPort.substr(getSsl() ? HTTPS_SCHEME.length() : HTTP_SCHEME.length());
    this->_blastId = blastId;
    this->_blasterId = blasterId;
    this->_key = key;
    this->_name = name;
}
//
//ScraperPosterMetaData::ScraperPosterMetaData(const char* host, int port,
//		const char* context, bool bSsl, const char* blastId, const char* blasterId, const char* key, const char* name) {
//	::strcpy(_host, host);
//	_port = port;
//	::strcpy(_context, context);
//	_bSsl = bSsl;
//	::strcpy(_blastId, blastId);
//	::strcpy(_blasterId, blasterId);
//	::strcpy(_key, key);
//	::strcpy(_name, name);
//}

std::string ScraperPosterMetaData::buildPath(const char* servlet) {
    std::string url = servlet;
    url += "?blastId=";
    url += this->getBlastId();
    url += "&blasterId=";
    url += this->getBlasterId();
    url += "&key=";
    url += this->getKey();
    return url;
}

//const std::string ScraperPosterMetaData::getScheme() {
//    return getSsl() ? ScraperPosterMetaData::HTTPS_SCHEME : ScraperPosterMetaData::HTTP_SCHEME;/
//}
int ScraperPosterMetaData::getPort() {
	return _port;
}
//char* ScraperPosterMetaData::getContext() {
//	return _context;
//}
bool ScraperPosterMetaData::getSsl() {
	return _bSsl;
}
std::string ScraperPosterMetaData::getBlastId() {
	return _blastId;
}
std::string ScraperPosterMetaData::getBlasterId() {
	return _blasterId;
}
std::string ScraperPosterMetaData::getKey() {
	return _key;
}
std::string ScraperPosterMetaData::getName() {
	return _name;
}

std::string ScraperPosterMetaData::getViewUrl(){
	return getSsl() ? ScraperPosterMetaData::HTTPS_SCHEME : ScraperPosterMetaData::HTTP_SCHEME
		+ getHost() + "/" + getName();
}

