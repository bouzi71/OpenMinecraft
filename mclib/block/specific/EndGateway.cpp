#include "stdafx.h"

#include <block/specific/EndGateway.h>

bool EndGateway::ImportNBT(NBT* nbt)
{
	auto ageTag = nbt->GetTag<TagLong>(L"Age");
	if (ageTag)
		m_Age = ageTag->GetValue();

	auto exactTag = nbt->GetTag<TagByte>(L"ExactTeleport");
	if (exactTag)
		m_ExactTeleport = exactTag->GetValue() != 0;

	auto exitTag = nbt->GetTag<TagCompound>(L"ExitPortal");
	if (exitTag)
	{
		auto xTag = exitTag->GetTag<TagInt>(L"X");
		auto yTag = exitTag->GetTag<TagInt>(L"Y");
		auto zTag = exitTag->GetTag<TagInt>(L"Z");

		if (!xTag || !yTag || !zTag) 
			return true;

		m_Exit = glm::ivec3(xTag->GetValue(), yTag->GetValue(), zTag->GetValue());
	}

	return true;
}
