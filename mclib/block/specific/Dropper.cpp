#include "stdafx.h"

#include <block/specific/Dropper.h>

bool Dropper::ImportNBT(NBT* nbt)
{
	auto nameTag = nbt->GetTag<TagString>(L"CustomName");
	if (nameTag)
	{
		this->SetCustomName(nameTag->GetValue());
	}

	return InventoryBlock::ImportNBT(nbt);
}
