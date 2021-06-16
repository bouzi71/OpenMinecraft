#ifndef MCLIB_NBT_NBT_H_
#define MCLIB_NBT_NBT_H_

#include <common/Types.h>
#include <nbt/Tag.h>


class DataBuffer;


class NBT
{
private:
	TagCompound m_Root;

public:
	MCLIB_API NBT() {}

	NBT(const NBT& rhs) = default;
	NBT& operator=(const NBT& rhs) = default;
	NBT(NBT&& rhs) = default;
	NBT& operator=(NBT&& rhs) = default;

	TagCompound& GetRoot() noexcept { return m_Root; }
	const TagCompound& GetRoot() const noexcept { return m_Root; }
	bool HasData() const { return m_Root.begin() != m_Root.end(); }

	void SetRoot(const TagCompound& compound) { m_Root = compound; }

	template <typename T>
	T* GetTag(const std::wstring& tagName) const
	{
		return m_Root.GetTag<T>(tagName);
	}

	friend MCLIB_API DataBuffer& operator>>(DataBuffer& out, NBT& nbt);
};

MCLIB_API DataBuffer& operator<<(DataBuffer& out, const NBT& nbt);
MCLIB_API DataBuffer& operator>>(DataBuffer& in, NBT& nbt);


#endif
