#include <stdio.h>
#include <string.h>
#include <iostream>
#include <memory>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <signal.h>
#include <map>
#include <pthread.h>
#include "core/argv.h"
#include "jsoncpp-1.8.4/include/json/json.h"
#include "main.h"
#include <exception>
#include <stdexcept>
#include <time.h>
#include <thread>
#include <readline/readline.h>
#include <readline/history.h>

void _send(void *cl,std::string msg);
void closews(void *cl);


char *random_uuid( char buf[37] )
{
	const char *c = "89ab";
	char *p = buf;
	for(int n=0;n<16;n++){
		int b=rand()%255;
		switch(n){
			case 6:
				sprintf(p,"4%x",b%15 );
				break;
			case 8:
				sprintf(p,"%c%x",c[rand()%strlen( c )],b%15 );
				break;
			default:
				sprintf(p,"%02x",b );
				break;
		}
		p+=2;
		switch(n)
		{
			case 3:
			case 5:
			case 7:
			case 9:
				*p++ = '-';
				break;
		}
	}
	*p = 0;
	return buf;
}

void initDone(void){
	std::cout<<"Server initialized."<<std::endl;
}

void createWebsocketServer(unsigned short port);

class MyAgentSession;
class MyAgent {
public:
	bool shutting_down;
	std::map<void *,MyAgentSession*> SMap;

	MyAgent(){
		shutting_down=false;
		new std::thread([=](){
			while(true){
				char *cmdline=readline("");
				add_history(cmdline);
				for(auto i:SMap){
					i.second->sendCommand(std::string(cmdline));
				}
				free(cmdline);
			}
		});
	}

	~MyAgent(){
		shutting_down=true;
		std::cout<<"Shutting down..."<<std::endl;
		for(auto i:SMap){
			delete i.second;
		}
	}

};

MyAgent *myagent;

void MyAgentSession::sendText(std::string text,bool isError){
	std::string cmd;
	cmd=((isError)?std::string("say §4"):std::string("say §b"))+text;
	sendCommand(cmd);
}

void MyAgentSession::subscribe(const std::string eventN){
	char rUUID[37]={0};
	random_uuid(rUUID);
	Json::Value rootVal;
	Json::FastWriter fw;
	Json::Value header;
	Json::Value body;
	header["requestId"]=Json::Value(std::string(rUUID));
	header["messagePurpose"]=Json::Value("subscribe");
	header["version"]=Json::Value(1);
	header["messageType"]=Json::Value("commandRequest");
	rootVal["header"]=header;
	body["eventName"]=Json::Value(eventN);
	rootVal["body"]=body;
	send(sock,fw.write(rootVal));
	return;
}

void MyAgentSession::sendCommand(const std::string cmd,std::string uuid){
	char oUUID[37]={0};
	std::string rUUID;
	char isATG=0;
	if(uuid=="null"){
		random_uuid(oUUID);
		rUUID=std::string(oUUID);
		isATG=1;
	}else{
		rUUID=uuid;
	}
	Json::Value packetj;
	Json::Value header;
	header["requestId"]=Json::Value(rUUID);
	header["messagePurpose"]=Json::Value("commandRequest");
	header["version"]=Json::Value(1);
	header["messageType"]=Json::Value("commandRequest");
	packetj["header"]=header;
	Json::Value body;
	Json::Value origin;
	origin["type"]=Json::Value("player");
	body["origin"]=origin;
	body["commandLine"]=Json::Value(cmd);
	body["version"]=Json::Value(1);
	packetj["body"]=body;
	Json::FastWriter fwt;
	send(sock,fwt.write(packetj));
}

const std::string MyAgentSession::sendCommandSync(const std::string cmd){
	char rUUID[37]={0};
	random_uuid(rUUID);
	sendCommand(cmd,std::string(rUUID));
	setNMark(std::string(rUUID));
	std::string result;
	while((result=getUValue(std::string(rUUID)))==""){usleep(100000);}
	return result;
}

MyAgentSession::MyAgentSession(void *_sock){
	busy=true;
	sock=_sock;
	std::cout<<"A client connected."<<std::endl;
	subscribe("PlayerMessage");
	sendText("MyAgent++ connected!");
	busy=false;
}

MyAgentSession::~MyAgentSession(){
	if(busy){
		std::cout<<"Session is busy.Killing..."<<std::endl;
		killbusy();
	}
	std::cout<<"Connection closed."<<std::endl;
	closews(sock);
}

void MyAgentSession::killbusy(){
#ifndef Android
	if(busy)pthread_cancel(busythr);
#endif
	busy=false;
}

