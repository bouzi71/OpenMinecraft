#include "stdafx.h"

#include <inventory/Hotbar.h>

#include <inventory/Inventory.h>
#include <protocol/PacketDispatcher.h>

Hotbar::Hotbar(PacketDispatcher* dispatcher, Connection* connection, InventoryManager* inventoryManager)
	: PacketHandler(dispatcher)
	, m_Connection(connection)
	, m_InventoryManager(inventoryManager)
	, m_SelectedSlot(0)
{
	dispatcher->RegisterHandler(State::Play, play::HeldItemChange, this);
}

Hotbar::~Hotbar()
{
	GetDispatcher()->UnregisterHandler(this);
}

void Hotbar::SelectSlot(int32 hotbarIndex)
{
	m_SelectedSlot = hotbarIndex;

	out::HeldItemChangePacket heldItemPacket(hotbarIndex);
	m_Connection->SendPacket(&heldItemPacket);
}

void Hotbar::HandlePacket(in::HeldItemChangePacket* packet)
{
	m_SelectedSlot = packet->GetSlot();
}

Slot Hotbar::GetCurrentItem()
{
	return GetItem(m_SelectedSlot);
}

Slot Hotbar::GetItem(int32 hotbarIndex)
{
	Inventory* inventory = m_InventoryManager->GetPlayerInventory();
	if (inventory == nullptr) return Slot();

	return inventory->GetItem(Inventory::HOTBAR_SLOT_START + hotbarIndex);
}
