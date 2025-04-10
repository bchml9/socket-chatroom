#include <winsock2.h>
#include <Ws2tcpip.h>
#include <iostream>
#include <string>
#include <tchar.h>
#pragma comment(lib, "ws2_32.lib")

#define SERVER_ADDRESS "192.168.1.133" 
#define PORT 27015
#define BUFFER_SIZE 1024

/*
Initialize Winsock.
Create a socket.
Connect to the server.
Send and receive data.
Disconnect.
*/

int main(const int argc, const char *argv[]) {
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
   InetPton(AF_INET, _T(SERVER_ADDRESS), &server_addr.sin_addr.s_addr);

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
   WSACleanup();
}