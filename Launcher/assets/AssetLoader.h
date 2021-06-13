#pragma once

#ifndef TERRACOTTA_ASSETS_ASSETLOADER_H_
#define TERRACOTTA_ASSETS_ASSETLOADER_H_


#include <common/JsonFwd.h>

#include "TextureArray.h"

#include <assets/blocks/BlockFace.h>
#include <assets/blocks/BlockElement.h>
#include <assets/blocks/BlockModel.h>
#include <assets/blocks/BlockVariant.h>

namespace terra
{

// Stores the intermediate faces of a block. The textures are resolved later and stored into a final RenderableFace.
struct IntermediateFace
{
	BlockFace face;
	BlockFace cull_face;
	std::string texture;
	int tint_index;
	glm::vec2 uv_from;
	glm::vec2 uv_to;
};

// Stores the intermediate element for a BlockModel. The faces are resolved later and stored in a final BlockElement.
struct IntermediateElement
{
	glm::vec3 from;
	glm::vec3 to;
	bool shade;
	ElementRotation rotation;

	std::vector<IntermediateFace> faces;
};


class AssetCache;


class AssetLoader
{
public:
	AssetLoader(const IBaseManager& BaseManager, AssetCache& cache);

	void Initialize();

private:
	using TextureMap = std::unordered_map<std::string, std::string>;

	bool LoadTexture(const std::string& path, TextureHandle* handle);

	std::unique_ptr<BlockModel> LoadBlockModel(const std::string& path, TextureMap& texture_map, std::vector<IntermediateElement>& intermediates);
	BlockModel* LoadBlockModel(const std::string& ModelPath);

	std::unique_ptr<BlockVariant> LoadDefaultVariantBlock(const CMinecraftBlock * Block);

	std::unique_ptr<BlockVariant> LoadBlockVariant(const CMinecraftBlock * Block, json VariantJSONObject);
	void LoadBlockVariants(const CMinecraftBlock* ForBlock);
	void LoadBlockVariants();

private:
	const IBaseManager& m_BaseManager;
	AssetCache& m_Cache;
};

} // ns terra

#endif

