#pragma once

#include "block/Block.h"

struct IMinecraftBlocksLoader
{
	virtual const CMinecraftBlock* GetBlock(uint32 ID) const = 0;
};

MCLIB_API void SetBlocksLoader(IMinecraftBlocksLoader * BlocksLoaderPtr);

extern IMinecraftBlocksLoader * g_BlocksLoader;