#pragma once

#include "Block.h"

class BlockRegistry
{
public:
	static MCLIB_API void CreateInstance(const IBaseManager& BaseManager);
	static MCLIB_API BlockRegistry* GetInstance();

public:
	MCLIB_API BlockRegistry(const IBaseManager& BaseManager);
	MCLIB_API ~BlockRegistry();

	MCLIB_API const CMinecraftBlock* GetBlock(uint32 ID) const;
	MCLIB_API const CMinecraftBlock* GetBlock(uint16 Type, uint16 Meta) const;

	MCLIB_API void RegisterBlockInfos();
	MCLIB_API void RegisterVanillaBlocks();
	MCLIB_API void RegisterVanillaBlocks2();

	MCLIB_API void RegisterBlock(CMinecraftBlock* block);
	MCLIB_API void ClearRegistry();

	MCLIB_API const std::map<uint32, CMinecraftBlock*>& GetAllBlocks() const noexcept;
	
private:
	struct SBlockInfo
	{
		SBlockInfo(bool IsTransperent, bool IsSolid)
			: IsTransperent(IsTransperent)
			, IsSolid(IsSolid)
		{}

		bool IsTransperent;
		bool IsSolid;
	};

	const IBaseManager& m_BaseManager;
	std::map<uint32, CMinecraftBlock*> m_Blocks;
	std::map<std::string, SBlockInfo> m_BlockInfos;
};