#ifndef MCLIB_BLOCK_DROPPER_H_
#define MCLIB_BLOCK_DROPPER_H_

#include <common/Nameable.h>
#include <block/BlockEntity.h>
#include <block/InventoryBlock.h>



class Dropper 
	: public BlockEntity
	, public InventoryBlock
	, public Nameable
{
public:
	MCLIB_API Dropper(BlockEntityType type, Vector3i position) : BlockEntity(type, position) {}
	MCLIB_API bool ImportNBT(NBT* nbt);
};

#endif
