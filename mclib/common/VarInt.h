#ifndef MCLIB_COMMON_VAR_INT_H_
#define MCLIB_COMMON_VAR_INT_H_


#include <common/Types.h>

#include <iosfwd>

class DataBuffer;

class CVarInt
{
private:
	int64 m_Value;

public:
	MCLIB_API CVarInt() noexcept;
	MCLIB_API CVarInt(int8 val) noexcept;
	MCLIB_API CVarInt(int16 val) noexcept;
	MCLIB_API CVarInt(int32 val) noexcept;
	MCLIB_API CVarInt(int64 val) noexcept;

	int8 GetByte() const noexcept { return (int8)m_Value; }
	int16 GetShort() const noexcept { return (int16)m_Value; }
	int32 GetInt() const noexcept { return (int32)m_Value; }
	int64 GetLong() const noexcept { return m_Value; }

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
