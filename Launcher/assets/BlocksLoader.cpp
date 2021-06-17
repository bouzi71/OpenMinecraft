#include "stdafx.h"

// General
#include "BlocksLoader.h"

// Additional
#include "AssetCache.h"

#include <common/Json.h>


IMinecraftBlocksLoader * g_BlocksLoader = nullptr;

CMinecraftBlocksLoader::CMinecraftBlocksLoader(const IBaseManager & BaseManager, AssetCache & AssetCache)
	: m_BaseManager(BaseManager)
	, m_AssetCache(AssetCache)
{
	g_BlocksLoader = this;
	SetBlocksLoader(this);
}

CMinecraftBlocksLoader::~CMinecraftBlocksLoader()
{}

const CMinecraftBlock* CMinecraftBlocksLoader::GetBlock(uint32 ID) const
{
	//const auto& blocksIt = m_Blocks.find(ID);
	//if (blocksIt == m_Blocks.end())
	//	return nullptr;

	//return blocksIt->second;

	uint32 type = ID >> 4;
	uint32 meta = ID & 15;

	const auto& blocksIt = m_Blocks.find(type);
	if (blocksIt == m_Blocks.end())
		return nullptr;

	CMinecraftBlock* block = blocksIt->second;
	if (block == nullptr)
		return nullptr;

	const CMinecraftBlock* blockMeta = block->GetBlockmeta(meta);
	if (blockMeta == nullptr)
		return block;

	return blockMeta;
}

const CMinecraftBlock* CMinecraftBlocksLoader::GetBlock(uint16 type, uint16 meta) const
{
	return GetBlock(type << 4 | (meta & 15));
}

void CMinecraftBlocksLoader::RegisterBlockInfos()
{
	auto blockInfosFile = m_BaseManager.GetManager<IFilesManager>()->Open("Blocks_info.json");
	if (blockInfosFile == nullptr)
		return;

	json root;
	try
	{
		root = json::parse(std::string((char*)blockInfosFile->getData(), blockInfosFile->getSize()));
	}
	catch (json::parse_error& e)
	{
		Log::Error("BlockInfos: Parse error '%s'.", e.what());
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

		m_BlockInfos.insert(std::make_pair(minecraftName, SBlockInfo(isTransperent, isSolid)));
	}

}

