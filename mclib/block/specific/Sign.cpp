#include "stdafx.h"

#include <block/specific/Sign.h>

bool Sign::ImportNBT(NBT* nbt)
{
	TagString* textTags[] = {
		nbt->GetTag<TagString>(L"Text1"),
		nbt->GetTag<TagString>(L"Text2"),
		nbt->GetTag<TagString>(L"Text3"),
		nbt->GetTag<TagString>(L"Text4"),
	};

	int32 textCount = 0;
	for (int32 i = 0; i < 4; ++i)
	{
		if (textTags[i] != nullptr)
		{
			m_Text[i] = textTags[i]->GetValue();
			textCount++;
		}
	}

	// There must be 4 text lines to be considered valid.
	return textCount == 4;
}

const std::wstring& Sign::GetText(std::size_t index) const
{
	return m_Text[index];
}
