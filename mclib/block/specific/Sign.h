#ifndef MCLIB_BLOCK_SIGN_H_
#define MCLIB_BLOCK_SIGN_H_

#include <block/BlockEntity.h>

class Sign 
	: public BlockEntity
{
private:
	std::array<std::wstring, 4> m_Text;

public:
	MCLIB_API Sign(BlockEntityType type, glm::ivec3 position) : BlockEntity(type, position) {}
	MCLIB_API bool ImportNBT(NBT* nbt);

	MCLIB_API const std::wstring& GetText(std::size_t index) const;
};

#endif
