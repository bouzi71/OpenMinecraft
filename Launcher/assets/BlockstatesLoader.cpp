#include "stdafx.h"

// General
#include "BlockstatesLoader.h"

// Additional
#include "AssetCache.h"

#include <common/Json.h>

#include <block/Block.h>

namespace
{
	std::string GetBlockNameFromPath(const std::string& path)
	{
		auto ext_pos = path.find_last_of('.');
		auto pos = path.find_last_of('/');
		auto filename = path.substr(pos + 1);
		auto ext_size = path.size() - ext_pos;

		return filename.substr(0, filename.length() - ext_size);
	}

	std::vector<std::string> VariantNameToVariables(const std::string& VariantName)
	{
		std::string s = VariantName;
		std::string delimiter = ",";
		std::vector<std::string> result;

		size_t pos = 0;
		std::string token;
		while ((pos = s.find(delimiter)) != std::string::npos)
		{
			token = s.substr(0, pos);
			result.push_back(token);
			s.erase(0, pos + delimiter.length());
		}
		result.push_back(s);

		return result;
	}
}




//
// CMinecraftBlockstatesLoader
//
CMinecraftBlockstatesLoader::CMinecraftBlockstatesLoader(const IBaseManager& BaseManager, AssetCache& AssetCache)
	: m_BaseManager(BaseManager)
	, m_AssetCache(AssetCache)
{}

void CMinecraftBlockstatesLoader::Initialize()
{
	for (const auto& blockIt : m_AssetCache.GetBlocksLoader().GetAllBlocks())
	{
		bool zeroBlockLoaded = false;
		for (const auto& metaBlockIt : blockIt.second->GetBlockmetas())
		{
			if (metaBlockIt.first == 0)
				zeroBlockLoaded = true;
			LoadBlockstateByBlock(metaBlockIt.second);
		}

		if (false == zeroBlockLoaded)
			LoadBlockstateByBlock(blockIt.second);		
	}

	m_AssetCache.GetTexturesLoader().Generate();

	// Fancy graphics mode seems to do something different than using cullface.
	/*for (auto& model : m_Cache.GetBlockModels("leaves"))
	{
		for (auto& element : model->GetElements())
		{
			for (auto& face : element.GetFaces())
			{
				face.cull_face = BlockFace::None;
				face.tint_index = 1;
			}
		}
	}*/
}

const BlockVariant* CMinecraftBlockstatesLoader::GetVariant(const CMinecraftBlock* block) const
{
	const auto& blockVariantsIt = m_BlockVariants.find(block->GetID());
	if (blockVariantsIt == m_BlockVariants.end())
		return nullptr;

	return blockVariantsIt->second.get();
}

void CMinecraftBlockstatesLoader::AddVariant(std::unique_ptr<BlockVariant> variant)
{
	if (variant == nullptr)
		throw std::exception("AssetCache: Block is nullptr.");

	if (m_BlockVariants.find(variant->GetBlock()->GetID()) != m_BlockVariants.end())
		Log::Warn("CMinecraftBlockstatesLoader: Variant block '%s' already exists in m_BlockVariants.", variant->GetBlock()->GetName().c_str());

	m_BlockVariants[variant->GetBlock()->GetID()] = std::move(variant);
}




