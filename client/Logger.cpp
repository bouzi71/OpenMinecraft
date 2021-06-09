#include "Logger.h"

#include <mclib/util/Utility.h>

#include <iostream>

namespace example
{

Logger::Logger(CMinecraftClient* client, PacketDispatcher* dispatcher)
	: PacketHandler(dispatcher), m_Client(client)
{
	m_Client->RegisterListener(this);

	dispatcher->RegisterHandler(State::Login, login::Disconnect, this);

	dispatcher->RegisterHandler(State::Play, play::Chat, this);
	dispatcher->RegisterHandler(State::Play, play::Disconnect, this);
	dispatcher->RegisterHandler(State::Play, play::EntityLookAndRelativeMove, this);
	dispatcher->RegisterHandler(State::Play, play::BlockChange, this);
	dispatcher->RegisterHandler(State::Play, play::MultiBlockChange, this);
}

Logger::~Logger()
{
	GetDispatcher()->UnregisterHandler(this);
	m_Client->UnregisterListener(this);
}

void Logger::HandlePacket(in::ChatPacket* packet)
{
	std::string message = ParseChatNode(packet->GetChatData());

	if (!message.empty())
		std::cout << message << std::endl;

	if (message.find("!selected") != std::string::npos)
	{
		Slot item = m_Client->GetHotbar().GetCurrentItem();

		std::cout << "Selected item: " << item.GetItemId() << ":" << item.GetItemDamage() << " (" << item.GetItemCount() << ")" << std::endl;
	}
	else if (message.find("!select") != std::string::npos)
	{
		std::string amountStr = message.substr(message.find("!select ") + 8);
		int slotIndex = strtol(amountStr.c_str(), nullptr, 10);


		if (slotIndex >= 0 && slotIndex < 9)
		{
			m_Client->GetHotbar().SelectSlot(slotIndex);
		}
		else
		{
			std::cout << "Bad slot index." << std::endl;
		}
	}
	else if (message.find("!find ") != std::string::npos)
	{
		std::string toFind = message.substr(message.find("!find ") + 6);

		s32 itemId = strtol(toFind.c_str(), nullptr, 10);
		Inventory* inv = m_Client->GetInventoryManager()->GetPlayerInventory();
		if (inv)
		{
			bool contained = inv->Contains(itemId);

			std::cout << "Contains " << itemId << ": " << std::boolalpha << contained << std::endl;
		}
	}
}

void Logger::HandlePacket(in::EntityLookAndRelativeMovePacket* packet)
{
	Vector3d delta = ToVector3d(packet->GetDelta()) / (128.0 * 32.0);

	//std::cout << delta << std::endl;
}

void Logger::HandlePacket(in::BlockChangePacket* packet)
{
	Vector3i pos = packet->GetPosition();
	s32 blockId = packet->GetBlockId();

	std::cout << "Block changed at " << pos << " to " << blockId << std::endl;
}

void Logger::HandlePacket(in::MultiBlockChangePacket* packet)
{
	auto chunkX = packet->GetChunkX();
	auto chunkZ = packet->GetChunkZ();

	for (const auto& change : packet->GetBlockChanges())
	{
		Vector3i pos(chunkX + change.x, change.y + chunkZ + change.z);

		std::cout << "Block changed at " << pos << " to " << change.blockData << std::endl;
	}
}

void Logger::HandlePacket(login::in::DisconnectPacket* packet)
{
	std::wcout << L"Disconnected: " << packet->GetReason() << std::endl;
}

void Logger::OnTick()
{
	PlayerPtr player = m_Client->GetPlayerManager()->GetPlayerByName(L"testplayer");
	if (!player) return;

	EntityPtr entity = player->GetEntity();
	if (!entity) return;
}

} // ns example
