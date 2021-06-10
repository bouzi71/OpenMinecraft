#include "stdafx.h"

#include "AssetLoader.h"
#include "AssetCache.h"

#include <common/Json.h>
#include "zip/ZipArchive.h"

#include "stb_image.h"



#include <iostream>
#include <fstream>

#include <block/Block.h>
#include <block/BlockRegistry.h>

namespace terra
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

glm::vec3 GetVectorFromJson(const json& node)
{
	glm::vec3 result(0, 0, 0);

	int index = 0;

	for (auto& n : node)
	{
		if (n.is_number())
		{
			result[index++] = n.get<glm::vec3::value_type>() / static_cast<glm::vec3::value_type>(16.0);
		}
	}

	return result;
}

std::pair<glm::vec2, glm::vec2> GetUVFromJson(const json& node)
{
	glm::vec2 from(0, 0);
	glm::vec2 to(1, 1);

	int index = 0;

	for (auto& n : node)
	{
		if (n.is_number())
		{
			if (index < 2)
			{
				from[index++] = n.get<glm::vec3::value_type>() / static_cast<glm::vec3::value_type>(16.0);
			}
			else
			{
				to[index - 2] = n.get<glm::vec3::value_type>() / static_cast<glm::vec3::value_type>(16.0);
				++index;
			}
		}
	}

	return std::make_pair<>(from, to);
}



AssetLoader::AssetLoader(AssetCache& cache)
	: m_Cache(cache)
{}

bool AssetLoader::LoadArchive(const std::string& archive_path)
{
	terra::ZipArchive archive;

	if (!archive.Open(archive_path.c_str()))
	{
		std::cout << "Failed to open archive.\n";
		return false;
	}

	LoadBlockVariants(archive);

	m_Cache.GetTextures().Generate();

	// Fancy graphics mode seems to do something different than using cullface.
	for (auto& model : m_Cache.GetBlockModels("leaves"))
	{
		for (auto& element : model->GetElements())
		{
			for (auto& face : element.GetFaces())
			{
				face.cull_face = BlockFace::None;
				face.tint_index = 1;
			}
		}
	}

	return true;
}

bool AssetLoader::LoadTexture(terra::ZipArchive& archive, const std::string& path, TextureHandle* handle)
{
	if (m_Cache.GetTextures().GetTexture(path, handle)) 
		return true;

	std::string texture_path = "assets/minecraft/textures/" + path;

	std::size_t texture_size;
	char* texture_raw = archive.ReadFile(texture_path.c_str(), &texture_size);

	if (texture_raw == nullptr) 
		return false;

	int width, height, channels;
	unsigned char* image = stbi_load_from_memory(reinterpret_cast<unsigned char*>(texture_raw), (int)texture_size, &width, &height, &channels, STBI_rgb_alpha);

	archive.FreeFileData(texture_raw);

	if (image == nullptr) 
		return false;

	if (width == 16)
	{
		std::size_t size = 16 * 16 * 4;

		*handle = m_Cache.AddTexture(path, std::string(reinterpret_cast<char*>(image), size));
	}

	stbi_image_free(image);

	return true;
}



