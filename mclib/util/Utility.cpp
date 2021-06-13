#include "stdafx.h"

#include <util/Utility.h>

#include <block/Block.h>
#include <common/DataBuffer.h>
#include <common/Json.h>
#include <core/Connection.h>
#include <core/PlayerManager.h>
#include <entity/EntityManager.h>
#include <protocol/Protocol.h>
#include <world/World.h>

#include <iostream>
#include <iomanip>

#include <sstream>
#include <limits>


#undef max
#undef min

#ifdef _WIN32
#include <ShlObj.h>
#else
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#endif


enum DyeColor
{
	InkSac = 0x1E1B1B,
	RoseRed = 0xB3312C,
	CactusGreen = 0x3B511A,
	CocoaBeans = 0x51301A,
	LapisLazuli = 0x253192,
	Purple = 0x7B2FBE,
	Cyan = 0x287697,
	LightGray = 0xABABAB,
	Gray = 0x434343,
	Pink = 0xD88198,
	Lime = 0x41CD34,
	DandelionYellow = 0xDECF2A,
	LightBlue = 0x6689D3,
	Magenta = 0xC354CD,
	Orange = 0xEB8844,
	BoneMeal = 0xF0F0F0,
};


#ifdef _WIN32

std::wstring GetAppDataPath()
{
	WCHAR* appdata = nullptr;
	if (::SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, NULL, &appdata) != S_OK)
		return false;

	std::wstring result(appdata);

	::CoTaskMemFree(appdata);

	return result;
}

#else

std::wstring GetAppDataPath()
{
	const char* home = nullptr;

	if ((home = getenv("HOME")) == nullptr)
		home = getpwuid(getuid())->pw_dir;

	return to_wstring(home);
}

#endif

bool GetProfileToken(const std::string& username, AuthToken* token)
{
	std::wstring appData = GetAppDataPath();

	const std::wstring filename = appData + L"/.minecraft/launcher_profiles.json";

	std::ifstream in(std::string(filename.begin(), filename.end()));

	if (!in.is_open())
		return false;

	json root;

	try
	{
		root = json::parse(in);
	}
	catch (json::parse_error& e)
	{
		std::cout << e.what() << std::endl;
		return false;
	}

	if (root.value("clientToken", json()).is_null() || root.value("authenticationDatabase", json()).is_null())
		return false;

	auto& authDB = root["authenticationDatabase"];

	std::string accessToken;
	std::string clientToken;

	clientToken = root["clientToken"].get<std::string>();

	for (auto& member : authDB.items())
	{
		auto& account = member.value();

		auto accessTokenNode = account.value("accessToken", json());
		if (accessTokenNode.is_null())
			continue;

		accessToken = accessTokenNode.get<std::string>();

		auto profilesNode = account.value("profiles", json());
		// Check if it's using Linux format
		if (profilesNode.is_null())
		{
			if (account.find("displayName") == account.end())
				continue;

			if (account.find("uuid") == account.end())
				continue;

			if (account["displayName"] != username)
				continue;

			*token = AuthToken(accessToken, clientToken, member.key());
			return true;
		}
		else
		{
			for (auto& kv : profilesNode.items())
			{
				std::string profileId = kv.key();
				auto& profile = kv.value();

				auto nameNode = profile.value("displayName", json());
				if (nameNode.is_null())
					continue;

				if (nameNode.get<std::string>() != username)
					continue;

				*token = AuthToken(accessToken, clientToken, profileId);
				return true;
			}
		}
	}

	return false;
}

