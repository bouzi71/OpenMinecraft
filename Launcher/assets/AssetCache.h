#ifndef TERRACOTTA_ASSETS_ASSETCACHE_H_
#define TERRACOTTA_ASSETS_ASSETCACHE_H_

#include "TextureArray.h"

#include <assets/blocks/BlockState.h>
#include <assets/blocks/BlockModel.h>
#include <assets/blocks/BlockVariant.h>

#include <block/Block.h>

namespace terra
{

class ZipArchive;

class AssetCache
{
public:
	AssetCache() {}
	virtual ~AssetCache() {}

	TextureArray& GetTextures() { return m_TextureArray; }
	TextureHandle AddTexture(const std::string& path, const std::string& data);

	void AddVariantModel(std::unique_ptr<BlockVariant> variant);
	BlockVariant* GetVariant(const CMinecraftBlock* block);

	std::vector<BlockModel*> GetBlockModels(const std::string& find);
	BlockModel* GetBlockModel(const std::string& path);
	void AddBlockModel(const std::string& path, std::unique_ptr<BlockModel> model);

private:
	// Maps block id to the BlockState
	//std::vector<BlockVariant*> m_VariantCache;

	// Maps model path to a BlockModel
	std::unordered_map<std::string, std::unique_ptr<BlockModel>> m_BlockModels;

	// Block name -> (list of Variants)
	std::map<u32, std::unique_ptr<BlockVariant>> m_BlockVariants;

	TextureArray m_TextureArray;
};

} // terra

extern std::unique_ptr<terra::AssetCache> g_AssetCache;

#endif
