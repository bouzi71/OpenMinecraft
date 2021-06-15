#include "stdafx.h"


// Additional
#include "core/Connection.h"
#include "protocol/PacketHandler.h"
#include "world/PlayerController.h"

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