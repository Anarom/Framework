#pragma once
#define DEFAULT_BUFLEN 512

#include <winsock2.h>
#include <ws2tcpip.h> //addrinfo

#include <algorithm>
#include <thread>
#include <vector>

#include <iostream>
#include <syncstream>

#include "messages.pb.h"


class ActionLib {
public:
	typedef  int (ActionLib::* methodPointer)(ActionParams);
	ActionLib();
	methodPointer get_action_by_id(unsigned int action_id);
private:
	std::map<unsigned int, methodPointer> action_map;
	int simple_action(ActionParams params);
	int another_simple_action(ActionParams params);
};



class Server {
private:
	const char* PORT = "27015";
	std::vector<SOCKET> recv_sockets = {};
	SOCKET ListenSocket = INVALID_SOCKET;
	std::thread conn_thread;
	std::thread clock_thread;
	DataContainer container;
	int conn_proc();
	void clock_proc();
	void recv_proc(SOCKET client_socket);
	int send_state();
	int update_state();
	int process(char* action_string);
	unsigned int start();
	void shut_down();
	const unsigned int CLOCK_SPEED = 10;

	ActionLib aLib;
	
public:
	bool startup_successful = false;
	Server();
	int run();
};

