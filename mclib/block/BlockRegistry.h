#pragma once

#include "Block.h"

class BlockRegistry
{
public:
	static MCLIB_API BlockRegistry* GetInstance();

public:
	MCLIB_API ~BlockRegistry();

	MCLIB_API const CMinecraftBlock* GetBlock(u32 ID) const;
	MCLIB_API const CMinecraftBlock* GetBlock(u16 Type, u16 Meta) const;

	MCLIB_API void RegisterVanillaBlocks();
	MCLIB_API void RegisterVanillaBlocks2();
	MCLIB_API void RegisterBlock(CMinecraftBlock* block);
	MCLIB_API void ClearRegistry();

	MCLIB_API const std::map<u32, CMinecraftBlock*>& GetAllBlocks() const noexcept;

private:
	BlockRegistry();
	
private:
	std::map<u32, CMinecraftBlock*> m_Blocks;
};