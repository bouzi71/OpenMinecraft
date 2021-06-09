#ifndef MCLIB_INVENTORY_SLOT_H_
#define MCLIB_INVENTORY_SLOT_H_

#include <mclib.h>
#include <common/Types.h>
#include <nbt/NBT.h>
#include <protocol/ProtocolState.h>

class DataBuffer;

class Slot
{
public:
	Slot() noexcept 
		: m_ItemId(-1)
		, m_ItemCount(0)
		, m_ItemDamage(0) 
	{}
	Slot(s16 itemId, u8 itemCount, s16 itemDamage) noexcept
		: m_ItemId(itemId)
		, m_ItemCount(itemCount)
		, m_ItemDamage(itemDamage)
		, m_NBT()
	{}

	Slot(s16 itemId, u8 itemCount, s16 itemDamage, NBT nbt) noexcept
		: m_ItemId(itemId)
		, m_ItemCount(itemCount)
		, m_ItemDamage(itemDamage)
		, m_NBT(nbt)
	{}

	Slot(const Slot& rhs) = default;
	Slot& operator=(const Slot& rhs) = default;
	Slot(Slot&& rhs) = default;
	Slot& operator=(Slot&& rhs) = default;

	s16 GetItemId() const noexcept { return m_ItemId; }
	u8 GetItemCount() const noexcept { return m_ItemCount; }
	s16 GetItemDamage() const noexcept { return m_ItemDamage; }
	const NBT& GetNBT() const noexcept { return m_NBT; }

	static MCLIB_API Slot FromNBT(TagCompound& compound);

	DataBuffer Serialize(Version version) const;
	void Deserialize(DataBuffer& in, Version version);

private:
	NBT m_NBT;
	s16 m_ItemId;
	s16 m_ItemDamage;
	u8 m_ItemCount;
};

#endif
