#include "stdafx.h"

#include <common/Position.h>
#include <common/DataBuffer.h>

#include <cmath>
#include <sstream>

Position::Position(int32 x, int32 y, int32 z) noexcept
	: m_X(x)
	, m_Y(y)
	, m_Z(z)
{

}

int64 Position::Encode64() const noexcept
{
	return ((m_X & 0x3FFFFFF) << 38) | ((m_Y & 0xFFF) << 26) | (m_Z & 0x3FFFFFF);
}


DataBuffer& operator<<(DataBuffer& out, const Position& pos)
{
	return out << pos.Encode64();
}

DataBuffer& operator>>(DataBuffer& in, Position& pos)
{
	uint64 val;
	in >> val;

	pos.m_X = val >> 38;
	pos.m_Y = (val >> 26) & 0xFFF;
	pos.m_Z = val << 38 >> 38;

	if (pos.m_X >= std::pow(2, 25)) pos.m_X -= (int64)std::pow(2, 26);
	if (pos.m_Y >= std::pow(2, 11)) pos.m_Y -= (int64)std::pow(2, 12);
	if (pos.m_Z >= std::pow(2, 25)) pos.m_Z -= (int64)std::pow(2, 26);

	return in;
}

std::string to_string(const Position& pos)
{
	std::stringstream ss;

	ss << "(" << pos.GetX() << ", " << pos.GetY() << ", " << pos.GetZ() << ")";
	return ss.str();
}

std::ostream& operator<<(std::ostream& out, const Position& pos)
{
	return out << to_string(pos);
}

std::wostream& operator<<(std::wostream& out, const Position& pos)
{
	std::string str = to_string(pos);
	std::wstring wstr(str.begin(), str.end());

	return out << wstr;
}