//
// Private
//
void CMinecraftBlockstatesLoader::LoadBlockstateByBlock(const CMinecraftBlock * ForBlock)
{
	if (ForBlock->GetName().empty())
		return;

	std::string blockStateFileFullName = "assets/minecraft/blockstates/" + ForBlock->GetName() + ".json";

	auto blockStateFile = m_BaseManager.GetManager<IFilesManager>()->Open(blockStateFileFullName);
	if (blockStateFile == nullptr)
	{
		Log::Error("AssetLoader::LoadBlockstateByBlock: Unable to open file '%s'.", blockStateFileFullName.c_str());
		return;
	}

	// file content
	json blockStateRoot;
	try
	{
		blockStateRoot = json::parse(std::string((char*)blockStateFile->getDataEx(), blockStateFile->getSize()));
	}
	catch (json::parse_error& e)
	{
		Log::Error("AssetLoader::LoadBlockstateByBlock: File '%s' parse error '%s'.", blockStateFileFullName.c_str(), e.what());
		return;
	}


	json variantsObject = blockStateRoot.value("variants", json());
	json multipartObject = blockStateRoot.value("multipart", json());

	// variants
	if (variantsObject.is_object())
	{
		std::map<std::string, json> variantsMap;
		for (const auto& kv : variantsObject.items())
			variantsMap[kv.key()] = kv.value();

		const auto& normalVariant = variantsMap.find("normal");
		if (normalVariant != variantsMap.end())
		{
			auto blockVariant = LoadBlockVariant(ForBlock, normalVariant->second);
			if (blockVariant != nullptr)
			{
				m_AssetCache.GetBlockstatesLoader().AddVariant(std::move(blockVariant));
			}
			else
			{
				Log::Error("Add variant error 1. '%s'.", ForBlock->GetName().c_str());
			}
		}
		else
		{
			for (const auto& variantMapIt : variantsMap)
			{
				const auto& variantArray = VariantNameToVariables(variantMapIt.first);
				if (false == ForBlock->IsVariablesMatch(variantArray))
					continue;

				auto blockVariant = LoadBlockVariant(ForBlock, variantMapIt.second);
				if (blockVariant != nullptr)
				{
					m_AssetCache.GetBlockstatesLoader().AddVariant(std::move(blockVariant));
				}
				else
				{
					Log::Error("Add variant error 2. '%s'.", ForBlock->GetName().c_str());
				}
			}
		}

		return;
	}
	// multipart
	else if (multipartObject.is_array())
	{
		for (const auto& kv : multipartObject)
		{
			//json whenPredicateObject = kv.value("when", json());
			//if (false == whenPredicateObject.is_object())
			{
				auto blockVariant = LoadBlockVariant(ForBlock, kv.value("apply", json()));
				if (blockVariant != nullptr)
				{
					m_AssetCache.GetBlockstatesLoader().AddVariant(std::move(blockVariant));
					return;
				}
				else
				{
					Log::Error("Add variant error 1. '%s'.", ForBlock->GetName().c_str());
				}
			}
		}

		return;
	}


	Log::Error("AssetLoader: Blockstate file '%s' don't contains 'variants'.", blockStateFileFullName.c_str());

	auto blockVariant = LoadDefaultVariantBlock(ForBlock);
	if (blockVariant != nullptr)
		m_AssetCache.GetBlockstatesLoader().AddVariant(std::move(blockVariant));
}

std::unique_ptr<BlockVariant> CMinecraftBlockstatesLoader::LoadBlockVariant(const CMinecraftBlock * Block, json VariantJSONObject)
{
	json usedVariant;

	if (VariantJSONObject.is_array())
	{
		for (const auto& modelIt : VariantJSONObject)
		{
			usedVariant = modelIt;
		}
	}
	else
	{
		usedVariant = VariantJSONObject;
	}

	json model_node = usedVariant.value("model", json());
	if (false == model_node.is_string())
	{
		Log::Error("AssetLoader::LoadBlockVariant: Variant don't contains 'model'.");
		return nullptr;
	}

	std::string modelName = model_node.get<std::string>();
	const BlockModel* model = m_AssetCache.GetModelsLoader().GetBlockModel("block/" + modelName + ".json");
	if (model == nullptr)
	{
		return std::move(LoadDefaultVariantBlock(Block));
	}

	std::unique_ptr<BlockVariant> variant = std::make_unique<BlockVariant>(model, Block);
	float x = usedVariant.value("x", 0.0f);
	float y = usedVariant.value("y", 0.0f);
	float z = usedVariant.value("z", 0.0f);
	variant->SetRotation(glm::vec3(x, y, z));

	return std::move(variant);
}

std::unique_ptr<BlockVariant> CMinecraftBlockstatesLoader::LoadDefaultVariantBlock(const CMinecraftBlock * Block)
{
	const BlockModel* model = m_AssetCache.GetModelsLoader().GetBlockModel("block/bedrock.json");
	if (model == nullptr)
	{
		model = m_AssetCache.GetModelsLoader().GetBlockModel("block/bedrock.json");
		if (model == nullptr)
		{
			Log::Error("AssetLoader::LoadBlockVariant: Could not find block model 'bedrock'.");
			return nullptr;
		}
	}

	std::unique_ptr<BlockVariant> variant = std::make_unique<BlockVariant>(model, Block);
	return std::move(variant);
}