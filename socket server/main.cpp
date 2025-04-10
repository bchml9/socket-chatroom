#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <iostream>
#include <tchar.h>
#include <string>
#include <thread>

#define SERVER_ADDRESS "192.168.1.133" 
#define PORT 27015
#define BUFFER_SIZE 1024

void handleClient(SOCKET client_sock) {
	char chat_buffer[BUFFER_SIZE];
	while (true) { // u need a while loop for this !
		// receive data from the client
		int recv_size = recv(client_sock, chat_buffer, sizeof(chat_buffer), 0);
		if (recv_size == SOCKET_ERROR) {
			std::cout << "recv() failed: " << WSAGetLastError() << std::endl;
			break;
		}
		else if (recv_size == 0) {
			std::cout << "Client disconnected." << std::endl;
			break;
		}
		// receive and send messages
		chat_buffer[recv_size] = '\0';
		std::cout << "\nClient: " << chat_buffer << std::endl;

		std::cout << "Server: ";
		std::cout.flush();

	}
	// close socket when communication ends. lol
	closesocket(client_sock);
}

void sendMessages(SOCKET client_sock) {
	while (true) {
		std::cout << "\rServer: ";
		std::cout.flush();
		std::string server_message;
		std::getline(std::cin, server_message);

		send(client_sock, server_message.c_str(), server_message.length(), 0);
	}
	closesocket(client_sock);
}

int main(const int argc, const char* argv[]) {
	// initialize winsock
	WSAData wsaData;
	WORD wVersionRequested = MAKEWORD(2, 1);
	if (WSAStartup(wVersionRequested, &wsaData) != 0)
		ExitProcess(EXIT_FAILURE);

	// create the socket
	SOCKET server_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (server_sock == INVALID_SOCKET) {
		std::cout << "error at socket(): " << WSAGetLastError() << std::endl;
		WSACleanup();
		ExitProcess(EXIT_FAILURE);
	}
	else {
		std::cout << "socket is working :)" << std::endl;
	}

	// bind socket
	SOCKADDR_IN server_addr;
	server_addr.sin_family = AF_INET;
	InetPton(AF_INET, _T(SERVER_ADDRESS), &server_addr.sin_addr.s_addr);
	server_addr.sin_port = htons(PORT);
	if (bind(server_sock, (sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) { // returns 0 if successful, otherwise SOCKET_ERROR 
		std::cout << "bind() failed: " << WSAGetLastError() << std::endl;
		closesocket(server_sock);
		WSACleanup();
		ExitProcess(EXIT_FAILURE);
	}
	else {
		std::cout << "binded :)" << std::endl;
	}

	// start listening
	if (listen(server_sock, 1) == SOCKET_ERROR) {
		std::cout << "listen() failed: " << WSAGetLastError() << std::endl;
		closesocket(server_sock);
		WSACleanup();
		ExitProcess(EXIT_FAILURE);
	}
	else
		std::cout << "listen() worked :)" << std::endl;

	std::cout << "server is listening on port " << PORT << "..." << std::endl;

	// accept client connection
	while (true) {
		SOCKADDR_IN client_addr;
		int client_len = sizeof(client_addr);
		SOCKET client_sock = accept(server_sock, (sockaddr*)&client_addr, &client_len);
		if (client_sock == INVALID_SOCKET) {
			std::cout << "accept() failed: " << WSAGetLastError() << std::endl;
			continue;
		}
		std::cout << "client connected :)" << std::endl;

		std::thread client_receive_thread(handleClient, client_sock);
		std::thread client_send_thread(sendMessages, client_sock);

		// detach threads to handle multiple clients independently
		client_receive_thread.detach();
		client_send_thread.detach();
	}


	closesocket(server_sock);

	WSACleanup();
	ExitProcess(EXIT_SUCCESS);
}

