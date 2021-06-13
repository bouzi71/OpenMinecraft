#ifndef MCLIB_INVENTORY_HOTBAR_H_
#define MCLIB_INVENTORY_HOTBAR_H_

#include <core/Connection.h>
#include <inventory/Slot.h>
#include <protocol/PacketHandler.h>

class InventoryManager;

class Hotbar 
	: public PacketHandler
{
public:
	MCLIB_API Hotbar(PacketDispatcher* dispatcher, Connection* connection, InventoryManager* inventoryManager);
	MCLIB_API ~Hotbar();

	Hotbar(const Hotbar& rhs) = delete;
	Hotbar& operator=(const Hotbar& rhs) = delete;
	Hotbar(Hotbar&& rhs) = delete;
	Hotbar& operator=(Hotbar&& rhs) = delete;

	MCLIB_API void HandlePacket(in::HeldItemChangePacket* packet);

	// Sends a packet telling the server that the client is changing slots.
	MCLIB_API void SelectSlot(int32 hotbarIndex); // Slot should be between 0 and 8.
	MCLIB_API int32 GetSelectedSlot() const noexcept { return m_SelectedSlot; }

	MCLIB_API Slot GetCurrentItem();

	MCLIB_API Slot GetItem(int32 hotbarIndex); // Index should be between 0 and 8.

private:
	Connection* m_Connection;
	InventoryManager* m_InventoryManager;
	int32 m_SelectedSlot;
};

#endif
