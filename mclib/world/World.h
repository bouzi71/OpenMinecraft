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
	virtual void OnChunkLoad(ChunkPtr chunk, const ChunkColumnMetadata& meta, uint16 yIndex) {}
	virtual void OnChunkUnload(std::shared_ptr<ChunkColumn> chunk) {}
	virtual void OnBlockChange(glm::ivec3 position, const CMinecraftBlock * newBlock, const CMinecraftBlock * oldBlock) {}
};


class World 
	: public PacketHandler
	, public ObserverSubject<WorldListener>
{
private:
	typedef std::pair<int32, int32> ChunkCoord;

	std::map<ChunkCoord, std::shared_ptr<ChunkColumn>> m_Chunks;

	bool MCLIB_API SetBlock(glm::ivec3 position, uint32 blockData);

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
	std::shared_ptr<ChunkColumn> MCLIB_API GetChunk(glm::ivec3 pos) const;

	MCLIB_API const CMinecraftBlock* GetBlock(glm::dvec3 pos) const;
	MCLIB_API const CMinecraftBlock* GetBlock(glm::vec3 pos) const;
	MCLIB_API const CMinecraftBlock* GetBlock(glm::ivec3 pos) const;

	MCLIB_API BlockEntityPtr GetBlockEntity(glm::ivec3 pos) const;
	// Gets all of the known block entities in loaded chunks
	MCLIB_API std::vector<BlockEntityPtr> GetBlockEntities() const;

	const std::map<ChunkCoord, std::shared_ptr<ChunkColumn>>::const_iterator begin() const { return m_Chunks.begin(); }
	const std::map<ChunkCoord, std::shared_ptr<ChunkColumn>>::const_iterator end() const { return m_Chunks.end(); }
};

#endif
