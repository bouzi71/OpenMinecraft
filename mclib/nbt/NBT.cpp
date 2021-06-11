#include "stdafx.h"

#include <nbt/NBT.h>

#include <common/DataBuffer.h>

DataBuffer& operator<<(DataBuffer& out, const NBT& nbt)
{
	const auto& root = nbt.GetRoot();
	out << root;
	return out;
}

DataBuffer& operator>>(DataBuffer& in, NBT& nbt)
{
	size_t offset = in.GetReadOffset();
	uint8 type;
	in >> type;

	// There is no NBT data.
	if (type == 0) 
		return in;

	in.SetReadOffset(offset);
	in >> nbt.m_Root;
	return in;
}
