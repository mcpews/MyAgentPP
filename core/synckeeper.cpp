#include <string>
#include <map>
#include <iostream>
#include <memory.h>

std::map<std::string,std::string> wantedUUIDMap;
std::map<std::string,bool> needMark;

std::string getUValue(const std::string a){
	auto result=wantedUUIDMap.find(a);
	if(result!=wantedUUIDMap.end()){
		std::string wret=result->second;
		wantedUUIDMap.erase(a);
		return wret;
	}else{
		return "";
	}
}

void setUValue(const std::string a,const std::string val){
	wantedUUIDMap[a]=val;
	return;
}

bool getNMark(const std::string a){
	//return needMark[a];
	auto result=needMark.find(a);
	if(result!=needMark.end()){
		needMark.erase(a);
		return true;
	}else{
		return false;
	}
}

void setNMark(const std::string a){
	needMark[a]=true;
	return;
}