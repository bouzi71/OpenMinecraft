#include "stdafx.h"

#include <block/specific/Banner.h>

bool Banner::ImportNBT(NBT* nbt)
{
	auto nameTag = nbt->GetTag<TagString>(L"CustomName");
	if (nameTag)
	{
		this->SetCustomName(nameTag->GetValue());
	}

	auto baseTag = nbt->GetTag<TagInt>(L"Base");
	if (!baseTag) 
		return false;

	m_Base = static_cast<DyeColor>(baseTag->GetValue());

	auto patternsTag = nbt->GetTag<TagList>(L"Patterns");
	if (!patternsTag) return true;

	for (auto iter = patternsTag->begin(); iter != patternsTag->end(); ++iter)
	{
		TagCompound* sectionCompound = (TagCompound*)iter->get();

		auto colorTag = sectionCompound->GetTag<TagInt>(L"Color");
		auto patternTag = sectionCompound->GetTag<TagString>(L"Pattern");

		if (!colorTag || !patternTag) return false;

		Pattern pattern;

		pattern.color = static_cast<DyeColor>(colorTag->GetValue());
		pattern.section = patternTag->GetValue();

		m_Patterns.push_back(pattern);
	}

	return true;
}
