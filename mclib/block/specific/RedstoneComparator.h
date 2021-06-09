#ifndef MCLIB_BLOCK_REDSTONECOMPARATOR_H_
#define MCLIB_BLOCK_REDSTONECOMPARATOR_H_

#include <block/BlockEntity.h>



class RedstoneComparator : public BlockEntity
{
private:
	s32 m_OutputSignal;

public:
	MCLIB_API RedstoneComparator(BlockEntityType type, Vector3i position) : BlockEntity(type, position) {}
	MCLIB_API bool ImportNBT(NBT* nbt);

	inline s32 GetOutputSignal() const noexcept { return m_OutputSignal; }
};

#endif
