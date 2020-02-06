#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "argv.h"
#include <string>
#include <vector>
#include <cstring>
#include <memory.h>
#include <iostream>
#include <map>

unsigned int str_f_replace(std::string& str,const std::string willreplace,const std::string to){
	unsigned int i=0;
	std::string::size_type n;
	while((n=str.find(willreplace))!=std::string::npos){
		str.replace(str.begin()+n,str.begin()+n+willreplace.size(),to);
		i++;
	}
	return i;
}

void argInput::split(const std::string& s,std::vector<std::string>& svn,const char* delim) {
	svn.clear();
	std::vector<std::string> sv;
	char* buffer = new char[s.size() + 1];
	memset((void*)buffer,0,s.size()+1);
	std::copy(s.begin(), s.end(), buffer);
	char* p = std::strtok(buffer, delim);
	do {
	sv.push_back(p);
	} while ((p = std::strtok(NULL, delim)));
	std::string lwa;
	bool lw=false;
	for(std::string str:sv){
		std::string rs=str;
		str_f_replace(rs,"\\\"","}}}}||||/||||{{{{");
		unsigned int iww=str_f_replace(rs,"\"","");
		str_f_replace(rs,"}}}}||||/||||{{{{","\"");
		if(0!=(iww%2)){
			if(!lw){
				lw=true;
				lwa=rs;
			}else{
				lw=false;
				lwa+=(std::string(" ")+rs);
				svn.push_back(lwa);std::cout<<lwa<<std::endl;
			}
		}else{
			if(!lw){
				svn.push_back(rs);
			}else{
				lwa+=(std::string(" ")+rs);
			}
		}
	}
	if(lw)throw std::string("Unterminated string.");
	return;
}

argInput::argInput(std::string cmd){
	if(cmd.c_str()[1]!='/'||(cmd.c_str()[0]!='*'&&cmd.c_str()[0]!='.'&&cmd.c_str()[0]!=':'&&cmd.c_str()[0]!='!')){
		throw (unsigned char)254;
		return;
	}
	type=cmd.c_str()[0];

	split(cmd.substr(2),splited,"~");
	if(splited.empty()){
		throw (unsigned char)254;
		return;
	}
	if(type==':'||type=='!'){
		if(splited.size()>=2){
			times=std::stoi(splited[1]);
		}else{
			times=1;
		}
	}else{
		times=0;
	}
	doCmd=splited[0];
	return;
}
