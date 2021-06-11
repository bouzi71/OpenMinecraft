#ifndef MCLIB_BLOCK_BREWINGSTAND_H_
#define MCLIB_BLOCK_BREWINGSTAND_H_

#include <common/Nameable.h>
#include <block/BlockEntity.h>
#include <block/InventoryBlock.h>



class BrewingStand 
	: public BlockEntity
	, public InventoryBlock
	, public Nameable
{
private:
	int32 m_BrewTime;
	uint8 m_Fuel;

public:
	MCLIB_API BrewingStand(BlockEntityType type, glm::ivec3 position) : BlockEntity(type, position) {}
	MCLIB_API bool ImportNBT(NBT* nbt);

	Slot GetLeftSlot() const noexcept;
	Slot GetMiddleSlot() const noexcept;
	Slot GetRightSlot() const noexcept;
	Slot GetIngredientSlot() const noexcept;
	Slot GetFuelSlot() const noexcept;

	uint8 GetBrewTime() const noexcept { return m_BrewTime; }
	uint8 GetFuel() const noexcept { return m_Fuel; }
};

#endif
