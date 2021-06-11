#include "stdafx.h"

#include <core/Connection.h>

#include <common/DataBuffer.h>
#include <core/Compression.h>
#include <core/Encryption.h>
#include <network/Network.h>
#include <network/TCPSocket.h>
#include <protocol/PacketDispatcher.h>
#include <protocol/PacketFactory.h>
#include <util/Utility.h>

#include <future>
#include <thread>


Connection::Connection(PacketDispatcher* dispatcher, Version version)
	: PacketHandler(dispatcher),
	m_Encrypter(std::make_unique<EncryptionStrategyNone>()),
	m_Compressor(std::make_unique<CompressionNone>()),
	m_Socket(std::make_unique<TCPSocket>()),
	m_Yggdrasil(std::make_unique<Yggdrasil>()),
	m_Protocol(Protocol::GetProtocol(version)),
	m_SentSettings(false),
	m_Dimension(1)
{
	dispatcher->RegisterHandler(State::Login, login::Disconnect, this);
	dispatcher->RegisterHandler(State::Login, login::EncryptionRequest, this);
	dispatcher->RegisterHandler(State::Login, login::LoginSuccess, this);
	dispatcher->RegisterHandler(State::Login, login::SetCompression, this);

	dispatcher->RegisterHandler(State::Status, status::Response, this);

	dispatcher->RegisterHandler(State::Play, play::KeepAlive, this);
	dispatcher->RegisterHandler(State::Play, play::PlayerPositionAndLook, this);
	dispatcher->RegisterHandler(State::Play, play::JoinGame, this);
	dispatcher->RegisterHandler(State::Play, play::Respawn, this);
	dispatcher->RegisterHandler(State::Play, play::UpdateHealth, this);
	dispatcher->RegisterHandler(State::Play, play::Disconnect, this);
}

Connection::~Connection()
{
	GetDispatcher()->UnregisterHandler(this);
}

Socket::Status Connection::GetSocketState() const
{
	return m_Socket->GetStatus();
}

void Connection::HandlePacket(in::JoinGamePacket* packet)
{
	m_Dimension = packet->GetDimension();
}

void Connection::HandlePacket(in::RespawnPacket* packet)
{
	m_Dimension = packet->GetDimension();
}

void Connection::HandlePacket(in::KeepAlivePacket* packet)
{
	out::KeepAlivePacket response(packet->GetAliveId());
	SendPacket(&response);
}

void Connection::HandlePacket(in::PlayerPositionAndLookPacket* packet)
{
	// Used to verify position
	out::TeleportConfirmPacket confirmation(packet->GetTeleportId());
	SendPacket(&confirmation);

	out::PlayerPositionAndLookPacket response(packet->GetPosition(), packet->GetYaw(), packet->GetPitch(), true);
	SendPacket(&response);
}

void Connection::HandlePacket(in::UpdateHealthPacket* packet)
{
	if (packet->GetHealth() <= 0)
	{
		out::ClientStatusPacket status(out::ClientStatusPacket::Action::PerformRespawn);
		SendPacket(&status);
	}
}

void Connection::HandlePacket(login::in::DisconnectPacket* packet)
{
	m_Socket->Disconnect();

	NotifyListeners(&ConnectionListener::OnSocketStateChange, m_Socket->GetStatus());

	if (m_ProtocolState != State::Play)
		NotifyListeners(&ConnectionListener::OnLogin, false);
}

void Connection::AuthenticateClient(const std::wstring& serverId, const std::string& sharedSecret, const std::string& pubkey)
{
	bool success = true;
	std::string error = "";

	if (false == m_Yggdrasil->IsAuthenticated())
	{
		try
		{
			if (false == m_Yggdrasil->Authenticate(m_Username, m_Password))
			{
				error = "Failed to authenticate";
				success = false;
			}
		}
		catch (const YggdrasilException& e)
		{
			error = e.what();
			success = false;
		}
	}

	try
	{
		if (!m_Yggdrasil->JoinServer(serverId, sharedSecret, pubkey))
		{
			success = false;
			error = "Failed to join server through Yggdrasil.";
		}
	}
	catch (const YggdrasilException& e)
	{
		success = false;
		error = e.what();
	}

	m_Password.clear();
	NotifyListeners(&ConnectionListener::OnAuthentication, success, error);
}

