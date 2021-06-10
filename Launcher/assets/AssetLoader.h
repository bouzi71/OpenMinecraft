#pragma once

#ifndef TERRACOTTA_ASSETS_ASSETLOADER_H_
#define TERRACOTTA_ASSETS_ASSETLOADER_H_


#include <common/JsonFwd.h>
#include <glm/glm/glm.hpp>

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


class ZipArchive;
class AssetCache;


class AssetLoader
{
public:
	AssetLoader(AssetCache& cache);

	bool LoadArchive(const std::string& archive_path);

private:
	using TextureMap = std::unordered_map<std::string, std::string>;

	bool LoadTexture(terra::ZipArchive& archive, const std::string& path, TextureHandle* handle);

	std::unique_ptr<BlockModel> LoadBlockModel(terra::ZipArchive& archive, const std::string& path, TextureMap& texture_map, std::vector<IntermediateElement>& intermediates);
	BlockModel* LoadBlockModel(terra::ZipArchive& archive, const std::string& ModelPath);

	std::unique_ptr<BlockVariant> LoadDefaultVariantBlock(terra::ZipArchive& archive, const CMinecraftBlock * Block);

	std::unique_ptr<BlockVariant> LoadBlockVariant(terra::ZipArchive& archive, const CMinecraftBlock * Block, json VariantJSONObject);
	void LoadBlockVariants(terra::ZipArchive& archive, const CMinecraftBlock* ForBlock);
	void LoadBlockVariants(terra::ZipArchive& archive);

private:
	AssetCache& m_Cache;
};

} // ns terra

#endif

