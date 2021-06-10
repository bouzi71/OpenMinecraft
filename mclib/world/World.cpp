#include "stdafx.h"

#include <world/World.h>

World::World(PacketDispatcher* dispatcher)
	: PacketHandler(dispatcher)
{
	dispatcher->RegisterHandler(State::Play, play::MultiBlockChange, this);
	dispatcher->RegisterHandler(State::Play, play::BlockChange, this);
	dispatcher->RegisterHandler(State::Play, play::ChunkData, this);
	dispatcher->RegisterHandler(State::Play, play::UnloadChunk, this);
	dispatcher->RegisterHandler(State::Play, play::Explosion, this);
	dispatcher->RegisterHandler(State::Play, play::UpdateBlockEntity, this);
	dispatcher->RegisterHandler(State::Play, play::Respawn, this);
}

World::~World()
{
	GetDispatcher()->UnregisterHandler(this);
}

bool World::SetBlock(Vector3i position, u32 blockData)
{
	std::shared_ptr<ChunkColumn> chunk = GetChunk(position);
	if (!chunk) 
		return false;

	Vector3i relative(position);

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

	const CMinecraftBlock* block = BlockRegistry::GetInstance()->GetBlock(blockData);
	//if (block == nullptr)
	//{
	//	printf("World::SetBlock: CMinecraftBlock ID not found '%d'.\r\n", blockData);
	//	return false;
	//}

	(*chunk)[index]->SetBlock(relative, block);
	return true;
}

void World::HandlePacket(in::ExplosionPacket* packet)
{
	Vector3d position = packet->GetPosition();

	for (Vector3s offset : packet->GetAffectedBlocks())
	{
		Vector3d absolute = position + ToVector3d(offset);

		const CMinecraftBlock* oldBlock = GetBlock(absolute);

		// Set all affected blocks to air
		SetBlock(ToVector3i(absolute), 0);

		const CMinecraftBlock* newBlock = BlockRegistry::GetInstance()->GetBlock(0);
		NotifyListeners(&WorldListener::OnBlockChange, ToVector3i(absolute), newBlock, oldBlock);
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
		for (s16 i = 0; i < ChunkColumn::ChunksPerColumn; ++i)
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

	for (s32 i = 0; i < ChunkColumn::ChunksPerColumn; ++i)
	{
		ChunkPtr chunk = (*col)[i];

		NotifyListeners(&WorldListener::OnChunkLoad, chunk, meta, i);
	}
}

void World::HandlePacket(in::MultiBlockChangePacket* packet)
{
	Vector3i chunkStart(packet->GetChunkX() * 16, 0, packet->GetChunkZ() * 16);

	auto iter = m_Chunks.find(ChunkCoord(packet->GetChunkX(), packet->GetChunkZ()));
	if (iter == m_Chunks.end()) 
		return;

	std::shared_ptr<ChunkColumn> chunk = iter->second;
	if (chunk == nullptr)
		return;

	const auto& changes = packet->GetBlockChanges();
	for (const auto& change : changes)
	{
		Vector3i relative(change.x, change.y, change.z);

		chunk->RemoveBlockEntity(chunkStart + relative);

		std::size_t index = change.y / 16;
		const CMinecraftBlock* oldBlock = BlockRegistry::GetInstance()->GetBlock(0);
		if ((*chunk)[index] == nullptr)
		{
			(*chunk)[index] = std::make_shared<Chunk>();
		}
		else
		{
			oldBlock = chunk->GetBlock(relative);
		}

		const CMinecraftBlock* newBlock = BlockRegistry::GetInstance()->GetBlock(change.blockData);
		if (newBlock == nullptr)
			printf("World::MultiBlockChangePacket: CMinecraftBlock ID not found '%d'.\r\n", change.blockData);

		Vector3i blockChangePos = chunkStart + relative;
		relative.y %= 16;
		(*chunk)[index]->SetBlock(relative, newBlock);

		NotifyListeners(&WorldListener::OnBlockChange, blockChangePos, newBlock, oldBlock);
	}
}

void World::HandlePacket(in::BlockChangePacket* packet)
{
	const CMinecraftBlock* newBlock = BlockRegistry::GetInstance()->GetBlock((u16)packet->GetBlockId());
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
	Vector3i pos = packet->GetPosition();

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

std::shared_ptr<ChunkColumn> World::GetChunk(Vector3i pos) const
{
	s32 x = (s32)std::floor(pos.x / 16.0);
	s32 z = (s32)std::floor(pos.z / 16.0);

	ChunkCoord key(x, z);

	auto iter = m_Chunks.find(key);

	if (iter == m_Chunks.end()) return nullptr;

	return iter->second;
}

const CMinecraftBlock* World::GetBlock(Vector3f pos) const
{
	return GetBlock(Vector3i((s64)std::floor(pos.x), (s64)std::floor(pos.y), (s64)std::floor(pos.z)));
}

const CMinecraftBlock* World::GetBlock(Vector3d pos) const
{
	return GetBlock(Vector3i((s64)std::floor(pos.x), (s64)std::floor(pos.y), (s64)std::floor(pos.z)));
}

const CMinecraftBlock* World::GetBlock(Vector3i pos) const
{
	std::shared_ptr<ChunkColumn> col = GetChunk(pos);
	if (!col) 
		return BlockRegistry::GetInstance()->GetBlock(0);

	s64 x = pos.x % 16;
	s64 z = pos.z % 16;

	if (x < 0)
		x += 16;
	if (z < 0)
		z += 16;

	return col->GetBlock(Vector3i(x, pos.y, z));
}

BlockEntityPtr World::GetBlockEntity(Vector3i pos) const
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
