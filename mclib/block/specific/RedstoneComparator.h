#ifndef MCLIB_BLOCK_REDSTONECOMPARATOR_H_
#define MCLIB_BLOCK_REDSTONECOMPARATOR_H_

#include <block/BlockEntity.h>



class RedstoneComparator : public BlockEntity
{
private:
	int32 m_OutputSignal;

public:
	MCLIB_API RedstoneComparator(BlockEntityType type, glm::ivec3 position) : BlockEntity(type, position) {}
	MCLIB_API bool ImportNBT(NBT* nbt);

	inline int32 GetOutputSignal() const noexcept { return m_OutputSignal; }
};

#endif
