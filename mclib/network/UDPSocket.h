#ifndef NETWORK_UDP_SOCKET_H_
#define NETWORK_UDP_SOCKET_H_

#include <network/IPAddress.h>
#include <network/Socket.h>

#include <cstdint>

class UDPSocket 
	: public Socket
{
public:
	UDPSocket();

	bool MCLIB_API Connect(const IPAddress& address, uint16_t port);
	std::size_t MCLIB_API Send(const uint8* data, std::size_t size);
	DataBuffer MCLIB_API Receive(std::size_t amount);

private:
	IPAddress m_RemoteIP;
	uint16_t m_Port;
	sockaddr_in m_RemoteAddr;
};

#endif