int64 GetTime()
{
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

Slot CreateFirework(bool flicker, bool trail, uint8 type, uint8 duration, std::vector<int> colors, const std::string& name = "")
{
	NBT nbt;

	TagCompound* fireworks = new TagCompound(L"Fireworks");
	TagPtr flightTag(new TagByte("Flight", duration));

	fireworks->AddItem(TagType::Byte, flightTag);

	TagList* explosions = new TagList("Explosions", TagType::Compound);

	TagCompound* explosion = new TagCompound(L"Explosion");
	TagPtr flickerTag(new TagByte("Flicker", flicker ? 1 : 0));
	TagPtr trailTag(new TagByte("Trail", trail ? 1 : 0));
	TagPtr typeTag(new TagByte("Type", type));

	TagPtr colorsTag(new TagIntArray("Colors", colors));
	//TagPtr fadeColorsTag(new TagIntArray("FadeColors", colors));

	explosion->AddItem(TagType::Byte, flickerTag);
	explosion->AddItem(TagType::Byte, trailTag);
	explosion->AddItem(TagType::Byte, typeTag);
	explosion->AddItem(TagType::IntArray, colorsTag);

	explosions->AddItem(TagPtr(explosion));
	fireworks->AddItem(TagType::Compound, TagPtr(explosions));
	nbt.GetRoot().AddItem(TagType::Compound, TagPtr(fireworks));
	nbt.GetRoot().SetName(L"tag");

	if (!name.empty())
	{
		TagCompound* display = new TagCompound(L"display");
		TagPtr nameTag(new TagString("Name", name));

		display->AddItem(TagType::String, nameTag);

		nbt.GetRoot().AddItem(TagType::Compound, TagPtr(display));
	}

	Slot slot(401, 64, 0, nbt);

	return slot;
}

std::string ParseChatNode(const json& node)
{
	if (node.is_object())
	{
		auto translateNode = node.value("translate", json());

		if (translateNode.is_string())
		{
			std::string translate = translateNode.get<std::string>();
			std::string message;

			if (translate == "chat.type.text")
			{
				auto withNode = node.value("with", json());

				if (withNode.is_array())
				{
					for (auto iter = withNode.begin(); iter != withNode.end(); ++iter)
					{
						auto&& node = *iter;

						if (node.is_object())
						{
							auto&& textNode = node.value("text", json());
							if (textNode.is_string())
								message += "<" + textNode.get<std::string>() + "> ";
						}
						else if (node.is_string())
						{
							message += node.get<std::string>();
						}
					}
				}
			}

			return message;
		}
	}

	if (node.is_null()) return "";
	if (node.is_string()) return node.get<std::string>();
	if (node.is_object())
	{
		std::string result;

		auto extraNode = node.value("extra", json());
		auto textNode = node.value("text", json());

		if (!extraNode.is_null())
			result += ParseChatNode(extraNode);
		if (textNode.is_string())
			result += textNode.get<std::string>();
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
		std::cout << "Set health. health: " << packet->GetHealth() << "\n";
	}

	void HandlePacket(in::SetExperiencePacket* packet)
	{
		std::cout << "Set experience. Level: " << packet->GetLevel() << "\n";
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
			std::cout << message << "\n";
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

		std::cout << "Set slot (" << window << ", " << index << ") = " << slot.GetItemId() << ":" << slot.GetItemDamage() << "\n";
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

		std::cout << "Received world border packet\n";

		switch (action)
		{
			case WorldBorderPacket::Action::Initialize:
			{
				std::cout << "World border radius: " << (packet->GetDiameter() / 2.0) << "\n";
				std::cout << "World border center: " << packet->GetX() << ", " << packet->GetZ() << "\n";
				std::cout << "World border warning time: " << packet->GetWarningTime() << " seconds, blocks: " << packet->GetWarningBlocks() << " meters" << "\n";
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
		std::cout << "Spawn player " << player->GetName() << " at " << pos << "\n";
	}

	void OnPlayerJoin(PlayerPtr player)
	{
		std::cout << player->GetName() << L" added to playerlist" << "\n";
	}

	void OnPlayerLeave(PlayerPtr player)
	{
		std::cout << player->GetName() << L" removed from playerlist" << "\n";
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
		std::cout << "Plugin message received on channel " << packet->GetChannel();
		auto value = packet->GetData();
		std::cout << " Value: " << std::wstring(value.begin(), value.end()) << "\n";
	}

	void HandlePacket(in::ServerDifficultyPacket* packet)
	{
		std::cout << "New server difficulty: " << (int)packet->GetDifficulty() << "\n";
	}
};




class CreativeCreator
	: public PacketHandler
{
private:
	Connection* m_Connection;
	PlayerController* m_Controller;
	int16 m_Slot;
	Slot m_Item;
	bool m_Created;

	std::queue<Slot> m_CreateQueue;

public:
	CreativeCreator(PacketDispatcher* dispatcher, Connection* connection, PlayerController* controller, int16 slot, Slot item)
		: PacketHandler(dispatcher),
		m_Connection(connection), m_Controller(controller),
		m_Slot(slot), m_Item(item), m_Created(false)
	{
		dispatcher->RegisterHandler(State::Play, play::PlayerPositionAndLook, this);
		//dispatcher->RegisterHandler(State::Play, Play::SetSlot, this);
	}

	~CreativeCreator()
	{
		GetDispatcher()->UnregisterHandler(this);
	}

	void CreateItem()
	{
		out::CreativeInventoryActionPacket packet(m_Slot, m_Item);

		m_Connection->SendPacket(&packet);
	}

	void HandlePacket(in::SetSlotPacket* packet)
	{
		int index = packet->GetSlotIndex();
		Slot slot = packet->GetSlot();

		if (slot.GetItemId() > 0)
		{
			// Throw item
			using namespace out;

			PlayerDiggingPacket::Status status = PlayerDiggingPacket::Status::DropItemStack;
			glm::ivec3 pos(0, 0, 0);

			PlayerDiggingPacket packet(status, pos, Face::Bottom);

			m_Connection->SendPacket(&packet);


			// Create new item in its place now
			if (!m_CreateQueue.empty())
			{
				CreativeInventoryActionPacket packet(36, m_CreateQueue.front());
				m_CreateQueue.pop();
				m_Connection->SendPacket(&packet);
			}
		}
	}

	void HandlePacket(in::PlayerPositionAndLookPacket* packet)
	{
		if (m_Created) return;

		m_Created = true;

		std::async(std::launch::async, [&] {
			std::this_thread::sleep_for(std::chrono::seconds(7));

			std::vector<int> colors = {
				//DyeColor::InkSac,
				DyeColor::RoseRed,
				DyeColor::CactusGreen,
				//DyeColor::CocoaBeans,
				DyeColor::LapisLazuli,
				DyeColor::Purple,
				DyeColor::Cyan,
				DyeColor::LightGray,
				DyeColor::Gray,
				DyeColor::Pink,
				DyeColor::Lime,
				DyeColor::DandelionYellow,
				DyeColor::LightBlue,
				DyeColor::Magenta,
				DyeColor::Orange,
				DyeColor::BoneMeal
			};
			int slotIndex = 36;

			/* std::map<uint32, bool> usedMap;

			 for (u8 colorIndex1 = 0; colorIndex1 < colors.size(); ++colorIndex1) {
				 for (u8 colorIndex2 = 0; colorIndex2 < colors.size(); ++colorIndex2) {
					 for (u8 colorIndex3 = 0; colorIndex3 < colors.size(); ++colorIndex3) {
						 if (colorIndex1 == colorIndex2) continue;
						 if (colorIndex1 == colorIndex3) continue;
						 if (colorIndex2 == colorIndex3) continue;

						 std::vector<u8> sorted = { colorIndex1, colorIndex2, colorIndex3 };
						 std::sort(sorted.begin(), sorted.end());

						 uint32 combined = (sorted[0] << 16) | (sorted[1] << 8) | sorted[2];

						 if (usedMap.find(combined) != usedMap.end()) continue;

						 usedMap[combined] = true;

						 for (int type = 0; type < 5; ++type) {
							 Minecraft::Slot slot = CreateFirework(false, true, type, 2, std::vector<int> {
								 colors[colorIndex1],
								 colors[colorIndex2],
								 colors[colorIndex3]
							 });

							 m_CreateQueue.push(slot);
						 }
					 }
				 }
			 }
			 std::wcout << m_CreateQueue.size() << std::endl;
			 for (int colorIndex = 0; colorIndex < colors.size(); ++colorIndex) {
				 for (int type = 0; type < 5; ++type) {
					 Minecraft::Slot slot = CreateFirework(true, true, type, 1, std::vector<int>{colors[colorIndex]});

					 m_CreateQueue.push(slot);
				 }
			 }

			 if (!m_CreateQueue.empty()) {
				 Minecraft::Packets::Outbound::CreativeInventoryActionPacket packet(36, m_CreateQueue.front());
				 m_CreateQueue.pop();
				 m_Connection->SendPacket(&packet);
			 }*/
		});

	}
};



std::string StripChatMessage(const std::string& message)
{
	std::wstring utf16 = utf8to16(message);

	std::size_t pos = utf16.find((wchar_t)0xA7);

	while (pos != std::string::npos)
	{
		utf16.erase(pos, 2);
		pos = utf16.find((wchar_t)0xA7);
	}

	return utf16to8(utf16);
}
