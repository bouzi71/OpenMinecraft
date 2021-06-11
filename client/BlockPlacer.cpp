#include "BlockPlacer.h"

#include <mclib/util/Utility.h>

#include <iostream>

namespace example
{

BlockPlacer::BlockPlacer(PacketDispatcher* dispatcher, CMinecraftClient* client, PlayerController* pc, World* world)
	: PacketHandler(dispatcher),
	m_Client(client),
	m_PlayerController(pc),
	m_World(world),
	m_LastUpdate(GetTime())
{
	m_Target = glm::ivec3(-2, 62, 275);
	world->RegisterListener(this);
	client->RegisterListener(this);

	dispatcher->RegisterHandler(State::Play, play::WindowItems, this);
	dispatcher->RegisterHandler(State::Play, play::SetSlot, this);
}

BlockPlacer::~BlockPlacer()
{
	GetDispatcher()->UnregisterHandler(this);
	m_World->UnregisterListener(this);
	m_Client->UnregisterListener(this);
}

void BlockPlacer::HandlePacket(in::WindowItemsPacket* packet)
{
	auto slots = packet->GetSlots();
	m_HeldItem = slots[36];
}

void BlockPlacer::HandlePacket(in::SetSlotPacket* packet)
{
	if (packet->GetWindowId() != 0) return;

	if (packet->GetSlotIndex() == 36)
		m_HeldItem = packet->GetSlot();
}

void BlockPlacer::OnTick()
{
	int64 time = GetTime();
	if (time - m_LastUpdate < 5000) return;
	m_LastUpdate = time;

	if (m_PlayerController->GetPosition() == glm::dvec3(0, 0, 0)) return;
	if (!m_World->GetChunk(m_Target)) return;

	m_PlayerController->LookAt(ToVector3d(m_Target));

	if (m_HeldItem.GetItemId() != -1)
	{
		Block* block = m_World->GetBlock(m_Target + glm::ivec3(0, 1, 0));

		if (!block || block->GetID() == 0)
		{
			out::PlayerBlockPlacementPacket blockPlacePacket(m_Target, Face::Top, Hand::Main, glm::vec3(0.5, 0, 0.5));

			m_Client->GetConnection()->SendPacket(&blockPlacePacket);
			std::wcout << "Placing block" << std::endl;
		}
		else
		{
			using namespace out;
			{
				PlayerDiggingPacket::Status status = PlayerDiggingPacket::Status::StartedDigging;
				PlayerDiggingPacket packet(status, m_Target + glm::ivec3(0, 1, 0), Face::West);

				m_Client->GetConnection()->SendPacket(&packet);
			}

			std::wcout << "Destroying block" << std::endl;

			{
				PlayerDiggingPacket::Status status = PlayerDiggingPacket::Status::FinishedDigging;
				PlayerDiggingPacket packet(status, m_Target + glm::ivec3(0, 1, 0), Face::West);

				m_Client->GetConnection()->SendPacket(&packet);
			}
		}
	}
}

} // ns example
