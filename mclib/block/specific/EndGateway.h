#ifndef MCLIB_BLOCK_ENDGATEWAY_H_
#define MCLIB_BLOCK_ENDGATEWAY_H_

#include <block/BlockEntity.h>



class EndGateway 
	: public BlockEntity
{
private:
	s64 m_Age;
	bool m_ExactTeleport;
	Vector3i m_Exit;

public:
	MCLIB_API EndGateway(BlockEntityType type, Vector3i position) : BlockEntity(type, position) {}
	MCLIB_API bool ImportNBT(NBT* nbt);

	inline s64 GetAge() const noexcept { return m_Age; }
	inline bool IsExactTeleport() const noexcept { return m_ExactTeleport; }
	inline Vector3i GetExit() const noexcept { return m_Exit; }
};

#endif
