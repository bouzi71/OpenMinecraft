#pragma once

// FORWARD BEGIN
class AssetCache;
// FORWARD END

#include <common/JsonFwd.h>

#include <block/BlocksLoaderPtr.h>

class CMinecraftBlocksLoader
	: public IMinecraftBlocksLoader
{
public:
	CMinecraftBlocksLoader(const IBaseManager& BaseManager, AssetCache& AssetCache);
	virtual ~CMinecraftBlocksLoader();

	const CMinecraftBlock* GetBlock(uint32 ID) const override;
	const CMinecraftBlock* GetBlock(uint16 Type, uint16 Meta) const;

	void RegisterBlockInfos();
	void RegisterVanillaBlocks();
	void RegisterVanillaBlocks2();

	void RegisterBlock(CMinecraftBlock* block);
	void ClearRegistry();

	const std::map<uint32, CMinecraftBlock*>& GetAllBlocks() const noexcept;

private:
	const IBaseManager& m_BaseManager;
	AssetCache& m_AssetCache;

	struct SBlockInfo
	{
		SBlockInfo(bool IsTransperent, bool IsSolid)
			: IsTransperent(IsTransperent)
			, IsSolid(IsSolid)
		{}

		bool IsTransperent;
		bool IsSolid;
	};

	std::map<uint32, CMinecraftBlock*> m_Blocks;
	std::map<std::string, SBlockInfo> m_BlockInfos;
};