std::unique_ptr<BlockModel> AssetLoader::LoadBlockModel(terra::ZipArchive& archive, const std::string& path /* block/sdfsdfsdf.json */, TextureMap& texture_map, std::vector<IntermediateElement>& intermediates)
{
	std::string blockStateFileContent = "";

	size_t size;
	char* raw = archive.ReadFile(("assets/minecraft/models/" + path).c_str(), &size);
	if (raw == nullptr)
	{
		std::ifstream t("1.12.2/custom/models/" + path);

		if (false == t.is_open())
		{
			printf("AssetLoader::LoadBlockModel: File '%s' not found in archive.\r\n", path.c_str());
			return nullptr;
		}
		else
		{
			blockStateFileContent = std::string((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
		}
	}
	else
	{
		blockStateFileContent = std::string(raw, size);
		archive.FreeFileData(raw);
	}

	json root;
	try
	{
		root = json::parse(blockStateFileContent);
	}
	catch (json::parse_error& e)
	{
		std::cout << e.what() << std::endl;
		return nullptr;
	}

	auto parent_node = root.value("parent", json());
	if (parent_node.is_string())
	{
		std::string parent_path = parent_node.get<std::string>() + ".json";

		LoadBlockModel(archive, parent_path, texture_map, intermediates);
	}

	auto model = std::make_unique<BlockModel>();

	// Load each texture listed as part of this model.
	auto textures_node = root.value("textures", json());
	for (auto& kv : textures_node.items())
	{
		std::string texture_path = kv.value().get<std::string>();
		texture_map[kv.key()] = texture_path;
	}

	auto elements_node = root.value("elements", json());

	// Create each element of the model and resolve texture linking
	for (auto& element_node : elements_node)
	{
		glm::vec3 from = GetVectorFromJson(element_node.value("from", json()));
		glm::vec3 to = GetVectorFromJson(element_node.value("to", json()));

		IntermediateElement element;
		element.from = from;
		element.to = to;
		element.shade = element_node.value("shade", true);

		auto rotation_node = element_node.value("rotation", json());

		if (rotation_node.is_object())
		{
			element.rotation.origin = GetVectorFromJson(rotation_node.value("origin", json()));
			std::string axis = rotation_node.value("axis", "y");

			if (axis == "x")
			{
				element.rotation.axis = glm::vec3(1, 0, 0);
			}
			else if (axis == "y")
			{
				element.rotation.axis = glm::vec3(0, 1, 0);
			}
			else
			{
				element.rotation.axis = glm::vec3(0, 0, 1);
			}

			element.rotation.angle = rotation_node.value("angle", 0.0f);
			element.rotation.rescale = rotation_node.value("rescale", false);
		}

		auto faces_node = element_node.value("faces", json());
		for (auto& kv : faces_node.items())
		{
			BlockFace face = face_from_string(kv.key());
			auto face_node = kv.value();

			auto texture_node = face_node.value("texture", json());
			if (texture_node.is_string())
			{
				std::string texture = texture_node.get<std::string>();

				IntermediateFace renderable;

				renderable.texture = texture;
				renderable.face = face;
				renderable.cull_face = face_from_string(face_node.value("cullface", ""));
				renderable.tint_index = face_node.value("tintindex", -1);

				std::tie(renderable.uv_from, renderable.uv_to) = GetUVFromJson(face_node.value("uv", json()));

				element.faces.push_back(renderable);
			}
		}

		intermediates.push_back(element);
	}

	return model;
}

BlockModel* AssetLoader::LoadBlockModel(terra::ZipArchive& archive, const std::string& ModelName)
{
	TextureMap texture_map;
	std::vector<IntermediateElement> intermediates;

	auto model = LoadBlockModel(archive, ModelName, texture_map, intermediates);

	for (const IntermediateElement& intermediate : intermediates)
	{
		BlockElement element(intermediate.from, intermediate.to);
		element.SetShouldShade(intermediate.shade);
		element.GetRotation() = intermediate.rotation;

		for (const auto& intermediate_renderable : intermediate.faces)
		{
			std::string texture = intermediate_renderable.texture;

			while (false == texture.empty() && texture[0] == '#')
			{
				texture = texture_map[texture.substr(1)];
			}

			TextureHandle handle;
			if (LoadTexture(archive, texture + ".png", &handle))
			{
				RenderableFace renderable;
				renderable.face = intermediate_renderable.face;
				renderable.cull_face = intermediate_renderable.cull_face;
				renderable.texture = handle;
				renderable.tint_index = intermediate_renderable.tint_index;
				renderable.uv_from = intermediate_renderable.uv_from;
				renderable.uv_to = intermediate_renderable.uv_to;

				element.AddFace(renderable);
			}
		}

		model->AddElement(element);
	}

	m_Cache.AddBlockModel(ModelName, std::move(model));
	return m_Cache.GetBlockModel(ModelName);
}



std::unique_ptr<BlockVariant> AssetLoader::LoadDefaultVariantBlock(terra::ZipArchive & archive, const CMinecraftBlock * Block)
{
	BlockModel* model = m_Cache.GetBlockModel("block/bedrock.json");
	if (model == nullptr)
	{
		model = LoadBlockModel(archive, "block/bedrock.json");
		if (model == nullptr)
		{
			std::cout << "AssetLoader::LoadBlockVariant: Could not find block model 'bedrock'." << std::endl;
			return nullptr;
		}
	}

	std::unique_ptr<BlockVariant> variant = std::make_unique<BlockVariant>(model, Block);
	return std::move(variant);
}

std::unique_ptr<BlockVariant> AssetLoader::LoadBlockVariant(terra::ZipArchive& archive, const CMinecraftBlock * Block, json VariantJSONObject)
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
		std::cout << "AssetLoader::LoadBlockVariant: Variant don't contains 'model' json." << std::endl;
		return nullptr;
	}

	std::string modelName = model_node.get<std::string>();
	BlockModel* model = m_Cache.GetBlockModel("block/" + modelName + ".json");
	if (model == nullptr)
	{
		model = LoadBlockModel(archive, "block/" + modelName + ".json");
		if (model == nullptr)
		{
			model = m_Cache.GetBlockModel("block/bedrock.json");
			if (model == nullptr)
			{
				std::cout << "AssetLoader::LoadBlockVariant: Could not find block model '" << modelName << "'." << std::endl;
				return nullptr;
			}
		}
	}

	std::unique_ptr<BlockVariant> variant = std::make_unique<BlockVariant>(model, Block);
	float x = usedVariant.value("x", 0.0f);
	float y = usedVariant.value("y", 0.0f);
	float z = usedVariant.value("z", 0.0f);
	//variant->SetRotation(glm::vec3(x, y, z));

	//printf("Has rotation = %d. Rotation [%f, %f, %f].\r\n", variant->HasRotation(), variant->GetRotations().x, variant->GetRotations().y, variant->GetRotations().z);

	return std::move(variant);
}

