#ifndef MCLIB_BLOCK_BANNER_H_
#define MCLIB_BLOCK_BANNER_H_

#include <common/DyeColor.h>
#include <common/Nameable.h>

#include <block/BlockEntity.h>

class Banner 
	: public BlockEntity
	, public Nameable
{
public:
	struct Pattern
	{
		DyeColor color;
		std::wstring section;
	};
private:
	std::vector<Pattern> m_Patterns;
	DyeColor m_Base;

public:
	MCLIB_API Banner(BlockEntityType type, Vector3i position) : BlockEntity(type, position) {}
	MCLIB_API bool ImportNBT(NBT* nbt);

	DyeColor GetBaseColor() const noexcept { return m_Base; }
	const std::vector<Pattern>& GetPattern() const noexcept { return m_Patterns; }
};

#endif
