#ifndef MCLIB_COMMON_VAR_INT_H_
#define MCLIB_COMMON_VAR_INT_H_


#include <common/Types.h>

#include <iosfwd>

class DataBuffer;

class CVarInt
{
private:
	s64 m_Value;

public:
	MCLIB_API CVarInt() noexcept;
	MCLIB_API CVarInt(s8 val) noexcept;
	MCLIB_API CVarInt(s16 val) noexcept;
	MCLIB_API CVarInt(s32 val) noexcept;
	MCLIB_API CVarInt(s64 val) noexcept;

	s8 GetByte() const noexcept { return (s8)m_Value; }
	s16 GetShort() const noexcept { return (s16)m_Value; }
	s32 GetInt() const noexcept { return (s32)m_Value; }
	s64 GetLong() const noexcept { return m_Value; }

	// Returns how many bytes this will take up in a buffer
	std::size_t MCLIB_API GetSerializedLength() const;

	friend MCLIB_API DataBuffer& operator<<(DataBuffer& out, const CVarInt& pos);
	friend MCLIB_API DataBuffer& operator>>(DataBuffer& in, CVarInt& pos);
};

typedef CVarInt VarLong;

MCLIB_API DataBuffer& operator<<(DataBuffer& out, const CVarInt& var);
MCLIB_API DataBuffer& operator>>(DataBuffer& in, CVarInt& var);


MCLIB_API std::ostream& operator<<(std::ostream& out, const CVarInt& v);

#endif
