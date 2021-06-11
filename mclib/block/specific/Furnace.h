#ifndef MCLIB_BLOCK_FURNACE_H_
#define MCLIB_BLOCK_FURNACE_H_

#include <common/Nameable.h>
#include <block/BlockEntity.h>
#include <block/InventoryBlock.h>



class Furnace 
	: public BlockEntity
	, public InventoryBlock
	, public Nameable
{
private:
	int16 m_BurnTime;
	int16 m_CookTime;
	int16 m_CookTimeTotal;

public:
	MCLIB_API Furnace(BlockEntityType type, glm::ivec3 position) : BlockEntity(type, position), m_BurnTime(0), m_CookTime(0), m_CookTimeTotal(0) {}
	MCLIB_API bool ImportNBT(NBT* nbt);

	Slot GetSmeltingSlot() const noexcept;
	Slot GetFuelSlot() const noexcept;
	Slot GetResultSlot() const noexcept;

	// Number of ticks left before the current fuel runs out.
	int16 GetBurnTime() const noexcept { return m_BurnTime; }
	// Number of ticks the item has been smelting for. The item finishes smelting when this value reaches 200 (10 seconds). Is reset to 0 if BurnTime reaches 0.
	int16 GetCookTime() const noexcept { return m_CookTime; }
	// Number of ticks It takes for the item to be smelted.
	int16 GetCookTimeTotal() const noexcept { return m_CookTimeTotal; }
};

#endif
