#include "stdafx.h"

// General
#include "AssetCache.h"

// Additional
#include <common/Json.h>
#include <block/Block.h>



AssetCache::AssetCache(const IBaseManager& BaseManager)
	: m_TexturesLoader(BaseManager, *this)
	, m_ModelsLoader(BaseManager, *this)
	, m_BlockstatesLoader(BaseManager, *this)
	, m_BlocksLoader(BaseManager, *this)
{}

AssetCache::~AssetCache()
{}

void AssetCache::Initialize()
{
	m_BlocksLoader.RegisterVanillaBlocks();
	m_BlockstatesLoader.Initialize();
}

CMinecraftTexturesLoader& AssetCache::GetTexturesLoader()
{
	return m_TexturesLoader;
}

CMinecraftModelsLoader& AssetCache::GetModelsLoader()
{
	return m_ModelsLoader;
}

CMinecraftBlockstatesLoader & AssetCache::GetBlockstatesLoader()
{
	return m_BlockstatesLoader;
}

CMinecraftBlocksLoader & AssetCache::GetBlocksLoader()
{
	return m_BlocksLoader;
}
