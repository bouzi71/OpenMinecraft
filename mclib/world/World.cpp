#include "stdafx.h"

// General
#include "World.h"

// Additional
#include "block/BlocksLoaderPtr.h"

World::World(PacketDispatcher* dispatcher)
	: PacketHandler(dispatcher)
{
	GetDispatcher()->RegisterHandler(State::Play, play::MultiBlockChange, this);
	GetDispatcher()->RegisterHandler(State::Play, play::BlockChange, this);
	GetDispatcher()->RegisterHandler(State::Play, play::ChunkData, this);
	GetDispatcher()->RegisterHandler(State::Play, play::UnloadChunk, this);
	GetDispatcher()->RegisterHandler(State::Play, play::Explosion, this);
	GetDispatcher()->RegisterHandler(State::Play, play::UpdateBlockEntity, this);
	GetDispatcher()->RegisterHandler(State::Play, play::Respawn, this);
}

World::~World()
{
	GetDispatcher()->UnregisterHandler(this);
}

void World::HandlePacket(in::ExplosionPacket* packet)
{
	glm::dvec3 position = packet->GetPosition();

	for (glm::i16vec3 offset : packet->GetAffectedBlocks())
	{
		glm::dvec3 absolute = position + glm::dvec3(offset);

		const CMinecraftBlock* oldBlock = GetBlock(absolute);

		// Set all affected blocks to air
		SetBlock(glm::ivec3(absolute), 0);

		const CMinecraftBlock* newBlock = g_BlocksLoader->GetBlock(0);
		NotifyListeners(&WorldListener::OnBlockChange, glm::ivec3(absolute), newBlock, oldBlock);
	}
}

void World::HandlePacket(in::ChunkDataPacket* packet)
{
	std::shared_ptr<ChunkColumn> col = packet->GetChunkColumn();
	const ChunkColumnMetadata& meta = col->GetMetadata();
	ChunkCoord key(meta.x, meta.z);

	if (meta.continuous && meta.sectionmask == 0)
	{
		m_Chunks[key] = nullptr;
		return;
	}

	auto iter = m_Chunks.find(key);

	if (!meta.continuous)
	{
		// This isn't an entire column of chunks, so just update the existing chunk column with the provided chunks.
		for (int16 i = 0; i < ChunkColumn::ChunksPerColumn; ++i)
		{
			// The section mask says whether or not there is data in this chunk.
			if (meta.sectionmask & (1 << i))
			{
				(*iter->second)[i] = (*col)[i];
			}
		}
	}
	else
	{
		// This is an entire column of chunks, so just replace the entire column with the new one.
		m_Chunks[key] = col;
	}

	for (int32 i = 0; i < ChunkColumn::ChunksPerColumn; ++i)
	{
		ChunkPtr chunk = (*col)[i];

		NotifyListeners(&WorldListener::OnChunkLoad, chunk, meta, i);
	}
}

void World::HandlePacket(in::MultiBlockChangePacket* packet)
{
	glm::ivec3 chunkStart(packet->GetChunkX() * 16, 0, packet->GetChunkZ() * 16);

	auto iter = m_Chunks.find(ChunkCoord(packet->GetChunkX(), packet->GetChunkZ()));
	if (iter == m_Chunks.end()) 
		return;

	std::shared_ptr<ChunkColumn> chunk = iter->second;
	if (chunk == nullptr)
		return;

	const auto& changes = packet->GetBlockChanges();
	for (const auto& change : changes)
	{
		glm::ivec3 relative(change.x, change.y, change.z);

		chunk->RemoveBlockEntity(chunkStart + relative);

		std::size_t index = change.y / 16;
		const CMinecraftBlock* oldBlock = g_BlocksLoader->GetBlock(0);
		if ((*chunk)[index] == nullptr)
		{
			(*chunk)[index] = std::make_shared<Chunk>();
		}
		else
		{
			oldBlock = chunk->GetBlock(relative);
		}

		const CMinecraftBlock* newBlock = g_BlocksLoader->GetBlock(change.blockData);
		if (newBlock == nullptr)
			printf("World::MultiBlockChangePacket: CMinecraftBlock ID not found '%d'.\r\n", change.blockData);

		glm::ivec3 blockChangePos = chunkStart + relative;
		relative.y %= 16;
		(*chunk)[index]->SetBlock(relative, newBlock);

		NotifyListeners(&WorldListener::OnBlockChange, blockChangePos, newBlock, oldBlock);
	}
}

