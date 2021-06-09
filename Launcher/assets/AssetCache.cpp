#include "stdafx.h"

#include "AssetCache.h"

#include "zip/ZipArchive.h"

#include <mclib/common/Json.h>
#include <mclib/block/Block.h>

#include <fstream>
#include <iostream>
#include <algorithm>

#include "stb_image.h"


namespace terra
{

BlockModel* AssetCache::GetBlockModel(const std::string& path)
{
	auto iter = m_BlockModels.find(path);
	if (iter == m_BlockModels.end()) 
		return nullptr;

	return iter->second.get();
}

std::vector<BlockModel*> AssetCache::GetBlockModels(const std::string& find)
{
	std::vector<BlockModel*> result;

	for (auto& kv : m_BlockModels)
	{
		if (kv.first.find(find) != std::string::npos)
		{
			result.push_back(kv.second.get());
		}
	}

	return result;
}

TextureHandle AssetCache::AddTexture(const std::string& path, const std::string& data)
{
	return m_TextureArray.Append(path, data);
}

void AssetCache::AddBlockModel(const std::string& path, std::unique_ptr<BlockModel> model)
{
	if (m_BlockModels.find(path) != m_BlockModels.end())
	{
		printf("ASDASDASDSADASDASDASDASDASDAS %s\r\n", path.c_str());
		return;
	}
	m_BlockModels[path] = std::move(model);
}

void AssetCache::AddVariantModel(std::unique_ptr<BlockVariant> variant)
{
	if (variant == nullptr)
		throw std::exception("AssetCache: Block is nullptr.");

	if (m_BlockVariants.find(variant->GetBlock()->GetID()) != m_BlockVariants.end())
		std::cout << "AssetCache: Variant block '" << variant->GetBlock()->GetName() << "' already exists in m_BlockVariants." << std::endl;

	m_BlockVariants[variant->GetBlock()->GetID()] = std::move(variant);
}

BlockVariant* AssetCache::GetVariant(const Block* block)
{
	//const auto& variantCacheIt = m_VariantCache.find(block->GetID());
	//if (variantCacheIt == m_VariantCache.end())
	//{
		const auto& blockVariantsIt = m_BlockVariants.find(block->GetID());
		if (blockVariantsIt == m_BlockVariants.end())
			return nullptr;

		//m_VariantCache[block->GetID()] = (blockVariantsIt->second).get();
		return blockVariantsIt->second.get();
	//}
	//else
	//{
	//	return (variantCacheIt->second);
	//}
}

} // ns terra