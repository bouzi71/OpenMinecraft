#ifndef CLIENT_BLOCKPLACER_H_
#define CLIENT_BLOCKPLACER_H_

#include <mclib/core/Client.h>
#include <mclib/protocol/PacketHandler.h>
#include <mclib/world/World.h>

namespace example
{

class BlockPlacer 
	: public PacketHandler
	, public WorldListener
	, public ClientListener
{
public:
	BlockPlacer(PacketDispatcher* dispatcher, CMinecraftClient* client, PlayerController* pc, World* world);
	~BlockPlacer();

	void HandlePacket(in::WindowItemsPacket* packet);
	void HandlePacket(in::SetSlotPacket* packet);

	// ClientListener
	void OnTick();

private:
	CMinecraftClient* m_Client;
	PlayerController* m_PlayerController;
	World* m_World;
	glm::ivec3 m_Target;
	int64 m_LastUpdate;
	Slot m_HeldItem;
};

} // ns example

#endif
