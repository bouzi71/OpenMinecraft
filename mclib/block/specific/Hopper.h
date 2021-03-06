#ifndef MCLIB_BLOCK_HOPPER_H_
#define MCLIB_BLOCK_HOPPER_H_

#include <common/Nameable.h>
#include <block/BlockEntity.h>
#include <block/InventoryBlock.h>



class Hopper 
	: public BlockEntity
	, public InventoryBlock
	, public Nameable
{
private:
	int32 m_TransferCooldown;

public:
	MCLIB_API Hopper(BlockEntityType type, glm::ivec3 position) : BlockEntity(type, position) {}
	MCLIB_API bool ImportNBT(NBT* nbt);

	int32 GetTransferCooldown() const noexcept { return m_TransferCooldown; }
};

#endif
