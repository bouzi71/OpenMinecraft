#ifndef MCLIB_CORE_CONNECTION_H_
#define MCLIB_CORE_CONNECTION_H_

#include <common/DataBuffer.h>
#include <common/JsonFwd.h>
#include <common/Types.h>
#include <core/AuthToken.h>
#include <core/ClientSettings.h>
#include <core/Compression.h>
#include <core/Encryption.h>
#include <network/Socket.h>
#include <protocol/Protocol.h>
#include <protocol/packets/Packet.h>
#include <protocol/PacketHandler.h>
#include <util/ObserverSubject.h>
#include <util/Yggdrasil.h>


#include <queue>
#include <future>

#include <cstdio>

class ConnectionListener
{
public:
	virtual MCLIB_API ~ConnectionListener() {}

	virtual void MCLIB_API OnSocketStateChange(Socket::Status newStatus) {}
	virtual void MCLIB_API OnLogin(bool success) {}
	virtual void MCLIB_API OnAuthentication(bool success, std::string error) {}
	virtual void MCLIB_API OnPingResponse(const nlohmann::json& node) {}
};

class Connection
	: public PacketHandler
	, public ObserverSubject<ConnectionListener>
{
private:
	std::unique_ptr<EncryptionStrategy> m_Encrypter;
	std::unique_ptr<CompressionStrategy> m_Compressor;
	std::unique_ptr<Socket> m_Socket;
	std::unique_ptr<Yggdrasil> m_Yggdrasil;
	ClientSettings m_ClientSettings;
	std::string m_Server;
	std::string m_Email;
	std::string m_Username;
	std::string m_Password;
	DataBuffer m_HandleBuffer;
	Protocol& m_Protocol;
	State m_ProtocolState;
	uint16 m_Port;
	bool m_SentSettings;
	int32 m_Dimension;

	void AuthenticateClient(const std::wstring& serverId, const std::string& sharedSecret, const std::string& pubkey);
	CMinecraftPacket* CreatePacket(DataBuffer& buffer);
	void SendSettingsPacket();

public:
	MCLIB_API Connection(PacketDispatcher* dispatcher, Version version = Version::Minecraft_1_12_2);
	MCLIB_API ~Connection();

	Connection(const Connection& other) = delete;
	Connection& operator=(const Connection& rhs) = delete;
	Connection(Connection&& other) = delete;
	Connection& operator=(Connection&& rhs) = delete;

	Yggdrasil* GetYggdrasil() { return m_Yggdrasil.get(); }
	Socket::Status MCLIB_API GetSocketState() const;
	ClientSettings& GetSettings() noexcept { return m_ClientSettings; }
	int32 GetDimension() const noexcept { return m_Dimension; }
	State GetProtocolState() const { return m_ProtocolState; }

	void SendSettings() noexcept { m_SentSettings = false; }

	void MCLIB_API HandlePacket(in::KeepAlivePacket* packet);
	void MCLIB_API HandlePacket(in::PlayerPositionAndLookPacket* packet);

	void MCLIB_API HandlePacket(login::in::DisconnectPacket* packet);
	void MCLIB_API HandlePacket(login::in::EncryptionRequestPacket* packet);
	void MCLIB_API HandlePacket(login::in::LoginSuccessPacket* packet);
	void MCLIB_API HandlePacket(login::in::SetCompressionPacket* packet);

	void MCLIB_API HandlePacket(in::JoinGamePacket* packet);
	void MCLIB_API HandlePacket(in::RespawnPacket* packet);
	void MCLIB_API HandlePacket(in::UpdateHealthPacket* packet);
	void MCLIB_API HandlePacket(status::in::ResponsePacket* packet);

	bool MCLIB_API Connect(const std::string& server, uint16 port);
	void MCLIB_API Disconnect();
	void MCLIB_API CreatePacket();

	void MCLIB_API Ping();
	bool MCLIB_API Login(const std::string& username, const std::string& password);
	bool MCLIB_API Login(const std::string& username, AuthToken token);

	template <typename T>
	void SendPacket(T&& packet)
	{
		int32 id = m_Protocol.GetPacketId(packet);

		packet.SetId(id);
		packet.SetProtocolVersion(m_Protocol.GetVersion());

		if (packet.GetProtocolState() == State::Play && GetProtocolState() != State::Play)
		{
			printf("Skipping packet '0x%X'. State '%d'.\r\n", packet.GetId().GetInt(), packet.GetProtocolState());
			//return;
		}

		//if (packet.GetId().GetInt() != 0x0E)
			printf("Sending packet '0x%X'. State '%d'. Size '%d'\r\n", packet.GetId().GetInt(), packet.GetProtocolState(), packet.Serialize().GetSize());

		DataBuffer packetBuffer = packet.Serialize();
		DataBuffer compressed = m_Compressor->Compress(packetBuffer);
		DataBuffer encrypted = m_Encrypter->Encrypt(compressed);

		m_Socket->Send(encrypted);
	}

	template <typename T>
	void SendPacket(T* packet)
	{
		SendPacket(*packet);
	}
};

#endif
