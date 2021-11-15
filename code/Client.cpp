#include "Client.h"


Client::Client() {
	Client::startup_successful = Client::start() == 0;
}

unsigned int Client::start() {
	int result;
	WSADATA wsaData;
	struct addrinfo*
		res = NULL,
		* ptr = NULL,
		hints;
	result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != 0) {
		std::cout << "WSAStartup failed with error: " << result << "\n";
		return result;
	}
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	result = getaddrinfo(Client::HOST, Client::PORT, &hints, &res);
	if (result != 0) {
		std::cout << "getaddrinfo failed with error: " << result << "\n";
		WSACleanup();
		return result;
	}

	for (ptr = res; ptr != NULL; ptr = ptr->ai_next) {
		Client::conn_socket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (Client::conn_socket == INVALID_SOCKET) {
			std::cout << "socket failed with error: " << WSAGetLastError() << "\n";
			WSACleanup();
			return WSAGetLastError();
		}
		result = connect(Client::conn_socket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (result == SOCKET_ERROR) {
			closesocket(Client::conn_socket);
			Client::conn_socket = INVALID_SOCKET;
			continue;
		}
		break;
	}
	freeaddrinfo(res);
	if (Client::conn_socket == INVALID_SOCKET) {
		std::cout << "Unable to connect to server!\n";
		WSACleanup();
		return 1;
	}
	std::cout << "connected" << std::endl;
	return 0;
}

void Client::shut_down() {
	closesocket(Client::conn_socket);
	WSACleanup();
}

int Client::run() {
	if (not Client::startup_successful) return 1;
	Client::input_thread = std::thread(&Client::input_proc, this);
	Client::recv_thread = std::thread(&Client::recv_proc, this);
	Client::input_thread.join();
	Client::shut_down();
}

int Client::send_data(std::string data) {
	std::osyncstream synced_out(std::cout);
	int result = send(Client::conn_socket, data.c_str(), data.length(), 0);
	if (result == SOCKET_ERROR) {
		std::cout << "send failed with error: " << WSAGetLastError() << "\n";
		return WSAGetLastError();
	}
	//synced_out << "data sent" << std::endl;
	//synced_out.emit;
	return 0;
}



int Client::input_proc(void) {
	//std::osyncstream synced_out(std::cout);
	Action generated_action;
	int result = 0;

	//user input here
	unsigned int messages_left = 10;
	while (messages_left > 0) {
		result = Client::send_data(generated_action.SerializeAsString());
		if (result != 0) return result;
		messages_left--;
		Sleep(1000);
	}
	return 0;
}

void Client::recv_proc(void) {
	//std::osyncstream synced_out(std::cout);
	int result = 0;
	char recvbuf[DEFAULT_BUFLEN];
	while (true) {
		result = recv(Client::conn_socket, recvbuf, DEFAULT_BUFLEN, 0);
		if (result > 0) {
			//synced_out << "recv thread: data received" << std::endl;
			//synced_out.emit();
			//process received data here
		}
		else if (result <= 0) {
			//synced_out << "recv thread: destroyed" << std::endl;
			//synced_out.emit();
			shutdown(Client::conn_socket, SD_SEND);
			closesocket(Client::conn_socket);
			break;
		}
	}
}