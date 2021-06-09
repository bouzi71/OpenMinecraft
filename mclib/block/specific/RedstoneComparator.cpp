#include "stdafx.h"

#include <block/specific/RedstoneComparator.h>

bool RedstoneComparator::ImportNBT(NBT* nbt)
{
	auto outputSignalTag = nbt->GetTag<TagInt>(L"OutputSignal");
	if (outputSignalTag)
		m_OutputSignal = outputSignalTag->GetValue();

	return true;
}
