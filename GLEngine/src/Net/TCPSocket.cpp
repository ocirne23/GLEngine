#include "Network/TCPSocket.h"

#include "Utils/StringUtils.h"

#include <winsock2.h>
#include <WS2tcpip.h>
#include <stdio.h>

void TCPSocket::listen(Address a_ip, Port a_port, uint a_maxNumConnections)
{
	assert(ip && port);
	assert(m_socket == 0 && m_connectedSocket == 0);

	WSADATA wsaData;
	int result = ::WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != 0)
	{
		print("WSAStartup failed: %i\n", result);
		assert(false);
		return;
	}

	eastl::string portString = StringUtils::to_string(a_port);
	eastl::string ipString = a_ip;

	{
		ADDRINFOA hints = {};
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;
		PADDRINFOA info = {};
		::getaddrinfo(ipString.c_str(), portString.c_str(), &hints, &info);
	
		SOCKET tcpSocket = ::socket(info->ai_family, info->ai_socktype, info->ai_protocol);
		if (tcpSocket == INVALID_SOCKET)
		{
			print("Error creating socket: %i\n", WSAGetLastError());
			::freeaddrinfo(info);
			::WSACleanup();
			assert(false);
			return;
		}

		result = ::bind(tcpSocket, info->ai_addr, int(info->ai_addrlen));
		if (result == SOCKET_ERROR) {
			print("Socket bind failed: %i\n", WSAGetLastError());
			::freeaddrinfo(info);
			closesocket(tcpSocket);
			::WSACleanup();
			return;
		}
		m_socket = tcpSocket;
		freeaddrinfo(info);
	}

	if (::listen(m_socket, SOMAXCONN_HINT(a_maxNumConnections)) == SOCKET_ERROR) {
		printf("Socket listen failed: %i\n", WSAGetLastError());
		::closesocket(m_socket);
		m_socket = 0;
		::WSACleanup();
		return;
	}

	SOCKET connectedSocket = ::accept(m_socket, NULL, NULL);
	if (connectedSocket == INVALID_SOCKET) {
		printf("Socket accept failed: %i\n", WSAGetLastError());
		::closesocket(m_socket);
		::WSACleanup();
		return;
	}
	m_connectedSocket = connectedSocket;

	while (true)
	{
		result = ::recv(m_socket, m_receiveBuffer, BUFFER_SIZE, 0);
		if (result > 0) 
		{
			printf("Bytes received: %d\n", result);
		}
		else
		{
			printf("Connection closing: %i\n", result < 0 ? WSAGetLastError() : 0);
			::closesocket(m_socket);
			m_socket = 0;
			::WSACleanup();
			break;
		}
	}

	result = ::shutdown(m_connectedSocket, SD_SEND);
	if (result == SOCKET_ERROR) {
		printf("Socket shutdown failed: %i\n", WSAGetLastError());
		::closesocket(m_connectedSocket);
		m_connectedSocket = 0;
		::WSACleanup();
		return;
	}
	::closesocket(m_connectedSocket);
	m_connectedSocket = 0;
	::WSACleanup();
}

void TCPSocket::connect(Address a_ip, Port a_port)
{
	//----------------------
	// Initialize Winsock
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR) {
		print("WSAStartup function failed with error: %d\n", iResult);
		return;
	}
	//----------------------
	// Create a SOCKET for connecting to server
	SOCKET ConnectSocket;
	ConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ConnectSocket == INVALID_SOCKET) {
		print("socket function failed with error: %ld\n", WSAGetLastError());
		WSACleanup();
		return;
	}

	eastl::string ipString = a_ip;

	//----------------------
	// The sockaddr_in structure specifies the address family,
	// IP address, and port of the server to be connected to.
	sockaddr_in clientService;
	clientService.sin_family = AF_INET;
	clientService.sin_addr.s_addr = inet_addr(ipString.c_str());
	clientService.sin_port = htons(a_port);

	//----------------------
	// Connect to server.
	iResult = connect(ConnectSocket, (SOCKADDR *)& clientService, sizeof(clientService));
	if (iResult == SOCKET_ERROR) {
		wprintf(L"connect function failed with error: %ld\n", WSAGetLastError());
		iResult = closesocket(ConnectSocket);
		if (iResult == SOCKET_ERROR)
			wprintf(L"closesocket function failed with error: %ld\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}

	wprintf(L"Connected to server.\n");

	iResult = closesocket(ConnectSocket);
	if (iResult == SOCKET_ERROR) {
		wprintf(L"closesocket function failed with error: %ld\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}

	WSACleanup();
}
