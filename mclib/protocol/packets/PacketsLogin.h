#pragma once

#include <protocol/packets/Packet.h>

class Connection;
class PacketHandler;

namespace login
{

namespace in
{
//
// Login packets
//
class DisconnectPacket : public CMinecraftInboundPacket
{ // 0x00
private:
	MCString m_Reason;

public:
	MCLIB_API DisconnectPacket();

	bool MCLIB_API Deserialize(DataBuffer& data, std::size_t packetLength);
	void MCLIB_API Dispatch(PacketHandler* handler);

	std::wstring GetReason() const { return m_Reason.GetUTF16(); }
};

class EncryptionRequestPacket : public CMinecraftInboundPacket
{ // 0x01
private:
	MCString m_ServerId;
	std::string m_PublicKey;
	std::string m_VerifyToken;

public:
	MCLIB_API EncryptionRequestPacket();

	bool MCLIB_API Deserialize(DataBuffer& data, std::size_t packetLength);
	void MCLIB_API Dispatch(PacketHandler* handler);

	std::string GetPublicKey() const { return m_PublicKey; }
	std::string GetVerifyToken() const { return m_VerifyToken; }
	MCString GetServerId() const { return m_ServerId; }
};

class LoginSuccessPacket : public CMinecraftInboundPacket
{ // 0x02
public:
	MCLIB_API LoginSuccessPacket();
	bool MCLIB_API Deserialize(DataBuffer& data, std::size_t packetLength);
	void MCLIB_API Dispatch(PacketHandler* handler);

	std::wstring GetUUID() const { return m_UUID.GetUTF16(); }
	std::wstring GetUsername() const { return m_Username.GetUTF16(); }

private:
	MCString m_UUID;
	MCString m_Username;
};

class SetCompressionPacket : public CMinecraftInboundPacket
{ // 0x03
private:
	CVarInt m_MaxPacketSize;

public:
	MCLIB_API SetCompressionPacket();
	bool MCLIB_API Deserialize(DataBuffer& data, std::size_t packetLength);
	void MCLIB_API Dispatch(PacketHandler* handler);

	// Packets of this size or higher may be compressed
	int64 GetMaxPacketSize() const { return m_MaxPacketSize.GetLong(); }
};
}

namespace out
{
//
// HandshakePacket
//
class HandshakePacket : public CMinecraftOutboundPacket
{ // 0x00
private:
	CVarInt m_ProtocolVersion;
	MCString m_Server;
	uint16 m_Port;
	CVarInt m_NewState;

public:
	MCLIB_API HandshakePacket(int32 protocol, std::string server, uint16 port, State state);
	DataBuffer MCLIB_API Serialize() const;
};



//
// LoginStartPacket
//
class LoginStartPacket : public CMinecraftOutboundPacket
{ // 0x00
private:
	MCString m_Name;

public:
	MCLIB_API LoginStartPacket(const std::string& name);
	DataBuffer MCLIB_API Serialize() const;
};



//
// EncryptionResponsePacket
//
class EncryptionResponsePacket : public CMinecraftOutboundPacket
{ // 0x01
private:
	std::string m_SharedSecret;
	std::string m_VerifyToken;

public:
	MCLIB_API EncryptionResponsePacket(const std::string& sharedSecret, const std::string& verifyToken);
	DataBuffer MCLIB_API Serialize() const;

	std::string GetSharedSecret() const { return m_SharedSecret; }
	std::string GetVerifyToken() const { return m_VerifyToken; }
};

}
}