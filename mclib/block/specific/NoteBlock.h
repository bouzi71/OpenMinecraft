#ifndef MCLIB_BLOCK_NOTEBLOCK_H_
#define MCLIB_BLOCK_NOTEBLOCK_H_

#include <block/BlockEntity.h>



class NoteBlock 
	: public BlockEntity
{
private:
	uint8 m_Note;
	uint8 m_Powered;

public:
	MCLIB_API NoteBlock(BlockEntityType type, glm::ivec3 position) : BlockEntity(type, position) {}
	MCLIB_API bool ImportNBT(NBT* nbt);

	inline uint8 GetNode() const noexcept { return m_Note; }
	inline bool IsPowered() const noexcept { return m_Powered != 0; }
};

#endif
