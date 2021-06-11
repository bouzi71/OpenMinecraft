#ifndef MCLIB_BLOCK_BEACON_H_
#define MCLIB_BLOCK_BEACON_H_

#include <block/BlockEntity.h>




class Beacon 
	: public BlockEntity
{
private:
	std::wstring m_Lock;
	int32 m_Levels;
	int32 m_Primary;
	int32 m_Secondary;

public:
	MCLIB_API Beacon(BlockEntityType type, glm::ivec3 position) : BlockEntity(type, position), m_Levels(0), m_Primary(0), m_Secondary(0) {}
	MCLIB_API bool ImportNBT(NBT* nbt);

	int32 GetLevels() const noexcept { return m_Levels; }
	int32 GetPrimary() const noexcept { return m_Primary; }
	int32 GetSecondary() const noexcept { return m_Secondary; }
};

#endif