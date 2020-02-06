#include <string>
#include <iostream>
#include <mutex>
#include <App.h>
#include <pthread.h>
#include <utility>

void onMsg(void *,std::string);
void onConnection(void *);
void initDone();
void Z____Disconnection(void*);

void _send(void *cl,std::string msg){
	auto *client=(uWS::WebSocket<false,true>*)cl;
	client->send(std::string_view(msg),uWS::OpCode::TEXT);
}

void closews(void *cl){
	auto *client=(uWS::WebSocket<false,true>*)cl;
	client->close();
}

void onCon_newThread(void *ws){
	auto *rws=(uWS::WebSocket<false,true>*)ws;
	onConnection((void*)ws);
}

struct onMsgThreadStruct {
	void *ws;
	std::string_view msg;
};

void onMsg_newThread(void *strc){
	try {
		std::pair<void*, std::string_view> *sst = (std::pair<void*, std::string_view> *)strc;
		onMsg(sst->first, std::string(sst->second));
	}
	catch (const std::exception& err) {}
}

void createWebsocketServer(unsigned short port){
	struct PerSocketData {
			
	};
	uWS::App app = uWS::App();
	uWS::App::WebSocketBehavior beh;
	beh.compression = uWS::DISABLED;
	beh.maxPayloadLength = 2048;
	beh.idleTimeout = 1800;
	beh.open= [](auto *ws, auto *req) {
		pthread_t ptt;
		pthread_attr_t attr;
		pthread_attr_init(&attr);
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
		pthread_create(&ptt, &attr, (void *(*)(void*))onCon_newThread, (void*)ws);
	};
	beh.message = [](auto *ws, std::string_view message, uWS::OpCode opcode) {
		//struct onMsgThreadStruct tst={0};
		//tst.ws=(void*)ws;
		//tst.msg=message;
		pthread_t ptt;
		pthread_attr_t attr;
		pthread_attr_init(&attr);
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
		std::pair par = std::make_pair((void*)ws, message);
		pthread_create(&ptt, &attr, (void *(*)(void*))onMsg_newThread, (void*)&par);
	};
	beh.drain = [](auto *ws) {};
	beh.ping = [](auto*ws) {};
	beh.pong = [](auto*ws) {};
	beh.close = [](auto *ws, int code, std::string_view message) {
		Z____Disconnection((void*)ws);
	};
	app.ws<PerSocketData>("/*", (uWS::TemplatedApp<false>::WebSocketBehavior &&)beh).listen(port,[](auto *tk){
		if(tk){
			initDone();
		}
	}).run();
}
