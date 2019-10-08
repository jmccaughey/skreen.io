#pragma once
#include "HttpTimerJob.h"
#include <vector>
#include "windows.h"

class HostLatencyChecker
{
public:

	HostLatencyChecker(){
	}

	~HostLatencyChecker()
	{
	}

	void addHost(std::string host){
		hosts.push_back(host);
	}

	std::string getFastestHost(std::string path, unsigned int port, bool ssl){
		std::vector<HttpTimerJob*> jobs;
		std::vector<HANDLE> threads;
		std::string fastestHost("");
		unsigned int fastestResponse = 0;
		for (int i = 0; i < hosts.size(); i++){
			HttpTimerJob* httpTimerJob = new HttpTimerJob(hosts[i], port, path, ssl);
			jobs.push_back(httpTimerJob);
			HANDLE thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)HttpTimerJob::run, httpTimerJob, 0, NULL);
			threads.push_back(thread);
		}
		for (int i = 0; i < jobs.size(); i++){
			//threads[i]->join();
			WaitForSingleObject(threads[i], INFINITE);
			CloseHandle(threads[i]);
			HttpTimerJob* job = jobs[i];
			unsigned int responseTime = job->getDurationInMillis();
			unsigned int size = job->getResponseLength();
			if (size > 0 && responseTime > 0){
				if (fastestResponse == 0 || fastestResponse > responseTime){
					fastestHost = job->getHost();
					fastestResponse = responseTime;
				}
			}
			delete job;
		}
		//std::cout << "fastest request took " << fastestResponse << " milliseconds for " << fastestHost << std::endl;
		return fastestHost;
	}

private:
	std::vector<std::string> hosts;
    
};

