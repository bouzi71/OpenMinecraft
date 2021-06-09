#include "stdafx.h"

#include <block/specific/Piston.h>

bool Piston::ImportNBT(NBT* nbt)
{
	auto idTag = nbt->GetTag<TagInt>(L"blockId");
	if (idTag)
		m_BlockId = idTag->GetValue();

	auto dataTag = nbt->GetTag<TagInt>(L"blockData");
	if (dataTag)
		m_BlockData = dataTag->GetValue();

	auto facingTag = nbt->GetTag<TagInt>(L"facing");
	if (facingTag)
		m_Facing = static_cast<Face>(facingTag->GetValue());

	auto progressTag = nbt->GetTag<TagFloat>(L"progress");
	if (progressTag)
		m_Progress = progressTag->GetValue();

	auto extendingTag = nbt->GetTag<TagByte>(L"extending");
	if (extendingTag)
		m_Extending = extendingTag->GetValue() != 0;

	auto sourceTag = nbt->GetTag<TagByte>(L"source");
	if (sourceTag)
		m_Source = sourceTag->GetValue() != 0;

	return true;
}