void Connection::HandlePacket(login::in::EncryptionRequestPacket* packet)
{
	std::string pubkey = packet->GetPublicKey();
	std::string verify = packet->GetVerifyToken();

	auto aesEncrypter = std::make_unique<EncryptionStrategyAES>(pubkey, verify);
	login::out::EncryptionResponsePacket* encResp = aesEncrypter->GenerateResponsePacket();

	AuthenticateClient(packet->GetServerId().GetUTF16(), aesEncrypter->GetSharedSecret(), pubkey);

	SendPacket(encResp);

	m_Encrypter = std::move(aesEncrypter);
}

void Connection::SendSettingsPacket()
{
	out::ClientSettingsPacket clientSettings(
		m_ClientSettings.GetLocale(),
		m_ClientSettings.GetViewDistance(),
		m_ClientSettings.GetChatMode(),
		m_ClientSettings.GetChatColors(),
		m_ClientSettings.GetSkinParts(),
		m_ClientSettings.GetMainHand()
	);
	SendPacket(&clientSettings);

	m_SentSettings = true;
}

void Connection::HandlePacket(login::in::LoginSuccessPacket* packet)
{
	m_ProtocolState = State::Play;

	NotifyListeners(&ConnectionListener::OnLogin, true);
}

void Connection::HandlePacket(login::in::SetCompressionPacket* packet)
{
	m_Compressor = std::make_unique<CompressionZ>(packet->GetMaxPacketSize());
}

bool Connection::Connect(const std::string& server, uint16 port)
{
	bool result = false;

	m_Socket = std::make_unique<TCPSocket>();
	m_Yggdrasil = std::unique_ptr<Yggdrasil>(new Yggdrasil());
	m_ProtocolState = State::Handshake;


	m_Compressor = std::make_unique<CompressionNone>();
	m_Encrypter = std::make_unique<EncryptionStrategyNone>();

	m_Server = server;
	m_Port = port;

	if (::isdigit(m_Server.at(0)))
	{
		IPAddress addr(m_Server);

		result = m_Socket->Connect(addr, m_Port);
	}
	else
	{
		auto addrs = Dns::Resolve(m_Server);
		if (addrs.size() == 0) return false;

		for (auto addr : addrs)
		{
			if (m_Socket->Connect(addr, m_Port))
			{
				result = true;
				break;
			}
		}
	}

	m_Socket->SetBlocking(false);

	if (result)
		NotifyListeners(&ConnectionListener::OnSocketStateChange, m_Socket->GetStatus());

	return result;
}

void Connection::Disconnect()
{
	m_Socket->Disconnect();
	NotifyListeners(&ConnectionListener::OnSocketStateChange, m_Socket->GetStatus());
}

CMinecraftPacket* Connection::CreatePacket(DataBuffer& buffer)
{
	std::size_t readOffset = buffer.GetReadOffset();
	CVarInt length;

	try
	{
		buffer >> length;
	}
	catch (const std::out_of_range&)
	{
		// This will happen when the buffer only contains part of the CVarInt, 
		// so only part of the packet was received so far.
		// The buffer read offset isn't advanced when the exception is thrown, so no need to set it back to what it was.
		return nullptr;
	}

	if (length.GetInt() == 0 || buffer.GetRemaining() < (uint32)length.GetInt())
	{
		// Reset the read offset back to what it was because the full packet hasn't been received yet.
		buffer.SetReadOffset(readOffset);
		return nullptr;
	}

	DataBuffer decompressed = m_Compressor->Decompress(buffer, length.GetInt());

	return PacketFactory::CreatePacket(m_Protocol, m_ProtocolState, decompressed, length.GetInt(), this);
}

