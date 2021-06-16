#pragma once

// FORWARD BEGIN
class AssetCache;
// FORWARD END

#include <common/JsonFwd.h>

#include "blocks/BlockFace.h"
#include "blocks/BlockElement.h"
#include "blocks/BlockModel.h"


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


class CMinecraftModelsLoader
{
public:
	CMinecraftModelsLoader(const IBaseManager& BaseManager, AssetCache& AssetCache);
	virtual ~CMinecraftModelsLoader();

	void Initialize();

	std::vector<const BlockModel*> GetBlockModels(const std::string& find) const;
	const BlockModel* GetBlockModel(const std::string& path);
	void AddBlockModel(const std::string& path, std::unique_ptr<BlockModel> model);

private:
	std::unique_ptr<BlockModel> LoadBlockModel(const std::string& path, std::unordered_map<std::string, std::string>& texture_map, std::vector<IntermediateElement>& intermediates);
	BlockModel* LoadBlockModel(const std::string& ModelPath);

private:
	const IBaseManager& m_BaseManager;
	AssetCache& m_AssetCache;

	std::unordered_map<std::string, std::unique_ptr<BlockModel>> m_BlockModels;
};


