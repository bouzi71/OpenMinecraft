#include "stdafx.h"

#include <block/specific/Beacon.h>

bool Beacon::ImportNBT(NBT* nbt)
{
	auto levelsTag = nbt->GetTag<TagInt>(L"Levels");
	if (levelsTag)
		m_Levels = levelsTag->GetValue();

	auto primaryTag = nbt->GetTag<TagInt>(L"Primary");
	if (primaryTag)
		m_Primary = primaryTag->GetValue();

	auto secondaryTag = nbt->GetTag<TagInt>(L"Secondary");
	if (secondaryTag)
		m_Secondary = secondaryTag->GetValue();

	return true;
}
