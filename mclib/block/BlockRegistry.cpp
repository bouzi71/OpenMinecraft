#include "stdafx.h"

// General
#include "BlockRegistry.h"

// Additional
#include <common/Json.h>

// Additional
#include <fstream>
#include <streambuf>
#include <iostream>

namespace
{
struct SBlockInfo
{
	SBlockInfo(bool IsTransperent, bool IsSolid)
		: IsTransperent(IsTransperent)
		, IsSolid(IsSolid)
	{}

	bool IsTransperent;
	bool IsSolid;
};

std::map<std::string, SBlockInfo> blockInfos;

void BlockInfos()
{
	std::ifstream t("1.12.2/custom/Blocks_info.json");
	if (false == t.is_open())
		throw std::exception("Unable to open 'Blocks_info.json'.");

	std::string contents((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());

	json root;
	try
	{
		root = json::parse(contents);
	}
	catch (json::parse_error& e)
	{
		std::cout << e.what() << std::endl;
		return;
	}

	if (false == root.is_array())
	{
		printf("Blocks_info.json root is not array.");
		return;
	}

	for (const auto& blockInfoJSONIt : root.items())
	{
		json blockInfoJSON = blockInfoJSONIt.value();

		std::string minecraftName = blockInfoJSON.value("name", "<unknown>");
		bool isTransperent = blockInfoJSON.value("transparent", false);
		bool isSolid = blockInfoJSON.value("solid", false);

		blockInfos.insert(std::make_pair(minecraftName, SBlockInfo(isTransperent, isSolid)));
	}
}

} // ns

BlockRegistry* BlockRegistry::GetInstance()
{
	static BlockRegistry registry;
	return &registry;
}

BlockRegistry::~BlockRegistry()
{
	ClearRegistry();
}


const Block* BlockRegistry::GetBlock(u32 ID) const
{
	//const auto& blocksIt = m_Blocks.find(ID);
	//if (blocksIt == m_Blocks.end())
	//	return nullptr;

	//return blocksIt->second;

	u32 type = ID >> 4;
	u32 meta = ID & 15;

	const auto& blocksIt = m_Blocks.find(type);
	if (blocksIt == m_Blocks.end())
		return nullptr;

	Block* block = blocksIt->second;
	if (block == nullptr)
		return nullptr;

	const Block* blockMeta = block->GetBlockmeta(meta);
	if (blockMeta == nullptr)
		return block;

	return blockMeta;
}

const Block* BlockRegistry::GetBlock(u16 type, u16 meta) const
{
	return GetBlock(type << 4 | (meta & 15));
}

void BlockRegistry::RegisterVanillaBlocks()
{
	const AABB FullSolidBounds(Vector3d(0, 0, 0), Vector3d(1, 1, 1));

	BlockInfos();

	std::ifstream t("1.12.2/custom/Blocks.json");
	if (false == t.is_open())
		throw std::exception("Unable to open 'Blocks.json'.");

	std::string contents((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());

	json root;
	try
	{
		root = json::parse(contents);
	}
	catch (json::parse_error& e)
	{
		throw std::exception("Unable to parse 'Blocks.json'.");
	}

	if (false == root.is_array())
	{
		throw std::exception("Root item in 'Blocks.json' is not json array.");
	}

	BlockRegistry* registry = BlockRegistry::GetInstance();

	for (const auto& blockJSONIt : root.items())
	{
		json blockJSON = blockJSONIt.value();

		u32 blockId = blockJSON.value("id", 0);

		std::string displayName = blockJSON.value("display_name", "");
		_ASSERT(false == displayName.empty());

		std::string minecraftName = blockJSON.value("name", "");
		_ASSERT(false == minecraftName.empty());

		bool isTransperent = false;
		bool isSolid = false;

		auto blockInfoIt = blockInfos.find(minecraftName);
		if (blockInfoIt != blockInfos.end())
		{
			isTransperent = blockInfoIt->second.IsTransperent;
			isSolid = blockInfoIt->second.IsSolid;
		}

		// Remove "minecraft:" prefix
		const auto& minecraftTagPosition = minecraftName.find_first_of("minecraft:");
		if (minecraftTagPosition != std::string::npos)
			minecraftName = minecraftName.substr(minecraftTagPosition + std::string("minecraft:").length());


		Block* block = new Block(minecraftName, blockId, 0, isTransperent, isSolid);

		json metadataArray = blockJSON.value("metadata", json());
		if (metadataArray.is_array())
		{
			for (const auto& metadataJSONIt : metadataArray.items())
			{
				json metadataJSON = metadataJSONIt.value();

				u32 metaID = metadataJSON.value("value", 0);
				json variablesJSON = metadataJSON.value("variables", json());
				std::string blockState = metadataJSON.value("blockstate", "");
			
				//std::string blockMetaName = "";
				//if (false == blockState.empty())
				//	blockMetaName = "minecraft:" + blockState;

				Block* blockMeta = new Block(blockState, blockId, metaID, isTransperent, isSolid);

				AABB bounds = blockMeta->GetBoundingBox();
				if (blockMeta->IsSolid() && (bounds.max - bounds.min).Length() == 0)
					blockMeta->SetBoundingBox(FullSolidBounds);

				if (variablesJSON.is_array())
				{
					for (const auto& variableJSON : variablesJSON)
					{
						if (false == variableJSON.is_string())
							continue;

						std::string variableAsString = variableJSON.get<std::string>();
						if (variableAsString.empty())
							continue;

						blockMeta->AddVariable(variableAsString);
					}
				}

				//std::cout << "BlockRegistry:    Blockmeta '" << blockMeta->GetName() << "' added to block '" << block->GetName() << "' with ID: '" << blockMeta->GetID() << "'. Type '" << blockMeta->GetType() << "'. Meta: '" << blockMeta->GetMeta() << "'." << std::endl;
				block->AddBlockmeta(blockMeta);

				//registry->RegisterBlock(blockMeta);
			}
		}

		registry->RegisterBlock(block);

		AABB bounds = block->GetBoundingBox();
		if (block->IsSolid() && (bounds.max - bounds.min).Length() == 0)
			block->SetBoundingBox(FullSolidBounds);
	}
}

void BlockRegistry::ClearRegistry()
{
	for (auto& pair : m_Blocks)
		delete pair.second;
	m_Blocks.clear();
}

const std::map<u32, Block*>& BlockRegistry::GetAllBlocks() const noexcept
{
	return m_Blocks;
}



//
// Private
//
BlockRegistry::BlockRegistry()
{}

void BlockRegistry::RegisterBlock(Block* block)
{
	if (block == nullptr)
		throw std::exception("BlockRegistry: Block is nullptr.");

	const auto& blockIt = m_Blocks.find(block->GetType());
	if (blockIt == m_Blocks.end())
	{
		//std::cout << "BlockRegistry: Block '" << block->GetName() << "' registered in m_Blocks with ID: '" << block->GetID() << "'. Type '" << block->GetType() << "'. Meta: '" << block->GetMeta() << "'." << std::endl;
		m_Blocks[block->GetType()] = block;
	}
	else
	{
		std::cout << "BlockRegistry: Block '" << block->GetName() << "' already registered in m_Blocks with ID: '" << block->GetID() << "'. Type '" << block->GetType() << "'. Meta: '" << block->GetMeta() << "'." << std::endl;
	}
}
