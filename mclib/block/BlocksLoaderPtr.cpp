#include "stdafx.h"

// General
#include "BlocksLoaderPtr.h"

IMinecraftBlocksLoader * g_BlocksLoader = nullptr;

void SetBlocksLoader(IMinecraftBlocksLoader * BlocksLoaderPtr)
{
	g_BlocksLoader = BlocksLoaderPtr;
}
