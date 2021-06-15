#include "stdafx.h"

// General
#include "InventoryBlock.h"

bool InventoryBlock::ImportNBT(NBT* nbt)
{
	auto lockTag = nbt->GetTag<TagString>(L"Lock");
	auto lootTableTag = nbt->GetTag<TagString>(L"LootTable");
	auto lootTableSeedTag = nbt->GetTag<TagInt>(L"LootTableSeed");

	if (lockTag)
		m_Lock = lockTag->GetValue();

	if (lootTableTag)
		m_LootTable = lootTableTag->GetValue();

	if (lootTableSeedTag)
		m_LootTableSeed = lootTableSeedTag->GetValue();

	auto itemsTag = nbt->GetTag<TagList>(L"Items");
	if (itemsTag == nullptr) return false;

	for (auto iter = itemsTag->begin(); iter != itemsTag->end(); ++iter)
	{
		TagCompound* itemTag = (TagCompound*)iter->get();

		auto slotTag = itemTag->GetTag<TagByte>(L"Slot");
		if (!slotTag) return false;

		Slot item = Slot::FromNBT(*itemTag);
		m_Items.insert(std::make_pair(slotTag->GetValue(), item));
	}

	return true;
}
