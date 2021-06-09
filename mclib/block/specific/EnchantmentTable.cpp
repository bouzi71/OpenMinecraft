#include "stdafx.h"

#include <block/specific/EnchantmentTable.h>

bool EnchantmentTable::ImportNBT(NBT* nbt)
{
	auto nameTag = nbt->GetTag<TagString>(L"CustomName");
	if (nameTag)
	{
		this->SetCustomName(nameTag->GetValue());
	}

	return true;
}
