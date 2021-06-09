#ifndef MCLIB_BLOCK_NOTEBLOCK_H_
#define MCLIB_BLOCK_NOTEBLOCK_H_

#include <block/BlockEntity.h>



class NoteBlock 
	: public BlockEntity
{
private:
	u8 m_Note;
	u8 m_Powered;

public:
	MCLIB_API NoteBlock(BlockEntityType type, Vector3i position) : BlockEntity(type, position) {}
	MCLIB_API bool ImportNBT(NBT* nbt);

	inline u8 GetNode() const noexcept { return m_Note; }
	inline bool IsPowered() const noexcept { return m_Powered != 0; }
};

#endif
