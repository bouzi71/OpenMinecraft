#pragma once

// FORWARD BEGIN
class AssetCache;
// FORWARD END

#include <common/JsonFwd.h>

#include "blocks/BlockFace.h"
#include "blocks/BlockElement.h"
#include "blocks/BlockVariant.h"

class CMinecraftBlockstatesLoader
{
public:
	CMinecraftBlockstatesLoader(const IBaseManager& BaseManager, AssetCache& AssetCache);

	void Initialize();

	const BlockVariant* GetVariant(const CMinecraftBlock* block) const;
	void AddVariant(std::unique_ptr<BlockVariant> variant);

private:
	void LoadBlockstateByBlock(const CMinecraftBlock* ForBlock);
	std::unique_ptr<BlockVariant> LoadBlockstateArray(const CMinecraftBlock * Block, json VariantJSONObject);
	std::unique_ptr<BlockVariant> LoadBlockstateModel(const CMinecraftBlock * Block, json ModelJSONObject);
	std::unique_ptr<BlockVariant> LoadDefaultBlockstate(const CMinecraftBlock * Block);

private:
	const IBaseManager& m_BaseManager;
	AssetCache& m_AssetCache;

	std::map<uint32, std::unique_ptr<BlockVariant>> m_BlockVariants;
};
