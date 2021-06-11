#include "stdafx.h"

#include <world/Chunk.h>

#include <common/DataBuffer.h>

#include <algorithm>

Chunk::Chunk()
{
	m_BitsPerBlock = 4;
}

Chunk::Chunk(const Chunk& other)
{
	m_Data = other.m_Data;
	m_BitsPerBlock = other.m_BitsPerBlock;
}

Chunk& Chunk::operator=(const Chunk& other)
{
	m_Data = other.m_Data;
	m_BitsPerBlock = other.m_BitsPerBlock;
	return *this;
}

void Chunk::Load(DataBuffer& in, ChunkColumnMetadata* meta, int32 chunkIndex)
{
	in >> m_BitsPerBlock;

	CVarInt paletteLength;
	in >> paletteLength;

	m_Palette.reserve(paletteLength.GetInt());

	for (int32 i = 0; i < paletteLength.GetInt(); ++i)
	{
		CVarInt paletteValue;
		in >> paletteValue;
		m_Palette.push_back((uint16)paletteValue.GetInt());
	}

	CVarInt dataArrayLength;
	in >> dataArrayLength;

	m_Data.resize(dataArrayLength.GetInt());

	for (int32 i = 0; i < dataArrayLength.GetInt(); ++i)
	{
		uint64 data;
		in >> data;
		m_Data[i] = data;
	}

	static const int64 lightSize = 16 * 16 * 16 / 2;

	// CMinecraftBlock light data
	in.SetReadOffset(in.GetReadOffset() + lightSize);

	// Sky Light
	if (meta->skylight)
	{
		in.SetReadOffset(in.GetReadOffset() + lightSize);
	}
}

const CMinecraftBlock* Chunk::GetBlock(glm::ivec3 chunkPosition) const
{
	if (chunkPosition.x < 0 || chunkPosition.x > 15 || chunkPosition.y < 0 || chunkPosition.y > 15 || chunkPosition.z < 0 || chunkPosition.z > 15)
	{
		return BlockRegistry::GetInstance()->GetBlock(0);
	}

	const std::size_t index = (std::size_t)(chunkPosition.y * 16 * 16 + chunkPosition.z * 16 + chunkPosition.x);
	const int32 bitIndex = index * m_BitsPerBlock;
	const int32 startIndex = bitIndex / 64;
	const int32 endIndex = (((index + 1) * m_BitsPerBlock) - 1) / 64;
	const int32 startSubIndex = bitIndex % 64;

	const int64 maxValue = (1 << m_BitsPerBlock) - 1;
	uint32 value;

	if (startIndex == endIndex)
	{
		value = (uint32)((m_Data[startIndex] >> startSubIndex) & maxValue);
	}
	else
	{
		const int32 endSubIndex = 64 - startSubIndex;

		value = (uint32)(((m_Data[startIndex] >> startSubIndex) | (m_Data[endIndex] << endSubIndex)) & maxValue);
	}

	const uint16 blockType = m_BitsPerBlock < 9 ? m_Palette[value] : value;

	const CMinecraftBlock* block = BlockRegistry::GetInstance()->GetBlock(blockType);
	//if (block == nullptr)
	//	printf("World::GetBlock: CMinecraftBlock ID not found '%d'.\r\n", blockType);

	return block;
}

void Chunk::SetBlock(glm::ivec3 chunkPosition, const CMinecraftBlock* block)
{
	std::size_t index = (std::size_t)(chunkPosition.y * 16 * 16 + chunkPosition.z * 16 + chunkPosition.x);
	int32 bitIndex = index * m_BitsPerBlock;
	int32 startIndex = bitIndex / 64;
	int32 endIndex = (((index + 1) * m_BitsPerBlock) - 1) / 64;
	int32 startSubIndex = bitIndex % 64;

	int64 maxValue = (1 << m_BitsPerBlock) - 1;
	uint32 blockType = block->GetID();

	if (m_BitsPerBlock == 0)
	{
		m_BitsPerBlock = 4;
	}

	if (m_Data.empty())
	{
		m_Palette.push_back(0);
		uint32 size = (16 * 16 * 16) * m_BitsPerBlock / 64;

		m_Data.resize(size);
		memset(&m_Data[0], 0, size * sizeof(m_Data[0]));
	}

	auto iter = std::find_if(m_Palette.begin(), m_Palette.end(), [blockType](uint32 ptype) {
		return ptype == blockType;
	});

	if (iter == m_Palette.end())
		iter = m_Palette.insert(m_Palette.end(), blockType);

	int32 value = std::distance(m_Palette.begin(), iter);

	// Erase old value in data entry and OR with new data
	m_Data[startIndex] = (m_Data[startIndex] & ~(maxValue << startSubIndex)) | (((int64)value & maxValue) << startSubIndex);

	if (startIndex != endIndex)
	{
		int32 endSubIndex = 64 - startSubIndex;

		// Erase beginning of data and then OR with new data
		m_Data[endIndex] = (m_Data[endIndex] >> endSubIndex << endSubIndex) | ((int64)value & maxValue) >> endSubIndex;
	}
}

ChunkColumn::ChunkColumn(ChunkColumnMetadata metadata)
	: m_Metadata(metadata)
{
	for (std::size_t i = 0; i < m_Chunks.size(); ++i)
		m_Chunks[i] = nullptr;
}

const CMinecraftBlock* ChunkColumn::GetBlock(glm::ivec3 position)
{
	int32 chunkIndex = (int32)(position.y / 16);
	glm::ivec3 relativePosition(position.x, position.y % 16, position.z);

	if (chunkIndex < 0 || chunkIndex > 15 || !m_Chunks[chunkIndex]) 
		return BlockRegistry::GetInstance()->GetBlock(0);

	return m_Chunks[chunkIndex]->GetBlock(relativePosition);
}

BlockEntityPtr ChunkColumn::GetBlockEntity(glm::ivec3 worldPos)
{
	auto iter = m_BlockEntities.find(worldPos);
	if (iter == m_BlockEntities.end()) return nullptr;
	return iter->second;
}

std::vector<BlockEntityPtr> ChunkColumn::GetBlockEntities()
{
	std::vector<BlockEntityPtr> blockEntities;

	for (auto iter = m_BlockEntities.begin(); iter != m_BlockEntities.end(); ++iter)
		blockEntities.push_back(iter->second);

	return blockEntities;
}

DataBuffer& operator>>(DataBuffer& in, ChunkColumn& column)
{
	ChunkColumnMetadata* meta = &column.m_Metadata;

	for (int16 i = 0; i < ChunkColumn::ChunksPerColumn; ++i)
	{
		// The section mask says whether or not there is data in this chunk.
		if (meta->sectionmask & (1 << i))
		{
			column.m_Chunks[i] = std::make_shared<Chunk>();

			column.m_Chunks[i]->Load(in, meta, i);
		}
		else
		{
			// Air section, leave null
			column.m_Chunks[i] = nullptr;
		}
	}

	return in;
}
