#ifndef MCLIB_BLOCK_JUKEBOX_H_
#define MCLIB_BLOCK_JUKEBOX_H_

#include <block/BlockEntity.h>



class Jukebox
	: public BlockEntity
{
private:
	s32 m_RecordId;
	Slot m_RecordItem;

public:
	MCLIB_API Jukebox(BlockEntityType type, Vector3i position) : BlockEntity(type, position) {}
	MCLIB_API bool ImportNBT(NBT* nbt);

	inline s32 GetRecordId() const noexcept { return m_RecordId; }
	inline Slot GetItem() const noexcept { return m_RecordItem; }
};

#endif
