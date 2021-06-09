#include "stdafx.h"

#include <block/specific/NoteBlock.h>

bool NoteBlock::ImportNBT(NBT* nbt)
{
	auto noteTag = nbt->GetTag<TagByte>(L"note");
	if (noteTag)
		m_Note = noteTag->GetValue();

	auto poweredTag = nbt->GetTag<TagByte>(L"powered");
	if (poweredTag)
		m_Powered = poweredTag->GetValue();

	return true;
}
