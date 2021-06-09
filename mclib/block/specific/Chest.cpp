#include "stdafx.h"

#include <block/specific/Chest.h>

bool Chest::ImportNBT(NBT* nbt)
{
	auto nameTag = nbt->GetTag<TagString>(L"CustomName");
	if (nameTag)
	{
		this->SetCustomName(nameTag->GetValue());
	}

	return InventoryBlock::ImportNBT(nbt);
}