void CMinecraftBlocksLoader::RegisterVanillaBlocks()
{
	const BoundingBox FullSolidBounds(glm::dvec3(0, 0, 0), glm::dvec3(1, 1, 1));

	RegisterBlockInfos();

	auto blocksFile = m_BaseManager.GetManager<IFilesManager>()->Open("Blocks.json");
	if (blocksFile == nullptr)
		return;

	json root;
	try
	{
		root = json::parse(std::string((char*)blocksFile->getData(), blocksFile->getSize()));
	}
	catch (json::parse_error& e)
	{
		throw std::exception("Unable to parse 'Blocks.json'.");
	}

	if (false == root.is_array())
	{
		throw std::exception("Root item in 'Blocks.json' is not json array.");
	}

	for (const auto& blockJSONIt : root.items())
	{
		json blockJSON = blockJSONIt.value();

		uint32 blockId = blockJSON.value("id", 0);

		std::string displayName = blockJSON.value("display_name", "");
		_ASSERT(false == displayName.empty());

		std::string minecraftName = blockJSON.value("name", "");
		_ASSERT(false == minecraftName.empty());

		bool isTransperent = false;
		bool isSolid = false;

		auto blockInfoIt = m_BlockInfos.find(minecraftName);
		if (blockInfoIt != m_BlockInfos.end())
		{
			isTransperent = blockInfoIt->second.IsTransperent;
			isSolid = blockInfoIt->second.IsSolid;
		}

		// Remove "minecraft:" prefix
		const auto& minecraftTagPosition = minecraftName.find_first_of("minecraft:");
		if (minecraftTagPosition != std::string::npos)
			minecraftName = minecraftName.substr(minecraftTagPosition + std::string("minecraft:").length());


		CMinecraftBlock* block = new CMinecraftBlock(minecraftName, blockId, 0, isTransperent, isSolid);

		json metadataArray = blockJSON.value("metadata", json());
		if (metadataArray.is_array())
		{
			for (const auto& metadataJSONIt : metadataArray.items())
			{
				json metadataJSON = metadataJSONIt.value();

				uint32 metaID = metadataJSON.value("value", 0);
				json variablesJSON = metadataJSON.value("variables", json());
				std::string blockState = metadataJSON.value("blockstate", "");

				//std::string blockMetaName = "";
				//if (false == blockState.empty())
				//	blockMetaName = "minecraft:" + blockState;

				CMinecraftBlock* blockMeta = new CMinecraftBlock(blockState, blockId, metaID, isTransperent, isSolid);

				BoundingBox bounds = blockMeta->GetBoundingBox();
				if (blockMeta->IsSolid() && glm::length(bounds.getMax() - bounds.getMin()) == 0)
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

		RegisterBlock(block);

		BoundingBox bounds = block->GetBoundingBox();
		if (/*block->IsSolid() && */glm::length(bounds.getMax() - bounds.getMin()) == 0)
			block->SetBoundingBox(FullSolidBounds);
	}
}

void CMinecraftBlocksLoader::RegisterVanillaBlocks2()
{
	const BoundingBox FullSolidBounds(glm::dvec3(0, 0, 0), glm::dvec3(1, 1, 1));

	RegisterBlockInfos();

	std::ifstream t("1.13.2/custom/Blocks.json");
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

	//if (false == root.is_array())
	//{
	//	throw std::exception("Root item in 'Blocks.json' is not json array.");
	//}

	for (const auto& blockJSONIt : root.items())
	{
		std::string blockName = blockJSONIt.key();
		json blockJSON = blockJSONIt.value();

		bool isTransperent = false;
		bool isSolid = false;

		auto blockInfoIt = m_BlockInfos.find(blockName);
		if (blockInfoIt != m_BlockInfos.end())
		{
			isTransperent = blockInfoIt->second.IsTransperent;
			isSolid = blockInfoIt->second.IsSolid;
		}

		// Remove "minecraft:" prefix
		const auto& minecraftTagPosition = blockName.find_first_of("minecraft:");
		if (minecraftTagPosition != std::string::npos)
			blockName = blockName.substr(minecraftTagPosition + std::string("minecraft:").length());

		json statesJSONArray = blockJSON.value("states", json());
		if (false == statesJSONArray.is_array())
			throw std::exception(("Sates item for block " + blockName + "in 'Blocks.json' is not json array.").c_str());

		for (const auto& stateJSON : statesJSONArray)
		{
			uint32 blockId = stateJSON.value("id", 0);

			CMinecraftBlock* block = new CMinecraftBlock(blockName, blockId, isTransperent, isSolid);

			json propertiesJSON = stateJSON.value("properties", json());
			if (propertiesJSON.is_object())
			{
				for (const auto& propertyJSON : propertiesJSON.items())
				{
					std::string propertyJSONName = propertyJSON.key();
					std::string propertyJSONValue = propertyJSON.value().get<std::string>();

					block->AddVariable(propertyJSONName + "=" + propertyJSONValue);
				}
			}

			RegisterBlock(block);

			BoundingBox bounds = block->GetBoundingBox();
			if (block->IsSolid() && glm::length(bounds.getMax() - bounds.getMin()) == 0)
				block->SetBoundingBox(FullSolidBounds);
		}



		/*json metadataArray = blockJSON.value("metadata", json());
		if (metadataArray.is_array())
		{
			for (const auto& metadataJSONIt : metadataArray.items())
			{
				json metadataJSON = metadataJSONIt.value();

				uint32 metaID = metadataJSON.value("value", 0);
				json variablesJSON = metadataJSON.value("variables", json());
				std::string blockState = metadataJSON.value("blockstate", "");

				//std::string blockMetaName = "";
				//if (false == blockState.empty())
				//	blockMetaName = "minecraft:" + blockState;

				CMinecraftBlock* blockMeta = new CMinecraftBlock(blockState, blockId, metaID, isTransperent, isSolid);

				CMinecraftAABB bounds = blockMeta->GetBoundingBox();
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
		}*/

	}
}

void CMinecraftBlocksLoader::ClearRegistry()
{
	for (auto& pair : m_Blocks)
		delete pair.second;
	m_Blocks.clear();
}

const std::map<uint32, CMinecraftBlock*>& CMinecraftBlocksLoader::GetAllBlocks() const noexcept
{
	return m_Blocks;
}



//
// Private
//
void CMinecraftBlocksLoader::RegisterBlock(CMinecraftBlock* block)
{
	if (block == nullptr)
		throw std::exception("BlockRegistry: CMinecraftBlock is nullptr.");

	const auto& blockIt = m_Blocks.find(block->GetType());
	if (blockIt == m_Blocks.end())
	{
		//std::cout << "BlockRegistry: CMinecraftBlock '" << block->GetName() << "' registered in m_Blocks with ID: '" << block->GetID() << "'. Type '" << block->GetType() << "'. Meta: '" << block->GetMeta() << "'." << std::endl;
		m_Blocks[block->GetType()] = block;
	}
	else
	{
		Log::Error("BlockRegistry: CMinecraftBlock '%s' already registered in m_Blocks with ID: '%d'.", block->GetName().c_str(), block->GetID());
	}
}