void World::HandlePacket(in::BlockChangePacket* packet)
{
	const CMinecraftBlock* newBlock = g_BlocksLoader->GetBlock((uint16)packet->GetBlockId());
	const CMinecraftBlock* oldBlock = GetBlock(packet->GetPosition());

	SetBlock(packet->GetPosition(), packet->GetBlockId());

	NotifyListeners(&WorldListener::OnBlockChange, packet->GetPosition(), newBlock, oldBlock);

	std::shared_ptr<ChunkColumn> col = GetChunk(packet->GetPosition());
	if (col)
	{
		col->RemoveBlockEntity(packet->GetPosition());
	}
}

void World::HandlePacket(in::UpdateBlockEntityPacket* packet)
{
	glm::ivec3 pos = packet->GetPosition();

	std::shared_ptr<ChunkColumn> col = GetChunk(pos);
	if (false == col) 
		return;

	col->RemoveBlockEntity(pos);

	BlockEntityPtr entity = packet->GetBlockEntity();
	if (entity)
		col->AddBlockEntity(entity);
}

void World::HandlePacket(in::UnloadChunkPacket* packet)
{
	ChunkCoord coord(packet->GetChunkX(), packet->GetChunkZ());

	auto iter = m_Chunks.find(coord);

	if (iter == m_Chunks.end()) return;

	std::shared_ptr<ChunkColumn> chunk = iter->second;
	NotifyListeners(&WorldListener::OnChunkUnload, chunk);

	m_Chunks.erase(iter);
}

// Clear all chunks because the server will resend the chunks after this.
void World::HandlePacket(in::RespawnPacket* packet)
{
	for (auto entry : m_Chunks)
	{
		std::shared_ptr<ChunkColumn> chunk = entry.second;

		NotifyListeners(&WorldListener::OnChunkUnload, chunk);
	}
	m_Chunks.clear();
}

std::shared_ptr<ChunkColumn> World::GetChunk(glm::ivec3 pos) const
{
	int32 x = (int32)std::floor(pos.x / 16.0);
	int32 z = (int32)std::floor(pos.z / 16.0);

	ChunkCoord key(x, z);

	auto iter = m_Chunks.find(key);
	if (iter == m_Chunks.end()) 
		return nullptr;

	return iter->second;
}

const CMinecraftBlock* World::GetBlock(glm::vec3 pos) const
{
	return GetBlock(glm::ivec3((int64)std::floor(pos.x), (int64)std::floor(pos.y), (int64)std::floor(pos.z)));
}

const CMinecraftBlock* World::GetBlock(glm::dvec3 pos) const
{
	return GetBlock(glm::ivec3((int64)std::floor(pos.x), (int64)std::floor(pos.y), (int64)std::floor(pos.z)));
}

const CMinecraftBlock* World::GetBlock(glm::ivec3 pos) const
{
	std::shared_ptr<ChunkColumn> col = GetChunk(pos);
	if (col == nullptr) 
		return g_BlocksLoader->GetBlock(0);

	int64 x = pos.x % 16;
	int64 z = pos.z % 16;

	if (x < 0)
		x += 16;
	if (z < 0)
		z += 16;

	return col->GetBlock(glm::ivec3(x, pos.y, z));
}

BlockEntityPtr World::GetBlockEntity(glm::ivec3 pos) const
{
	std::shared_ptr<ChunkColumn> col = GetChunk(pos);

	if (!col) return nullptr;

	return col->GetBlockEntity(pos);
}

std::vector<BlockEntityPtr> World::GetBlockEntities() const
{
	std::vector<BlockEntityPtr> blockEntities;

	for (auto iter = m_Chunks.begin(); iter != m_Chunks.end(); ++iter)
	{
		if (iter->second == nullptr) continue;
		std::vector<BlockEntityPtr> chunkBlockEntities = iter->second->GetBlockEntities();
		if (chunkBlockEntities.empty()) continue;
		blockEntities.insert(blockEntities.end(), chunkBlockEntities.begin(), chunkBlockEntities.end());
	}

	return blockEntities;
}



//
// Private
//
bool World::SetBlock(glm::ivec3 position, uint32 blockData)
{
	std::shared_ptr<ChunkColumn> chunk = GetChunk(position);
	if (!chunk)
		return false;

	glm::ivec3 relative(position);

	relative.x %= 16;
	relative.y %= 16;
	relative.z %= 16;

	if (relative.x < 0)
		relative.x += 16;

	if (relative.z < 0)
		relative.z += 16;

	std::size_t index = (std::size_t)position.y / 16;
	if ((*chunk)[index] == nullptr)
	{
		ChunkPtr section = std::make_shared<Chunk>();

		(*chunk)[index] = section;
	}

	const CMinecraftBlock* block = g_BlocksLoader->GetBlock(blockData);
	//if (block == nullptr)
	//{
	//	printf("World::SetBlock: CMinecraftBlock ID not found '%d'.\r\n", blockData);
	//	return false;
	//}

	(*chunk)[index]->SetBlock(relative, block);
	return true;
}
