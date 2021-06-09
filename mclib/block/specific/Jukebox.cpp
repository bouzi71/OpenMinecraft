#include "stdafx.h"

#include <block/specific/Jukebox.h>

bool Jukebox::ImportNBT(NBT* nbt)
{
	auto recordTag = nbt->GetTag<TagInt>(L"Record");
	auto itemTag = nbt->GetTag<TagCompound>(L"RecordItem");

	if (recordTag)
		m_RecordId = recordTag->GetValue();

	if (itemTag)
		m_RecordItem = Slot::FromNBT(*itemTag);

	return true;
}
