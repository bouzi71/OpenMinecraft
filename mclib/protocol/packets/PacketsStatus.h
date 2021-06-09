#pragma once

#include <protocol/packets/Packet.h>

class Connection;
class PacketHandler;

namespace status
{

namespace in
{

class ResponsePacket : public CMinecraftInboundPacket
{ // 0x00
private:
	std::wstring m_Response;

public:
	MCLIB_API ResponsePacket();

	bool MCLIB_API Deserialize(DataBuffer& data, std::size_t packetLength);
	void MCLIB_API Dispatch(PacketHandler* handler);

	const std::wstring& GetResponse() const { return m_Response; }
};


class PongPacket : public CMinecraftInboundPacket
{ // 0x01
public:
	MCLIB_API PongPacket();

	bool MCLIB_API Deserialize(DataBuffer& data, std::size_t packetLength);
	void MCLIB_API Dispatch(PacketHandler* handler);

	s64 GetPayload() const { return m_Payload; }

private:
	s64 m_Payload;
};

}



namespace out
{

class RequestPacket
	: public CMinecraftOutboundPacket
{ // 0x00
public:
	MCLIB_API RequestPacket();
	DataBuffer MCLIB_API Serialize() const;
};


class PingPacket
	: public CMinecraftOutboundPacket
{ // 0x01
public:
	MCLIB_API PingPacket(s64 payload);
	DataBuffer MCLIB_API Serialize() const;

private:
	s64 m_Payload;
};

} // ns out

} // ns status