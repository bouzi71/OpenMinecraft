#ifndef MCLIB_WORLD_WORLD_H_
#define MCLIB_WORLD_WORLD_H_

#include <world/Chunk.h>
#include <protocol/PacketHandler.h>
#include <protocol/PacketDispatcher.h>
#include <util/ObserverSubject.h>




class MCLIB_API WorldListener
{
public:
	// yIndex is the chunk section index of the column, 0 means bottom chunk, 15 means top
	virtual void OnChunkLoad(ChunkPtr chunk, const ChunkColumnMetadata& meta, u16 yIndex) {}
	virtual void OnChunkUnload(std::shared_ptr<ChunkColumn> chunk) {}
	virtual void OnBlockChange(Vector3i position, const CMinecraftBlock * newBlock, const CMinecraftBlock * oldBlock) {}
};


class World 
	: public PacketHandler
	, public ObserverSubject<WorldListener>
{
private:
	typedef std::pair<s32, s32> ChunkCoord;

	std::map<ChunkCoord, std::shared_ptr<ChunkColumn>> m_Chunks;

	bool MCLIB_API SetBlock(Vector3i position, u32 blockData);

public:
	MCLIB_API World(PacketDispatcher* dispatcher);
	MCLIB_API ~World();

	World(const World& rhs) = delete;
	World& operator=(const World& rhs) = delete;
	World(World&& rhs) = delete;
	World& operator=(World&& rhs) = delete;

	void MCLIB_API HandlePacket(in::ChunkDataPacket* packet);
	void MCLIB_API HandlePacket(in::UnloadChunkPacket* packet);
	void MCLIB_API HandlePacket(in::MultiBlockChangePacket* packet);
	void MCLIB_API HandlePacket(in::BlockChangePacket* packet);
	void MCLIB_API HandlePacket(in::ExplosionPacket* packet);
	void MCLIB_API HandlePacket(in::UpdateBlockEntityPacket* packet);
	void MCLIB_API HandlePacket(in::RespawnPacket* packet);

	/**
	 * Pos can be any world position inside of the chunk
	 */
	std::shared_ptr<ChunkColumn> MCLIB_API GetChunk(Vector3i pos) const;

	MCLIB_API const CMinecraftBlock* GetBlock(Vector3d pos) const;
	MCLIB_API const CMinecraftBlock* GetBlock(Vector3f pos) const;
	MCLIB_API const CMinecraftBlock* GetBlock(Vector3i pos) const;

	MCLIB_API BlockEntityPtr GetBlockEntity(Vector3i pos) const;
	// Gets all of the known block entities in loaded chunks
	MCLIB_API std::vector<BlockEntityPtr> GetBlockEntities() const;

	const std::map<ChunkCoord, std::shared_ptr<ChunkColumn>>::const_iterator begin() const { return m_Chunks.begin(); }
	const std::map<ChunkCoord, std::shared_ptr<ChunkColumn>>::const_iterator end() const { return m_Chunks.end(); }
};

#endif
