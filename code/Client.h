#pragma once
#define DEFAULT_BUFLEN 512

#include <winsock2.h>
#include <ws2tcpip.h> //addrinfo

#include <thread>
#include <string>

#include <iostream>
#include <syncstream>

#include "messages.pb.h"


class Client {
private:
	const char* HOST = "localhost";
	const char* PORT = "27015";

	std::thread recv_thread;
	std::thread input_thread;
	SOCKET conn_socket = INVALID_SOCKET;

	void recv_proc();
	int input_proc();
	int send_data(std::string data);

	unsigned int start();
	void shut_down();

public:
	bool startup_successful = false;
	Client();
	int run();
};

