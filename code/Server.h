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


class Server {
private:
	const char* PORT = "27015";
	const unsigned int CLOCK_SPEED = 10;
	SOCKET ListenSocket = INVALID_SOCKET;
	std::vector<SOCKET> recv_sockets = {};
	std::thread conn_thread;
	std::thread clock_thread;
	DataContainer container;
	int conn_proc();
	void clock_proc();
	void recv_proc(SOCKET client_socket);
	int send_state();
	int update_state();
	int insert_into_state(char* new_data);
	unsigned int start();
	void shut_down();

public:
	bool startup_successful = false;
	Server();
	int run();
};

