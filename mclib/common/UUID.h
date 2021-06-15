#ifndef MCLIB_COMMON_UUID_H_
#define MCLIB_COMMON_UUID_H_

#include <common/Types.h>


class DataBuffer;


class CUUID
{
private:
	uint64 m_MostSigBits;
	uint64 m_LeastSigBits;

public:
	CUUID() noexcept : m_MostSigBits(0), m_LeastSigBits(0) {}
	CUUID(uint64 most, uint64 least) noexcept : m_MostSigBits(most), m_LeastSigBits(least) {}

	std::string MCLIB_API ToString(bool dashes = true) const;

	uint64 GetUpperBits() const noexcept { return m_MostSigBits; }
	uint64 GetLowerBits() const noexcept { return m_LeastSigBits; }

	static CUUID MCLIB_API FromString(const std::string& str, bool dashes = true);
	static CUUID MCLIB_API FromString(const std::wstring& str, bool dashes = true);

	bool operator<(const CUUID& r) const
	{
		const CUUID& l = *this;
		if (l.GetUpperBits() < r.GetUpperBits()) return true;
		if (l.GetUpperBits() > r.GetUpperBits()) return false;
		if (l.GetLowerBits() < r.GetLowerBits()) return true;
		if (l.GetLowerBits() > r.GetLowerBits()) return false;
		return false;
	}

	friend MCLIB_API DataBuffer& operator<<(DataBuffer& out, const CUUID& uuid);
	friend MCLIB_API DataBuffer& operator>>(DataBuffer& in, CUUID& uuid);
};

MCLIB_API DataBuffer& operator<<(DataBuffer& out, const CUUID& uuid);
MCLIB_API DataBuffer& operator>>(DataBuffer& in, CUUID& uuid);

MCLIB_API std::ostream& operator<<(std::ostream& out, const CUUID& uuid);
MCLIB_API std::wostream& operator<<(std::wostream& out, const CUUID& uuid);

#endif
