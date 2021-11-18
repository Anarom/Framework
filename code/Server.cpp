#include "Server.h"


Server::Server() {
	Server::startup_successful = Server::start() == 0;
}

unsigned int Server::start() {
	WSADATA wsaData;
	int result;

	struct addrinfo* res = NULL;
	struct addrinfo hints;
	
	result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != 0) {
		printf("WSAStartup failed with error: %d\n", result);
		return 1;
	}
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	result = getaddrinfo(NULL, Server::PORT, &hints, &res);
	if (result != 0) {
		printf("getaddrinfo failed with error: %d\n", result);
		WSACleanup();
		return 1;
	}

	Server::ListenSocket = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (Server::ListenSocket == INVALID_SOCKET) {
		printf("socket failed with error: %ld\n", WSAGetLastError());
		freeaddrinfo(res);
		WSACleanup();
		return 1;
	}

	result = bind(Server::ListenSocket, res->ai_addr, (int)res->ai_addrlen);
	if (result == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(res);
		closesocket(Server::ListenSocket);
		WSACleanup();
		return 1;
	}

	freeaddrinfo(res);
	result = listen(Server::ListenSocket, SOMAXCONN);
	if (result == SOCKET_ERROR) {
		printf("listen failed with error: %d\n", WSAGetLastError());
		closesocket(Server::ListenSocket);
		WSACleanup();
		return 1;
	}
	std::cout << "server ready" << std::endl;
	return 0;
}

void Server::shut_down() {
	closesocket(Server::ListenSocket);
	WSACleanup();
}

int Server::run() {
	if (not Server::startup_successful) return 1;
	Server::clock_thread = std::thread(&Server::clock_proc, this);
	Server::conn_thread = std::thread(&Server::conn_proc, this);
	Server::conn_thread.join();
	Server::shut_down();
	return 0;
}

int Server::send_state() {
	//std::osyncstream synced_out(std::cout);
	int result = 0;
	if (Server::recv_sockets.size() == 0) return 1;
	std::string serialized_state = Server::container.SerializeAsString();
	for (unsigned int x = 0; x < Server::recv_sockets.size();) {
		result = send(Server::recv_sockets.at(x), serialized_state.c_str(), serialized_state.length(), 0);
		if (result < 0) Server::recv_sockets.erase(Server::recv_sockets.begin() + x);
		else x++;
	}
	//synced_out << "clock data sent to " << Server::recv_sockets.size() << " clients" << std::endl;
	//synced_out.emit();
	return 0;
}

int Server::update_state() {
	return 0;
}

int Server::process(char* action_string) {
	Action action;
	action.ParseFromString(action_string);
	ActionLib::methodPointer method = Server::aLib.get_action_by_id(action.amethod());
	(Server::aLib.*method)(action.aparams());
	return 0;
}

int Server::conn_proc(void) {
	SOCKET client_socket = INVALID_SOCKET;
	while (true) {
		client_socket = accept(Server::ListenSocket, NULL, NULL);
		std::thread new_thread(&Server::recv_proc, this, client_socket);
		Server::recv_sockets.push_back(client_socket);
		new_thread.detach();
	}
	return 0;
}

void Server::clock_proc(void) {
	while (true) {
		Sleep(Server::CLOCK_SPEED);
		Server::update_state();
		Server::send_state();
	}
}

void Server::recv_proc(SOCKET client_socket) {
	std::osyncstream synced_out(std::cout);
	char recvbuf[DEFAULT_BUFLEN];
	int iResult = 0;
	synced_out << "thread: " << std::this_thread::get_id() << " created" << std::endl;
	synced_out.emit();
	while (true) {
		iResult = recv(client_socket, recvbuf, DEFAULT_BUFLEN, 0);
		if (iResult > 0) {
			//synced_out << "thread: " << std::this_thread::get_id() << " data received" << std::endl;
			//synced_out.emit();
			Server::process(recvbuf);
		}
		else if (iResult <= 0) {
			shutdown(client_socket, SD_BOTH);
			synced_out << "thread: " << std::this_thread::get_id() << " destroyed" << std::endl;
			synced_out.emit();
			break;
		}
	}
}

ActionLib::ActionLib() {
	ActionLib::action_map[31] = &ActionLib::simple_action;
	ActionLib::action_map[32] = &ActionLib::another_simple_action;
}

ActionLib::methodPointer ActionLib::get_action_by_id(unsigned int action_id){
	ActionLib::methodPointer method = ActionLib::action_map[action_id];
	return method;
}

int ActionLib::simple_action(ActionParams params) {
	std::cout << "method 1 called" << std::endl;
	return 1;
}

int ActionLib::another_simple_action(ActionParams params) {
	std::cout << "method 2 called" << std::endl;
	return 1;
}

