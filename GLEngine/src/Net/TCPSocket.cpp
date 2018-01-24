#include "Network/TCPSocket.h"

#include "Utils/StringUtils.h"

#include <winsock2.h>
#include <WS2tcpip.h>
#include <stdio.h>

void TCPSocket::initialize(Address ip, Port port)
{
	assert(ip && port);
	assert(m_socket == NULL);

	WSADATA wsaData;
	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != 0)
	{
		print("WSAStartup failed: %i\n", result);
		assert(false);
		return;
	}

	eastl::string portString = StringUtils::to_string(port);
	eastl::string ipString = ip;

	ADDRINFOA hints = {};
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	PADDRINFOA info = {};
	getaddrinfo(ipString.c_str(), portString.c_str(), &hints, &info);
	
	SOCKET tcpSocket = socket(info->ai_family, info->ai_socktype, info->ai_protocol);
	if (tcpSocket == INVALID_SOCKET)
	{
		print("Error creating socket: %i\n", WSAGetLastError());
		freeaddrinfo(info);
		WSACleanup();
		assert(false);
		return;
	}

	m_socket = rcast<uint*>(tcpSocket);
}