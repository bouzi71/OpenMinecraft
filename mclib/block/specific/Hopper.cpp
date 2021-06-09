#include "stdafx.h"

#include <block/specific/Hopper.h>

bool Hopper::ImportNBT(NBT* nbt)
{
	auto nameTag = nbt->GetTag<TagString>(L"CustomName");
	if (nameTag)
	{
		this->SetCustomName(nameTag->GetValue());
	}

	auto transferCooldownTag = nbt->GetTag<TagInt>(L"TransferCooldown");
	if (transferCooldownTag)
		m_TransferCooldown = transferCooldownTag->GetValue();

	return InventoryBlock::ImportNBT(nbt);
}
