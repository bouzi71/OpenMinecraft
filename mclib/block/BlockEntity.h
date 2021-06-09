#ifndef MCLIB_BLOCK_BLOCK_ENTITY_H_
#define MCLIB_BLOCK_BLOCK_ENTITY_H_

#include <mclib.h>
#include <common/Vector.h>
#include <nbt/NBT.h>
#include <inventory/Slot.h>

enum class BlockEntityType
{
	Banner,
	Beacon,
	Bed,
	Cauldron,
	BrewingStand,
	Chest,
	Comparator,
	CommandBlock,
	DaylightSensor,
	Dispenser,
	Dropper,
	EnchantingTable,
	EnderChest,
	EndGateway,
	EndPortal,
	FlowerPot,
	Furnace,
	Hopper,
	Jukebox,
	MonsterSpawner,
	Noteblock,
	Piston,
	ShulkerBox,
	Sign,
	Skull,
	StructureBlock,
	TrappedChest,

	Unknown
};

class BlockEntity
{
public:
	MCLIB_API BlockEntity(BlockEntityType type, Vector3i position) noexcept;

	MCLIB_API BlockEntityType GetType() const noexcept { return m_Type; }
	MCLIB_API Vector3i GetPosition() const noexcept { return m_Position; }
	MCLIB_API NBT* GetNBT() noexcept { return &m_NBT; }

	MCLIB_API static std::unique_ptr<BlockEntity> CreateFromNBT(NBT* nbt);

protected:
	virtual MCLIB_API bool ImportNBT(NBT* nbt) { return true; }

private:
	BlockEntityType m_Type;
	Vector3i m_Position;
	NBT m_NBT;
};
typedef std::shared_ptr<BlockEntity> BlockEntityPtr;

#endif
