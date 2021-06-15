#include "stdafx.h"

// General
#include "UUID.h"

// Addtional
#include "common/DataBuffer.h"

#include <iomanip>

std::string CUUID::ToString(bool dashes) const
{
	DataBuffer buffer;
	buffer << *this;

	std::stringstream out;
	std::size_t pos = 0;

	for (uint32 i = 0; i < 4; ++i)
		out << std::hex << std::setfill('0') << std::setw(2) << ((int)buffer[pos + i] & 0xFF);
	if (dashes)
		out << '-';
	pos += 4;

	for (uint32 j = 0; j < 3; ++j)
	{
		for (uint32 i = 0; i < 2; ++i)
			out << std::hex << std::setfill('0') << std::setw(2) << ((int)buffer[pos + i] & 0xFF);
		if (dashes)
			out << '-';
		pos += 2;
	}

	for (uint32 i = 0; i < 6; ++i)
		out << std::hex << std::setfill('0') << std::setw(2) << ((int)buffer[pos + i] & 0xFF);

	return out.str();
}

CUUID CUUID::FromString(const std::string& str, bool dashes)
{
	std::wstring wstr(str.begin(), str.end());
	return CUUID::FromString(wstr, dashes);
}

CUUID CUUID::FromString(const std::wstring& str, bool dashes)
{
	if (dashes)
		assert(str.length() == 36);
	else
		assert(str.length() == 32);

	//0ba955da-bce3-3796-b74a-9faae6cc08a2

	std::wstring upperStr, lowerStr;

	upperStr.reserve(18);
	lowerStr.reserve(18);

	upperStr = L"0x";
	lowerStr = L"0x";

	if (dashes)
	{
		for (uint32 i = 0; i < 8; ++i)
			upperStr += str[i];
		for (uint32 i = 0; i < 4; ++i)
			upperStr += str[i + 9];
		for (uint32 i = 0; i < 4; ++i)
			upperStr += str[i + 14];

		for (uint32 i = 0; i < 4; ++i)
			lowerStr += str[i + 19];
		for (uint32 i = 0; i < 12; ++i)
			lowerStr += str[i + 24];
	}
	else
	{
		for (uint32 i = 0; i < 16; ++i)
			upperStr += str[i];

		for (uint32 i = 0; i < 16; ++i)
			lowerStr += str[i + 16];
	}

	CUUID uuid;

	uuid.m_MostSigBits = std::stoull(upperStr, nullptr, 16);
	uuid.m_LeastSigBits = std::stoull(lowerStr, nullptr, 16);

	return uuid;
}

DataBuffer& operator<<(DataBuffer& out, const CUUID& uuid)
{
	out << uuid.m_MostSigBits;
	out << uuid.m_LeastSigBits;
	return out;
}

DataBuffer& operator>>(DataBuffer& in, CUUID& uuid)
{
	in >> uuid.m_MostSigBits;
	in >> uuid.m_LeastSigBits;
	return in;
}

std::ostream& operator<<(std::ostream& out, const CUUID& uuid)
{
	out << uuid.ToString();
	return out;
}

std::wostream& operator<<(std::wostream& out, const CUUID& uuid)
{
	std::string str = uuid.ToString();
	out << std::wstring(str.begin(), str.end());
	return out;
}
