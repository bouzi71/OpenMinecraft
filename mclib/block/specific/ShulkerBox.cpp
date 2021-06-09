#include "stdafx.h"

#include <block/specific/ShulkerBox.h>

bool ShulkerBox::ImportNBT(NBT* nbt)
{
	auto nameTag = nbt->GetTag<TagString>(L"CustomName");
	if (nameTag)
	{
		this->SetCustomName(nameTag->GetValue());
	}

	return InventoryBlock::ImportNBT(nbt);
}
