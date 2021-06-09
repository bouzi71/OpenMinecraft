#ifndef MCLIB_BLOCK_ENCHANTMENTTABLE_H_
#define MCLIB_BLOCK_ENCHANTMENTTABLE_H_

#include <common/Nameable.h>
#include <block/BlockEntity.h>



class EnchantmentTable 
	: public BlockEntity
	, public Nameable
{
public:
	MCLIB_API EnchantmentTable(BlockEntityType type, Vector3i position) : BlockEntity(type, position) {}
	MCLIB_API bool ImportNBT(NBT* nbt);
};

#endif
