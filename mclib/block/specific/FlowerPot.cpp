#include "stdafx.h"

#include <block/specific/FlowerPot.h>

bool FlowerPot::ImportNBT(NBT* nbt)
{
	auto itemTag = nbt->GetTag<TagString>(L"Item");
	if (itemTag)
		m_ItemId = itemTag->GetValue();

	auto dataTag = nbt->GetTag<TagInt>(L"Data");
	if (dataTag)
		m_ItemData = dataTag->GetValue();

	return true;
}
