#include "stdafx.h"

#include <block/specific/Furnace.h>

bool Furnace::ImportNBT(NBT* nbt)
{
	auto nameTag = nbt->GetTag<TagString>(L"CustomName");

	if (nameTag)
	{
		this->SetCustomName(nameTag->GetValue());
	}

	auto burnTimeTag = nbt->GetTag<TagShort>(L"BurnTime");
	if (burnTimeTag)
		m_BurnTime = burnTimeTag->GetValue();

	auto cookTimeTag = nbt->GetTag<TagShort>(L"CookTime");
	if (cookTimeTag)
		m_CookTime = cookTimeTag->GetValue();

	auto cookTimeTotalTag = nbt->GetTag<TagShort>(L"CookTimeTotal");
	if (cookTimeTotalTag)
		m_CookTimeTotal = cookTimeTotalTag->GetValue();

	return InventoryBlock::ImportNBT(nbt);
}

Slot Furnace::GetSmeltingSlot() const noexcept
{
	const auto& items = GetItems();
	auto iter = items.find(0);
	if (iter == items.end()) return Slot();
	return iter->second;
}

Slot Furnace::GetFuelSlot() const noexcept
{
	const auto& items = GetItems();
	auto iter = items.find(1);
	if (iter == items.end()) return Slot();
	return iter->second;
}

Slot Furnace::GetResultSlot() const noexcept
{
	const auto& items = GetItems();
	auto iter = items.find(2);
	if (iter == items.end()) return Slot();
	return iter->second;
}
