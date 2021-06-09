#include "stdafx.h"

#include <block/specific/BrewingStand.h>

bool BrewingStand::ImportNBT(NBT* nbt)
{
	auto nameTag = nbt->GetTag<TagString>(L"CustomName");
	if (nameTag)
		this->SetCustomName(nameTag->GetValue());

	auto brewTimeTag = nbt->GetTag<TagInt>(L"BrewTime");
	if (brewTimeTag)
		m_BrewTime = brewTimeTag->GetValue();

	auto fuelTag = nbt->GetTag<TagByte>(L"Fuel");
	if (fuelTag)
		m_Fuel = fuelTag->GetValue();

	return InventoryBlock::ImportNBT(nbt);
}

Slot BrewingStand::GetLeftSlot() const noexcept
{
	const auto& items = GetItems();
	auto iter = items.find(0);
	if (iter == items.end()) 
		return Slot();

	return iter->second;
}

Slot BrewingStand::GetMiddleSlot() const noexcept
{
	const auto& items = GetItems();
	auto iter = items.find(1);
	if (iter == items.end()) 
		return Slot();

	return iter->second;
}

Slot BrewingStand::GetRightSlot() const noexcept
{
	const auto& items = GetItems();
	auto iter = items.find(2);
	if (iter == items.end()) 
		return Slot();

	return iter->second;
}

Slot BrewingStand::GetIngredientSlot() const noexcept
{
	const auto& items = GetItems();
	auto iter = items.find(3);
	if (iter == items.end()) 
		return Slot();

	return iter->second;
}

Slot BrewingStand::GetFuelSlot() const noexcept
{
	const auto& items = GetItems();
	auto iter = items.find(4);
	if (iter == items.end()) 
		return Slot();

	return iter->second;
}
