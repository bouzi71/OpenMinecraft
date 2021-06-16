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
	std::unique_ptr<BlockVariant> LoadDefaultVariantBlock(const CMinecraftBlock * Block);

	std::unique_ptr<BlockVariant> LoadBlockVariant(const CMinecraftBlock * Block, json VariantJSONObject);
	void LoadBlockstateByBlock(const CMinecraftBlock* ForBlock);

private:
	const IBaseManager& m_BaseManager;
	AssetCache& m_AssetCache;

	std::map<uint32, std::unique_ptr<BlockVariant>> m_BlockVariants;
};