void Connection::CreatePacket()
{
	while (true)
	{
		DataBuffer buffer;

		m_Socket->Receive(buffer, 4096);

		if (buffer.IsEmpty())
		{
			if (m_Socket->GetStatus() != Socket::Connected)
			{
				NotifyListeners(&ConnectionListener::OnSocketStateChange, m_Socket->GetStatus());
			}
			return;
		}

		m_HandleBuffer << m_Encrypter->Decrypt(buffer);

		do
		{
			try
			{
				CMinecraftPacket* packet = CreatePacket(m_HandleBuffer);

				if (packet)
				{
					// Only send the settings after the server has accepted the new protocol state.
					if (!m_SentSettings && packet->GetProtocolState() == State::Play)
					{
						SendSettingsPacket();
					}

					this->GetDispatcher()->Dispatch(packet);
					PacketFactory::FreePacket(packet);
				}
				else
				{
					break;
				}
			}
			catch (const UnfinishedProtocolException&)
			{
				// Ignore for now
			}
		} while (false == m_HandleBuffer.IsFinished() && m_HandleBuffer.GetSize() > 0);

		if (m_HandleBuffer.IsFinished())
		{
			m_HandleBuffer = DataBuffer();
		}
		else if (m_HandleBuffer.GetReadOffset() != 0)
		{
			m_HandleBuffer = DataBuffer(m_HandleBuffer, m_HandleBuffer.GetReadOffset());
		}

		if (m_Socket->GetStatus() != Socket::Connected)
		{
			NotifyListeners(&ConnectionListener::OnSocketStateChange, m_Socket->GetStatus());
		}
	}
}

bool Connection::Login(const std::string& username, const std::string& password)
{
	static const std::string fml("\0FML\0", 5);

	if (m_Socket->GetStatus() != Socket::Status::Connected)
	{
		return false;
	}

	login::out::HandshakePacket handshake(static_cast<int32>(m_Protocol.GetVersion()), m_Server + fml, m_Port, State::Login);
	SendPacket(&handshake);

	m_ProtocolState = State::Login;

	m_Email = username;

	if (username.find("@") != std::string::npos)
	{
		m_Yggdrasil->Authenticate(username, password);

		m_Username = m_Yggdrasil->GetPlayerName();
	}
	else
	{
		m_Username = username;
	}

	m_Password = password;

	login::out::LoginStartPacket loginStart(m_Username);
	SendPacket(&loginStart);

	return true;
}

bool Connection::Login(const std::string& username, AuthToken token)
{
	static const std::string fml("\0FML\0", 5);

	if (m_Socket->GetStatus() != Socket::Status::Connected)
		return false;

	if (false == token.IsValid())
	{
		if (false == token.Validate(username))
			return false;
	}

	m_Yggdrasil = std::move(token.GetYggdrasil());

	login::out::HandshakePacket handshake(static_cast<int32>(m_Protocol.GetVersion()), m_Server + fml, m_Port, State::Login);
	SendPacket(&handshake);

	m_ProtocolState = State::Login;

	m_Username = username;

	login::out::LoginStartPacket loginStart(m_Username);
	SendPacket(&loginStart);

	return true;
}

void Connection::HandlePacket(status::in::ResponsePacket* packet)
{
	std::string response = to_string(packet->GetResponse());

	json data;

	try
	{
		data = json::parse(response);
	}
	catch (json::parse_error&)
	{

	}

	NotifyListeners(&ConnectionListener::OnPingResponse, data);
}

void Connection::Ping()
{
	std::string fml("\0FML\0", 5);

	login::out::HandshakePacket handshake(static_cast<int32>(m_Protocol.GetVersion()), m_Server + fml, m_Port, State::Status);
	SendPacket(&handshake);

	m_ProtocolState = State::Status;

	status::out::RequestPacket request;
	SendPacket(&request);
}
