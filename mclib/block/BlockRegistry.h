#pragma once

#include "Block.h"

class BlockRegistry
{
public:
	static MCLIB_API BlockRegistry* GetInstance();

public:
	MCLIB_API ~BlockRegistry();

	MCLIB_API const Block* GetBlock(u32 ID) const;
	MCLIB_API const Block* GetBlock(u16 Type, u16 Meta) const;

	MCLIB_API void RegisterVanillaBlocks();
	MCLIB_API void RegisterBlock(Block* block);
	MCLIB_API void ClearRegistry();

	MCLIB_API const std::map<u32, Block*>& GetAllBlocks() const noexcept;

private:
	BlockRegistry();
	
private:
	std::map<u32, Block*> m_Blocks;
};