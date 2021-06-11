#ifndef MCLIB_INVENTORY_INVENTORY_H_
#define MCLIB_INVENTORY_INVENTORY_H_

#include <core/Connection.h>
#include <inventory/Slot.h>
#include <protocol/PacketHandler.h>



class Inventory
{
public:
	static const MCLIB_API int32 HOTBAR_SLOT_START;
	static const MCLIB_API int32 PLAYER_INVENTORY_ID;

	using ItemMap = std::map<int32, Slot>;

private:
	ItemMap m_Items;
	int m_WindowId;
	int16 m_CurrentAction;
	Slot m_Cursor;

	MCLIB_API void HandleTransaction(Connection& conn, uint16 action, bool accepted);

public:
	MCLIB_API Inventory(int windowId);

	MCLIB_API Slot GetItem(int32 index) const;
	MCLIB_API const ItemMap& GetItems() const noexcept { return m_Items; }

	const Slot& GetCursorItem() const { return m_Cursor; }

	// Returns item slot index. Returns -1 if none are found.
	MCLIB_API int32 FindItemById(int32 itemId) const;

	MCLIB_API bool Contains(int32 itemId) const;
	MCLIB_API bool Contains(Slot item) const;
	MCLIB_API bool ContainsAtLeast(int32 itemId, int32 amount) const;
	MCLIB_API bool ContainsAtLeast(Slot item, int32 amount) const;

	// Moves an item to the cursor. It will fail if something is already on cursor or if target slot is empty.
	MCLIB_API bool PickUp(Connection& conn, int32 index);
	// Place the current cursor item into a slot. 
	// The server may set the new cursor to the item in the target slot.
	MCLIB_API bool Place(Connection& conn, int32 index);

	friend class InventoryManager;
};

class InventoryManager : public PacketHandler
{
private:
	Connection* m_Connection;
	std::map<int32, std::unique_ptr<Inventory>> m_Inventories;

	void SetSlot(int32 windowId, int32 slotIndex, const Slot& slot);

public:
	MCLIB_API InventoryManager(PacketDispatcher* dispatcher, Connection* connection);
	MCLIB_API ~InventoryManager();

	MCLIB_API void HandlePacket(in::SetSlotPacket* packet);
	MCLIB_API void HandlePacket(in::WindowItemsPacket* packet);
	MCLIB_API void HandlePacket(in::OpenWindowPacket* packet);
	MCLIB_API void HandlePacket(in::ConfirmTransactionPacket* packet);

	MCLIB_API Inventory* GetInventory(int32 windowId);
	MCLIB_API Inventory* GetPlayerInventory();
};

#endif
