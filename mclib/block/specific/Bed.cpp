#include "stdafx.h"

#include <block/specific/Bed.h>

bool Bed::ImportNBT(NBT* nbt)
{
	auto colorTag = nbt->GetTag<TagInt>(L"color");
	if (colorTag)
		m_Color = colorTag->GetValue();

	return true;
}
