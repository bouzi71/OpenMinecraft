#include "stdafx.h"

#include <protocol/packets/PacketsLogin.h>

#include <core/Connection.h>
#include <protocol/PacketHandler.h>

namespace login
{

namespace in
{
//
// DisconnectPacket
//
DisconnectPacket::DisconnectPacket()
{
	m_ProtocolState = State::Login;
}
bool DisconnectPacket::Deserialize(DataBuffer& data, std::size_t packetLength)
{
	// Update the protocol state so the login and play versions of this are handled correctly.
	if (m_Connection)
		m_ProtocolState = m_Connection->GetProtocolState();

	data >> m_Reason;
	return true;
}
void DisconnectPacket::Dispatch(PacketHandler* handler)
{
	handler->HandlePacket(this);
}



//
// EncryptionRequestPacket
//
EncryptionRequestPacket::EncryptionRequestPacket()
{
	m_ProtocolState = State::Login;
}
bool EncryptionRequestPacket::Deserialize(DataBuffer& data, std::size_t packetLength)
{
	CVarInt pubKeyLen;
	CVarInt verifyTokenLen;

	data >> m_ServerId;

	data >> pubKeyLen;
	data.ReadSome(m_PublicKey, pubKeyLen.GetInt());

	data >> verifyTokenLen;
	data.ReadSome(m_VerifyToken, verifyTokenLen.GetInt());

	return true;
}
void EncryptionRequestPacket::Dispatch(PacketHandler* handler)
{
	handler->HandlePacket(this);
}



//
// LoginSuccessPacket
//
LoginSuccessPacket::LoginSuccessPacket()
{
	m_ProtocolState = State::Login;
}
bool LoginSuccessPacket::Deserialize(DataBuffer& data, std::size_t packetLength)
{
	data >> m_UUID;
	data >> m_Username;
	return true;
}
void LoginSuccessPacket::Dispatch(PacketHandler* handler)
{
	handler->HandlePacket(this);
}



//
// SetCompressionPacket
//
SetCompressionPacket::SetCompressionPacket()
{
	m_ProtocolState = State::Login;
}

bool SetCompressionPacket::Deserialize(DataBuffer& data, std::size_t packetLength)
{
	data >> m_MaxPacketSize;
	return true;
}

void SetCompressionPacket::Dispatch(PacketHandler* handler)
{
	handler->HandlePacket(this);
}

}



namespace out
{

//
// Handshake packets
//
HandshakePacket::HandshakePacket(int32 protocol, std::string server, uint16 port, State state)
	: m_ProtocolVersion(protocol)
	, m_Server(server)
	, m_Port(port)
	, m_NewState((int32)state)
{
	m_ProtocolState = State::Handshake;
}
DataBuffer HandshakePacket::Serialize() const
{
	DataBuffer buffer;
	buffer << m_Id << m_ProtocolVersion << m_Server << m_Port << m_NewState;
	return buffer;
}



//
// LoginStartPacket
//
LoginStartPacket::LoginStartPacket(const std::string& name)
	: m_Name(name)
{
	m_ProtocolState = State::Login;
}
DataBuffer LoginStartPacket::Serialize() const
{
	DataBuffer buffer;
	buffer << m_Id;
	buffer << m_Name;
	return buffer;
}



//
// EncryptionResponsePacket
//
EncryptionResponsePacket::EncryptionResponsePacket(const std::string& sharedSecret, const std::string& verifyToken)
	: m_SharedSecret(sharedSecret)
	, m_VerifyToken(verifyToken)
{
	m_ProtocolState = State::Login;
}
DataBuffer EncryptionResponsePacket::Serialize() const
{
	DataBuffer buffer;

	buffer << m_Id;

	CVarInt sharedLength = (int32)m_SharedSecret.length();
	CVarInt verifyLength = (int32)m_VerifyToken.length();
	buffer << sharedLength;
	buffer << m_SharedSecret;
	buffer << verifyLength;
	buffer << m_VerifyToken;
	return buffer;
}

}

}