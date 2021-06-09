#ifndef PACKETS_PACKET_FACTORY_H_
#define PACKETS_PACKET_FACTORY_H_

#include <common/DataBuffer.h>
#include <protocol/Protocol.h>
#include <protocol/packets/Packet.h>

class Connection;

class PacketFactory
{
public:
	static MCLIB_API CMinecraftPacket* CreatePacket(Protocol& protocol, State state, DataBuffer data, std::size_t length, Connection* connection = nullptr);
	static void MCLIB_API FreePacket(CMinecraftPacket* packet);
};

#endif