void AssetLoader::LoadBlockVariants(terra::ZipArchive& archive, const CMinecraftBlock * ForBlock)
{
	std::string blockStateFileFullName = "assets/minecraft/blockstates/" + ForBlock->GetName() + ".json";

	// file
	std::string blockStateFileContent = "";

	size_t size = 0;
	char* raw = archive.ReadFile(blockStateFileFullName.c_str(), &size);
	if (raw == nullptr)
	{
		std::ifstream t("1.12.2/custom/blockstates/" + ForBlock->GetName() + ".json");
		if (false == t.is_open())
		{
			std::cout << "AssetLoader: Blockstate file '" << blockStateFileFullName << "' don't found." << std::endl;
			return;
		}
		else
		{
			blockStateFileContent = std::string((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
		}
	}
	else
	{
		blockStateFileContent = std::string(raw, size);
	}

	// file content
	
	json blockStateRoot;
	try
	{
		blockStateRoot = json::parse(blockStateFileContent);
	}
	catch (json::parse_error& e)
	{
		std::cout << "AssetLoader: Blockstate file '" << blockStateFileFullName << "' parse error: '" << e.what() << "'." << std::endl;
		return;
	}

	// variants
	json variantsObject = blockStateRoot.value("variants", json());
	if (false == variantsObject.is_object())
	{
		std::cout << "AssetLoader: Blockstate file '" << blockStateFileFullName << "' don't contains variant object." << std::endl;
		
		auto blockVariant = LoadDefaultVariantBlock(archive, ForBlock);
		if (blockVariant != nullptr)
			m_Cache.AddVariantModel(std::move(blockVariant));
	}

	std::map<std::string, json> variantsMap;
	for (const auto& kv : variantsObject.items())
		variantsMap[kv.key()] = kv.value();

	const auto& normalVariant = variantsMap.find("normal");
	if (normalVariant != variantsMap.end())
	{
		auto blockVariant = LoadBlockVariant(archive, ForBlock, normalVariant->second);
		if (blockVariant != nullptr)
			m_Cache.AddVariantModel(std::move(blockVariant));
		else
			std::cout << "ADD VARIANT ERROR! '" << ForBlock->GetName() << "'" << std::endl;
	}
	else
	{
		for (const auto& variantMapIt : variantsMap)
		{
			const auto& variantArray = VariantNameToVariables(variantMapIt.first);
			if (false == ForBlock->IsVariablesMatch(variantArray))
				continue;

			auto blockVariant = LoadBlockVariant(archive, ForBlock, variantMapIt.second);
			if (blockVariant != nullptr)
				m_Cache.AddVariantModel(std::move(blockVariant));
			else
				std::cout << "Add variant error! '" << ForBlock->GetName() << "'" << std::endl;
		}
	}
}

void AssetLoader::LoadBlockVariants(terra::ZipArchive& archive)
{
	for (const auto& blockIt : BlockRegistry::GetInstance()->GetAllBlocks())
	{
		LoadBlockVariants(archive, blockIt.second);

		for (const auto& metaBlockIt : blockIt.second->GetBlockmetas())
		{
			LoadBlockVariants(archive, metaBlockIt.second);
		}
	}
}

} // ns terra
