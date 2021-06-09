#include "stdafx.h"

#include "PacketFactory.h"

#include <core/Connection.h>

#include <exception>


CMinecraftPacket* PacketFactory::CreatePacket(Protocol& protocol, State state, DataBuffer data, std::size_t length, Connection* connection)
{
	if (data.GetSize() == 0) 
		return nullptr;

	CVarInt vid;
	data >> vid;

	CMinecraftInboundPacket* packet = protocol.CreateInboundPacket(state, vid.GetInt());
	if (packet)
	{
		packet->SetConnection(connection);
		packet->Deserialize(data, length);
	}
	else
	{
		throw UnfinishedProtocolException(vid, state);
	}

	return packet;
}

void PacketFactory::FreePacket(CMinecraftPacket* packet)
{
	delete packet;
}
