#ifndef MCLIB_PROTOCOL_PACKETS_PACKET_DISPATCHER_H_
#define MCLIB_PROTOCOL_PACKETS_PACKET_DISPATCHER_H_

#include <protocol/Protocol.h>
#include <protocol/packets/Packet.h>




class PacketHandler;

class PacketDispatcher
{
private:
	typedef int64 PacketId;
	typedef std::pair<State, PacketId> PacketType;
	std::map<PacketType, std::vector<PacketHandler*>> m_Handlers;

public:
	PacketDispatcher() = default;

	PacketDispatcher(const PacketDispatcher& rhs) = delete;
	PacketDispatcher& operator=(const PacketDispatcher& rhs) = delete;
	PacketDispatcher(PacketDispatcher&& rhs) = delete;
	PacketDispatcher& operator=(PacketDispatcher&& rhs) = delete;

	void MCLIB_API Dispatch(CMinecraftPacket* packet);

	void MCLIB_API RegisterHandler(State protocolState, PacketId id, PacketHandler* handler);
	void MCLIB_API UnregisterHandler(State protocolState, PacketId id, PacketHandler* handler);
	void MCLIB_API UnregisterHandler(PacketHandler* handler);
};

#endif
