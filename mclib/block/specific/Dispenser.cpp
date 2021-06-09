#include "stdafx.h"

#include <block/specific/Dispenser.h>

bool Dispenser::ImportNBT(NBT* nbt)
{
	auto nameTag = nbt->GetTag<TagString>(L"CustomName");
	if (nameTag)
	{
		this->SetCustomName(nameTag->GetValue());
	}

	return InventoryBlock::ImportNBT(nbt);
}
