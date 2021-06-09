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
	s32 m_BrewTime;
	u8 m_Fuel;

public:
	MCLIB_API BrewingStand(BlockEntityType type, Vector3i position) : BlockEntity(type, position) {}
	MCLIB_API bool ImportNBT(NBT* nbt);

	Slot GetLeftSlot() const noexcept;
	Slot GetMiddleSlot() const noexcept;
	Slot GetRightSlot() const noexcept;
	Slot GetIngredientSlot() const noexcept;
	Slot GetFuelSlot() const noexcept;

	u8 GetBrewTime() const noexcept { return m_BrewTime; }
	u8 GetFuel() const noexcept { return m_Fuel; }
};

#endif
