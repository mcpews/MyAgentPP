#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>

class argInput {
private:
	void split(const std::string& s,std::vector<std::string>& sv,const char* delim = " ");

public:
	std::vector<std::string> splited;
	std::string doCmd;
	char type;
	unsigned int times;

	~argInput()=default;

	argInput(std::string cmd);
};
