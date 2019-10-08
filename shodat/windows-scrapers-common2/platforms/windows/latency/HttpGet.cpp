// HttpGet.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "HostLatencyChecker.h"
#include <vector>
#include <iostream>

int _tmain(int argc, _TCHAR* argv[]){
	std::vector<std::string> hosts = { 
		"skreen.io"
		,
		"west.skreen.io" 
	};
	HostLatencyChecker hostLatencyChecker;
	for (int i = 0; i < hosts.size(); i++){
		hostLatencyChecker.addHost(hosts[i]);
	}
	std::string fastestHost = hostLatencyChecker.getFastestHost("/index.html", 80, false);
	std::cout << "fastest host was " << fastestHost << std::endl;
	
	getchar();
	
	return 0;
}



