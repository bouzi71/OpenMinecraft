#include "stdafx.h"

#include <inventory/Slot.h>

#include <common/DataBuffer.h>

Slot Slot::FromNBT(TagCompound& compound)
{
	u8 count = 0;
	s16 id = -1;
	s16 damage = 0;
	NBT nbt;

	for (auto iter = compound.begin(); iter != compound.end(); ++iter)
	{
		auto tag = iter->second;
		if (tag->GetName() == L"Count")
		{
			count = ((TagByte*)tag.get())->GetValue();
		}
		else if (tag->GetName() == L"Damage")
		{
			damage = ((TagShort*)tag.get())->GetValue();
		}
		else if (tag->GetName() == L"id")
		{
			id = ((TagShort*)tag.get())->GetValue();
		}
		else if (tag->GetName() == L"tag")
		{
			TagCompound* newRoot = ((TagCompound*)tag.get());

			nbt.SetRoot(*newRoot);
		}
	}

	return Slot(id, count, damage, nbt);
}

DataBuffer Slot::Serialize(Version version) const
{
	DataBuffer out;
	out << m_ItemId;
	if (m_ItemId == -1)
		return out;

	out << m_ItemCount << m_ItemDamage;

	if (m_NBT.HasData())
	{
		out << m_NBT;
	}
	else
	{
		out << (u8)0;
	}

	return out;
}

void Slot::Deserialize(DataBuffer& in, Version version)
{
	m_ItemId = -1;
	m_ItemCount = 0;
	m_ItemDamage = 0;

	s16 id;
	in >> id;

	m_ItemId = id;

	if (m_ItemId == -1)
	{
		m_ItemCount = 0;
		m_ItemDamage = 0;
		return;
	}

	in >> m_ItemCount;
	in >> m_ItemDamage;

	u8 hasNBT;
	in >> hasNBT;

	if (hasNBT)
	{
		in.SetReadOffset(in.GetReadOffset() - 1);
		in >> m_NBT;
	}
}