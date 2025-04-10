#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <iostream>
#include <tchar.h>
#include <string>
#include <thread>
#include <functional>

#define PORT 27015
#define HOST "192.168.1.133"
#define BUFFER_SIZE 1024

void receiveMessages(SOCKET client_sock) {
	char chat_buffer[BUFFER_SIZE];
	int recv_size;

	while (true) {
		// receive data from server
		recv_size = recv(client_sock, chat_buffer, sizeof(chat_buffer), 0);
		if (recv_size == SOCKET_ERROR) {
			std::cout << "recv() failed: " << WSAGetLastError() << std::endl;
			break;
		}
		else if (recv_size == 0) {
			std::cout << "server has disconnected :(" << std::endl;
			break;
		}

		// null-terminate + server response
		chat_buffer[recv_size] = '\0';
		std::cout << "\nServer: " << chat_buffer << std::endl;

		std::cout << "Client: ";
		std::cout.flush();
	}
	closesocket(client_sock);
}

void sendMessages(SOCKET client_sock) {
	while (true) {
		std::cout << "\rClient: ";
		std::cout.flush();

		std::string message;
		std::getline(std::cin, message);

		if (message == "exit") {
			std::cout << "exiting..." << std::endl;
			break;
		}

		send(client_sock, message.c_str(), message.length(), 0);
	}
	closesocket(client_sock);
}

int main(const int argc, const char* argv[]) {
	bool running = true;
	// initialize winsock
	WSAData wsaData;
	WORD wVersionRequested = MAKEWORD(2, 1);
	if (WSAStartup(wVersionRequested, &wsaData) != 0)
		ExitProcess(EXIT_FAILURE);

	// create the socket
	SOCKET client_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (client_sock == INVALID_SOCKET) {
		std::cout << "error at socket(): " << WSAGetLastError() << std::endl;
		WSACleanup();
		ExitProcess(EXIT_FAILURE);
	}
	else {
		std::cout << "socket is working :)" << std::endl;
	}

	// setup server connection info
	SOCKADDR_IN server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	InetPton(AF_INET, _T(HOST), &server_addr.sin_addr.s_addr);

	// connect to the server
	if (connect(client_sock, (sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
		std::cout << "connect() failed: " << WSAGetLastError() << std::endl;
		closesocket(client_sock);
		WSACleanup();
		ExitProcess(EXIT_FAILURE);
	}
	else {
		std::cout << "connected to server :)" << std::endl;
	}

	// create new threads to listen for messages from the seerver, and send
	std::thread send_thread(sendMessages, client_sock);
	std::thread receive_thread(receiveMessages, client_sock);


	send_thread.join();
	receive_thread.join();

	closesocket(client_sock);
	WSACleanup();

	return 0;
}