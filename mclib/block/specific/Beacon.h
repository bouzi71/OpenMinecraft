#ifndef MCLIB_BLOCK_BEACON_H_
#define MCLIB_BLOCK_BEACON_H_

#include <block/BlockEntity.h>




class Beacon 
	: public BlockEntity
{
private:
	std::wstring m_Lock;
	s32 m_Levels;
	s32 m_Primary;
	s32 m_Secondary;

public:
	MCLIB_API Beacon(BlockEntityType type, Vector3i position) : BlockEntity(type, position), m_Levels(0), m_Primary(0), m_Secondary(0) {}
	MCLIB_API bool ImportNBT(NBT* nbt);

	s32 GetLevels() const noexcept { return m_Levels; }
	s32 GetPrimary() const noexcept { return m_Primary; }
	s32 GetSecondary() const noexcept { return m_Secondary; }
};

#endif