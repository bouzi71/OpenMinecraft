#include "stdafx.h"


// Additional
#include "core/PlayerManager.h"
#include "core/Connection.h"
#include "protocol/PacketDispatcher.h"

#if 0
class EventLogger
	: public PlayerListener
	, public ConnectionListener
	, public PacketHandler
{
private:
	PlayerManager* m_PlayerManager;
	Connection* m_Connection;

public:
	EventLogger(PacketDispatcher* dispatcher, PlayerManager* playerManager, Connection* connection)
		: PacketHandler(dispatcher),
		m_PlayerManager(playerManager),
		m_Connection(connection)
	{
		m_PlayerManager->RegisterListener(this);
		m_Connection->RegisterListener(this);

		dispatcher->RegisterHandler(State::Login, login::Disconnect, this);
		dispatcher->RegisterHandler(State::Login, login::EncryptionRequest, this);
		dispatcher->RegisterHandler(State::Login, login::LoginSuccess, this);

		dispatcher->RegisterHandler(State::Play, play::KeepAlive, this);
		dispatcher->RegisterHandler(State::Play, play::JoinGame, this);
		dispatcher->RegisterHandler(State::Play, play::Chat, this);
		dispatcher->RegisterHandler(State::Play, play::SpawnPosition, this);
		dispatcher->RegisterHandler(State::Play, play::UpdateHealth, this);
		dispatcher->RegisterHandler(State::Play, play::PlayerPositionAndLook, this);
		dispatcher->RegisterHandler(State::Play, play::SpawnPlayer, this);
		dispatcher->RegisterHandler(State::Play, play::SpawnMob, this);
		dispatcher->RegisterHandler(State::Play, play::EntityRelativeMove, this);
		dispatcher->RegisterHandler(State::Play, play::EntityMetadata, this);
		dispatcher->RegisterHandler(State::Play, play::SetExperience, this);
		dispatcher->RegisterHandler(State::Play, play::EntityProperties, this);
		dispatcher->RegisterHandler(State::Play, play::ChunkData, this);
		dispatcher->RegisterHandler(State::Play, play::SetSlot, this);
		dispatcher->RegisterHandler(State::Play, play::WindowItems, this);
		dispatcher->RegisterHandler(State::Play, play::Statistics, this);
		dispatcher->RegisterHandler(State::Play, play::PlayerAbilities, this);
		dispatcher->RegisterHandler(State::Play, play::PluginMessage, this);
		dispatcher->RegisterHandler(State::Play, play::Disconnect, this);
		dispatcher->RegisterHandler(State::Play, play::ServerDifficulty, this);
		dispatcher->RegisterHandler(State::Play, play::WorldBorder, this);
	}

	~EventLogger()
	{
		m_PlayerManager->UnregisterListener(this);
		m_Connection->UnregisterListener(this);
	}

	void HandlePacket(in::PlayerPositionAndLookPacket* packet)
	{

	}

	void HandlePacket(in::ChunkDataPacket* packet)
	{
		/*auto column = packet->GetChunkColumn();
		auto x = column->GetMetadata().x;
		auto z = column->GetMetadata().z;

		std::wcout << "Received chunk data for chunk " << x << ", " << z << std::endl;*/
	}

	void HandlePacket(in::EntityPropertiesPacket* packet)
	{
		/*  std::wcout << "Received entity properties: " << std::endl;
		const auto& properties = packet->GetProperties();
		for (const auto& kv : properties) {
		std::wstring key = kv.first;
		const auto& property = kv.second;
		std::wcout << key << " : " << property.value << std::endl;
		for (const auto& modifier : property.modifiers)
		std::wcout << "Modifier: " << modifier.uuid << " " << modifier.amount << " " << (int)modifier.operation << std::endl;
		}*/
	}

	void HandlePacket(in::EntityRelativeMovePacket* packet)
	{

	}

	void HandlePacket(in::SpawnPlayerPacket* packet)
	{
		/*float x, y, z;
		x = packet->GetX();
		y = packet->GetY();
		z = packet->GetZ();
		m_Out << "Spawn player " << packet->GetUUID() << " at (" << x << ", " << y << ", " << z << ")" << std::endl;*/
	}

	void HandlePacket(in::UpdateHealthPacket* packet)
	{
		Log::Print("Set health. health: %f", packet->GetHealth());
	}

	void HandlePacket(in::SetExperiencePacket* packet)
	{
		Log::Print("Set experience. Level: %d", packet->GetLevel());
	}

	std::string ParseChatNode(const json& node)
	{
		if (node.is_null()) return "";
		if (node.is_string()) return node.get<std::string>();
		if (node.is_object())
		{
			std::string result;

			if (!node["extra"].is_null())
				result += ParseChatNode(node["extra"]);
			if (node["text"].is_string())
				result += node["text"].get<std::string>();
			return result;
		}
		if (node.is_array())
		{
			std::string result;

			for (auto arrayNode : node)
				result += ParseChatNode(arrayNode);
			return result;
		}
		return "";
	}

	void HandlePacket(in::ChatPacket* packet)
	{
		const json& root = packet->GetChatData();

		std::string message = ParseChatNode(root);
		std::size_t pos = message.find((char)0xA7);

		while (pos != std::string::npos)
		{
			message.erase(pos - 1, 3);
			pos = message.find((char)0xA7);
		}

		message.erase(std::remove_if(message.begin(), message.end(), [](char c) {
			return c < 32 || c > 126;
		}), message.end());

		if (message.length() > 0)
			Log::Print("Chat: %s", message.c_str());
	}

	void HandlePacket(in::EntityMetadataPacket* packet)
	{
		//const auto& metadata = packet->GetMetadata();

		//std::wcout << "Received entity metadata" << std::endl;
	}

	void HandlePacket(in::SpawnMobPacket* packet)
	{
		//const auto& metadata = packet->GetMetadata();

		//std::wcout << "Received SpawnMobPacket" << std::endl;
	}

	void HandlePacket(in::SetSlotPacket* packet)
	{
		Slot slot = packet->GetSlot();
		int window = packet->GetWindowId();
		int index = packet->GetSlotIndex();

		Log::Print("Set slot (%d, %d) = %d:%d", window, index, slot.GetItemId(), slot.GetItemDamage());
	}

	void HandlePacket(in::WindowItemsPacket* packet)
	{
		std::cout << "Received window items for WindowId " << (int)packet->GetWindowId() << "." << "\n";

		const std::vector<Slot>& slots = packet->GetSlots();

		for (const Slot& slot : slots)
		{
			int16 id = slot.GetItemId();
			uint8 count = slot.GetItemCount();
			int16 dmg = slot.GetItemDamage();
			//const Minecraft::NBT::NBT& nbt = slot.GetNBT();

			if (id != -1)
				std::cout << "Item: " << id << " Amount: " << (int)count << " Dmg: " << dmg << "\n";
		}
	}

	void HandlePacket(in::WorldBorderPacket* packet)
	{
		using namespace in;
		WorldBorderPacket::Action action = packet->GetAction();

		Log::Print("Received world border packet");

		switch (action)
		{
			case WorldBorderPacket::Action::Initialize:
			{
				Log::Print("World border radius: %d", (packet->GetDiameter() / 2.0));
				Log::Print("World border center: %f, %f", packet->GetX(), packet->GetZ());
				Log::Print("World border warning time: %d seconds, blocks: %d meters", packet->GetWarningTime(), packet->GetWarningBlocks());
			}
			break;
			default:
				break;
		}
	}

	void OnPlayerSpawn(PlayerPtr player)
	{
		auto entity = player->GetEntity();
		glm::dvec3 pos(0, 0, 0);

		if (entity)
			pos = entity->GetPosition();

		Log::Print("Spawn player %s at (%f, %f, %f)", utf16to8(player->GetName()).c_str(), pos.x, pos.y, pos.z);
	}

	void OnPlayerJoin(PlayerPtr player)
	{
		Log::Print("%s  added to playerlist.", utf16to8(player->GetName()).c_str());
	}

	void OnPlayerLeave(PlayerPtr player)
	{
		Log::Print("%s  removed from playerlist.", utf16to8(player->GetName()).c_str());
	}

	void HandlePacket(in::StatisticsPacket* packet)
	{
		const auto& stats = packet->GetStatistics();

		for (auto& kv : stats)
			std::cout << kv.first << " = " << kv.second << "\n";
	}

	void HandlePacket(in::PlayerAbilitiesPacket* packet)
	{
		std::cout << "Abilities: " << (int)packet->GetFlags() << ", " << packet->GetFlyingSpeed() << ", " << packet->GetFOVModifier() << "\n";
	}

	void HandlePacket(in::SpawnPositionPacket* packet)
	{
		std::cout << "Spawn position: " << packet->GetLocation() << "\n";
	}

	void HandlePacket(login::in::DisconnectPacket* packet)
	{
		std::cout << "Disconnected from server. Reason: " << "\n";
		std::cout << packet->GetReason() << "\n";
	}

	void HandlePacket(login::in::EncryptionRequestPacket* packet)
	{
		std::cout << "Encryption request received\n";
	}

	void HandlePacket(login::in::LoginSuccessPacket* packet)
	{
		std::cout << "Successfully logged in. Username: ";
		std::cout << packet->GetUsername() << "\n";
	}

	void HandlePacket(in::JoinGamePacket* packet)
	{
		std::cout << "Joining game with entity id of " << packet->GetEntityId() << "\n";
		std::cout << "Game difficulty: " << (int)packet->GetDifficulty() << ", Dimension: " << (int)packet->GetDimension() << "\n";
		std::cout << "Level type: " << packet->GetLevelType() << "\n";
	}

	void HandlePacket(in::PluginMessagePacket* packet)
	{
		Log::Print("Plugin message received on channel '%s'", utf16to8(packet->GetChannel()).c_str());
		
		auto value = packet->GetData();
		Log::Print(" Value: '%s'", value.c_str());
	}

	void HandlePacket(in::ServerDifficultyPacket* packet)
	{
		std::cout << "New server difficulty: " << (int)packet->GetDifficulty() << "\n";
	}
};

#endif
