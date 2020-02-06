#pragma once
#include <stdio.h>
#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <map>
#include <pthread.h>
#include "core/synckeeper.h"

class MyAgentSession {
private:
	void *sock;

public:
	bool busy;
	pthread_t busythr;
	void killbusy();
	void sendText(std::string text,bool isError=false);
	void subscribe(const std::string eventN);
	void sendCommand(const std::string cmd,const std::string uuid="null");
	const std::string sendCommandSync(const std::string cmd);
	MyAgentSession(void *_sock);
	~MyAgentSession();
	void onMsg(std::string msg);
	void send(void *cl,std::string msg);
};
