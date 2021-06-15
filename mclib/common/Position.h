#ifndef MCLIB_COMMON_POSITION_H_
#define MCLIB_COMMON_POSITION_H_

#include <common/Types.h>


class DataBuffer;


class Position
{
public:
	Position() noexcept 
		: m_X(0)
		, m_Y(0)
		, m_Z(0) 
	{}
	MCLIB_API Position(int32 x, int32 y, int32 z) noexcept;
	MCLIB_API int64 Encode64() const noexcept;

	int64 GetX() const { return m_X; }
	int64 GetY() const { return m_Y; }
	int64 GetZ() const { return m_Z; }

	friend MCLIB_API DataBuffer& operator<<(DataBuffer& out, const Position& pos);
	friend MCLIB_API DataBuffer& operator>>(DataBuffer& in, Position& pos);

private:
	int64 m_X;
	int64 m_Y;
	int64 m_Z;
};

MCLIB_API DataBuffer& operator<<(DataBuffer& out, const Position& pos);
MCLIB_API DataBuffer& operator>>(DataBuffer& in, Position& pos);

MCLIB_API std::string to_string(const Position& data);

MCLIB_API std::ostream& operator<<(std::ostream& out, const Position& pos);
MCLIB_API std::wostream& operator<<(std::wostream& out, const Position& pos);

#endif
