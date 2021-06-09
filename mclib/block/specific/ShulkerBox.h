#ifndef MCLIB_BLOCK_SHULKERBOX_H_
#define MCLIB_BLOCK_SHULKERBOX_H_

#include <common/Nameable.h>
#include <block/BlockEntity.h>
#include <block/InventoryBlock.h>



class ShulkerBox 
	: public BlockEntity
	, public InventoryBlock
	, public Nameable
{
public:
	MCLIB_API ShulkerBox(BlockEntityType type, Vector3i position) : BlockEntity(type, position) {}
	MCLIB_API bool ImportNBT(NBT* nbt);
};

#endif
