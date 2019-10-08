#pragma once

#include <vector>
#include <string>

class StringSplitter
{
public:

	void split(std::vector<std::string>& results, std::string input){
		int start = 0;
		int finish = 0;
		while (finish <= input.length() && start != std::string::npos){
			start = finish;
			finish = input.find('\n', start);
			std::string next("");
			if (finish == std::string::npos){
				next = input.substr(start);
			} else{
				next = input.substr(start, finish - start);
				finish++;
			}
			if (next.length() > 0){
				results.push_back(next);
			}
		}
	}

	~StringSplitter()
	{
	}
};