void MyAgentSession::onMsg(std::string msg){
	Json::Reader reader;
	Json::Value proot;
	if(!reader.parse(msg,proot)){
		return;
	}
	
	std::string mPur=proot["header"]["messagePurpose"].asString();
	std::string requestId=proot["header"]["requestId"].asString();
	if(getNMark(requestId)){
		setUValue(requestId,msg);
	}
	if(mPur=="error"){
	}else if(mPur=="commandResponse"){
		std::cout<<msg<<std::endl;
	}else if(mPur=="event"){
		if(!proot["body"]["properties"].isMember("Message"))return;
		std::string pmsg=proot["body"]["properties"]["Message"].asString();
		try{
			argInput inp(pmsg);
			if(inp.type=='*'||inp.type==':'){
				if(inp.doCmd=="fenchant"){
					char cmdname[128]={0};
#define enc(level,id) sprintf(cmdname,"enchant @s %d %d",level,id);sendCommandSync(std::string(cmdname))
					for(char i=0;i<33;i++){
						if(i==10||i==11||i==16||i==30){continue;}
						switch(i){
							case 0:
								enc(4,i);break;
							case 1:
								enc(4,i);break;
							case 2:
								enc(4,i);break;
							case 3:
								enc(4,i);break;
							case 4:
								enc(4,i);break;
							case 5:
								enc(3,i);break;
							case 6:
								enc(3,i);break;
							case 7:
								enc(3,i);break;
							case 8:
								enc(1,i);break;
							case 9:
								enc(5,i);break;
							case 10:
								enc(5,i);break;
							case 11:
								enc(5,i);break;
							case 12:
								enc(2,i);break;
							case 13:
								enc(2,i);break;
							case 14:
								enc(3,i);break;
							case 15:
								enc(5,i);break;
							case 16:
								enc(1,i);break;
							case 17:
								enc(3,i);break;
							case 18:
								enc(3,i);break;
							case 19:
								enc(5,i);break;
							case 20:
								enc(2,i);break;
							case 21:
								enc(1,i);break;
							case 22:
								enc(1,i);break;
							case 23:
								enc(3,i);break;
							case 24:
								enc(3,i);break;
							case 25:
								enc(2,i);break;
							case 26:
								enc(1,i);break;
							case 27:
								enc(1,i);break;
							case 28:
								enc(1,i);break;
							case 29:
								enc(5,i);break;
							case 30:
								enc(3,i);break;
							case 31:
								enc(3,i);break;
							case 33:
								enc(1,i);break;
							case 34:
								enc(1,i);break;
							case 35:
								enc(1,i);break;
							case 32:
							default:
								enc(1,i);break;
						}
					}
					return;
				}
				if(inp.times>0){
					sendText("Doing a loop...");
					for(unsigned int i=0;i<inp.times;i++){
						sendCommand(std::string("agent ")+inp.doCmd);
						usleep(150000);
					}
					sendText("Finished the loop.");
					return;
				}else{
					std::string cmdresp=sendCommandSync(std::string("agent ")+inp.doCmd);
					Json::Reader jreader;
					Json::Value jsonroot;
					if(!jreader.parse(cmdresp,jsonroot)){
						sendText("Command finished but myagent couldn't parse the response json.",true);
						return;
					}
					if(!jsonroot["body"].isMember("statusMessage")){
						sendText("Command response doesn't contain a status message.",jsonroot["body"]["statusCode"].asInt()<0);
					}else{
						sendText(std::string("Command response:\n")+jsonroot["body"]["statusMessage"].asString(),jsonroot["body"]["statusCode"].asInt()<0);
					}
				}
			}else if(inp.type=='.'||inp.type=='!'){
				if(inp.times>0){
					sendText("Doing a loop...");
					for(unsigned int i=0;i<inp.times;i++){
						sendCommand(inp.doCmd);
						usleep(150000);
					}
					sendText("Finished the loop.");
					return;
				}else{
					std::string cmdresp=sendCommandSync(inp.doCmd);
					Json::Reader jreader;
					Json::Value jsonroot;
					if(!jreader.parse(msg,jsonroot)){
						sendText("Command finished but myagent couldn't parse the response json.",true);
						return;
					}
					if(!jsonroot["body"]["statusMessage"].isString()){
						sendText("Command response doesn't contain a status message.",jsonroot["body"]["statusCode"].asInt()<0);
					}else{
						sendText(std::string("Command response:\n")+jsonroot["body"]["statusMessage"].asString(),jsonroot["body"]["statusCode"].asInt()<0);
					}
				}
			}
		}catch(std::string ex){
			sendText(std::string("CCmd Parsing Error: ")+ex,true);
		}catch(unsigned char i){
			if(i!=254)throw i;
		}
	}
}

void MyAgentSession::send(void*cl,std::string msg){
	_send(cl,msg);
}

void onConnection(void *cSock){
	if(myagent->shutting_down){
		closews(cSock);
		return;
	}
	myagent->SMap[cSock]=new MyAgentSession(cSock);
}

void onMsg(void *cSock,std::string msg){
	if(myagent->shutting_down)return;
	auto result=myagent->SMap.find(cSock);
	if(result==myagent->SMap.end())return;
#ifndef Android
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,NULL);
#endif
	myagent->SMap[cSock]->onMsg(msg);
}

void Z____Disconnection(void* ws){
	if(myagent->shutting_down)return;
	auto result=myagent->SMap.find(ws);
	if(result==myagent->SMap.end())return;
	myagent->SMap[ws]->killbusy();
	delete myagent->SMap[ws];
	myagent->SMap.erase(ws);
}

/*std::map<void *,FastBuilderSession*> FastBuilder_Z____GetFBSMap(){
	return fastbuilder->FBSMap;
}*/

int main(){
	rl_catch_signals=0;
	myagent=new MyAgent();
	signal(SIGINT,[](int signal){
		rl_cleanup_after_signal();
		delete myagent;
		std::cout<<"Quit correctly"<<std::endl;
		exit(0);
	});
	std::cout<<"Port:19131"<<std::endl;
	createWebsocketServer(19131);
	return 0;
}
