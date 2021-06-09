#include "stdafx.h"

#include <protocol/packets/PacketsStatus.h>

#include <core/Connection.h>
#include <protocol/PacketHandler.h>

namespace status
{


namespace in
{

//
// ResponsePacket
//
ResponsePacket::ResponsePacket()
{
	m_ProtocolState = State::Status;
}

bool ResponsePacket::Deserialize(DataBuffer& data, std::size_t packetLength)
{
	MCString response;
	data >> response;
	m_Response = response.GetUTF16();

	return true;
}

void ResponsePacket::Dispatch(PacketHandler* handler)
{
	handler->HandlePacket(this);
}



//
// PongPacket
//
PongPacket::PongPacket()
{
	m_ProtocolState = State::Status;
}

bool PongPacket::Deserialize(DataBuffer& data, std::size_t packetLength)
{
	data >> m_Payload;
	return true;
}

void PongPacket::Dispatch(PacketHandler* handler)
{
	handler->HandlePacket(this);
}

}





namespace out
{

//
// RequestPacket
//
RequestPacket::RequestPacket()
{
	m_ProtocolState = State::Status;
}
DataBuffer RequestPacket::Serialize() const
{
	DataBuffer buffer;
	buffer << m_Id;
	return buffer;
}



//
// PingPacket
//
PingPacket::PingPacket(s64 payload) : m_Payload(payload)
{
	m_ProtocolState = State::Status;
}
DataBuffer PingPacket::Serialize() const
{
	DataBuffer buffer;
	buffer << m_Id << m_Payload;
	return buffer;
}

} // ns out

} // ns status