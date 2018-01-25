#include "Network/Network.h"

#include <winsock2.h>
#include <WS2tcpip.h>

Network::Network()
{
	WSADATA wsaData;
	int result = ::WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != 0)
	{
		print("WSAStartup failed: %i\n", result);
		assert(false);
		m_isInitialized = false;
		return;
	}
	m_isInitialized = true;
}

Network::~Network()
{
	if (m_isInitialized)
	{
		::WSACleanup();
	}
}