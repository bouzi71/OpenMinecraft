#pragma once

#include "TexturesLoader.h"
#include "ModelsLoader.h"
#include "BlockstatesLoader.h"
#include "BlocksLoader.h"

class AssetCache
{
public:
	AssetCache(const IBaseManager& BaseManager);
	virtual ~AssetCache();

	void Initialize();

	CMinecraftTexturesLoader& GetTexturesLoader();
	CMinecraftModelsLoader& GetModelsLoader();
	CMinecraftBlockstatesLoader& GetBlockstatesLoader();
	CMinecraftBlocksLoader& GetBlocksLoader();

private:
	CMinecraftTexturesLoader m_TexturesLoader;
	CMinecraftModelsLoader m_ModelsLoader;
	CMinecraftBlockstatesLoader m_BlockstatesLoader;
	CMinecraftBlocksLoader m_BlocksLoader;
};
