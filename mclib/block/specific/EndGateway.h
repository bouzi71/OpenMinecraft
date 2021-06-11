#ifndef MCLIB_BLOCK_ENDGATEWAY_H_
#define MCLIB_BLOCK_ENDGATEWAY_H_

#include <block/BlockEntity.h>



class EndGateway 
	: public BlockEntity
{
private:
	int64 m_Age;
	bool m_ExactTeleport;
	glm::ivec3 m_Exit;

public:
	MCLIB_API EndGateway(BlockEntityType type, glm::ivec3 position) : BlockEntity(type, position) {}
	MCLIB_API bool ImportNBT(NBT* nbt);

	inline int64 GetAge() const noexcept { return m_Age; }
	inline bool IsExactTeleport() const noexcept { return m_ExactTeleport; }
	inline glm::ivec3 GetExit() const noexcept { return m_Exit; }
};

#endif
