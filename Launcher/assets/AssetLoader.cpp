#include "stdafx.h"

#include "AssetLoader.h"
#include "AssetCache.h"

#include <common/Json.h>

#include "stb_image.h"

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



AssetLoader::AssetLoader(const IBaseManager& BaseManager, AssetCache& cache)
	: m_BaseManager(BaseManager)
	, m_Cache(cache)
{}

void AssetLoader::Initialize()
{
	LoadBlockVariants();
	m_Cache.GetTextures().Generate();

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

bool AssetLoader::LoadTexture(const std::string& path, TextureHandle* handle)
{
	if (m_Cache.GetTextures().GetTexture(path, handle)) 
		return true;

	std::string textureFileFullName = "assets/minecraft/textures/" + path;

	auto blockStateFile = m_BaseManager.GetManager<IFilesManager>()->Open(textureFileFullName);
	if (blockStateFile == nullptr)
	{
		Log::Error("AssetLoader::LoadTexture: Unable to open file '%s'.", textureFileFullName.c_str());
		return false;
	}

	int width, height, channels;
	unsigned char* image = stbi_load_from_memory(blockStateFile->getData(), blockStateFile->getSize(), &width, &height, &channels, STBI_rgb_alpha);
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



std::unique_ptr<BlockModel> AssetLoader::LoadBlockModel(const std::string& path /* block/sdfsdfsdf.json */, TextureMap& texture_map, std::vector<IntermediateElement>& intermediates)
{
	std::string blockModelFileFullName = "assets/minecraft/models/" + path;

	auto blockStateFile = m_BaseManager.GetManager<IFilesManager>()->Open(blockModelFileFullName);
	if (blockStateFile == nullptr)
	{
		Log::Error("AssetLoader::LoadBlockModel: Unable to open file '%s'.", blockModelFileFullName.c_str());
		return false;
	}

	json root;
	try
	{
		root = json::parse(std::string((char*)blockStateFile->getDataEx(), blockStateFile->getSize()));
	}
	catch (json::parse_error& e)
	{
		Log::Error("AssetLoader::LoadBlockModel: File '%s' parse error '%s'.", blockModelFileFullName.c_str(), e.what());
		return nullptr;
	}

	auto parent_node = root.value("parent", json());
	if (parent_node.is_string())
	{
		std::string parent_path = parent_node.get<std::string>() + ".json";
		LoadBlockModel(parent_path, texture_map, intermediates);
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

BlockModel* AssetLoader::LoadBlockModel(const std::string& ModelName)
{
	TextureMap texture_map;
	std::vector<IntermediateElement> intermediates;

	auto model = LoadBlockModel(ModelName, texture_map, intermediates);

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
			if (LoadTexture(texture + ".png", &handle))
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



std::unique_ptr<BlockVariant> AssetLoader::LoadDefaultVariantBlock(const CMinecraftBlock * Block)
{
	BlockModel* model = m_Cache.GetBlockModel("block/bedrock.json");
	if (model == nullptr)
	{
		model = LoadBlockModel("block/bedrock.json");
		if (model == nullptr)
		{
			Log::Error("AssetLoader::LoadBlockVariant: Could not find block model 'bedrock'.");
			return nullptr;
		}
	}

	std::unique_ptr<BlockVariant> variant = std::make_unique<BlockVariant>(model, Block);
	return std::move(variant);
}

std::unique_ptr<BlockVariant> AssetLoader::LoadBlockVariant(const CMinecraftBlock * Block, json VariantJSONObject)
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
	BlockModel* model = m_Cache.GetBlockModel("block/" + modelName + ".json");
	if (model == nullptr)
	{
		model = LoadBlockModel("block/" + modelName + ".json");
		if (model == nullptr)
		{
			model = m_Cache.GetBlockModel("block/bedrock.json");
			if (model == nullptr)
			{
				Log::Error("AssetLoader::LoadBlockVariant: Could not find block model '%s'.", modelName.c_str());
				return nullptr;
			}
		}
	}

	std::unique_ptr<BlockVariant> variant = std::make_unique<BlockVariant>(model, Block);
	float x = usedVariant.value("x", 0.0f);
	float y = usedVariant.value("y", 0.0f);
	float z = usedVariant.value("z", 0.0f);
	variant->SetRotation(glm::vec3(x, y, z));

	//printf("Has rotation = %d. Rotation [%f, %f, %f].\r\n", variant->HasRotation(), variant->GetRotations().x, variant->GetRotations().y, variant->GetRotations().z);

	return std::move(variant);
}

void AssetLoader::LoadBlockVariants(const CMinecraftBlock * ForBlock)
{
	if (ForBlock->GetName().empty())
		return;

	std::string blockStateFileFullName = "assets/minecraft/blockstates/" + ForBlock->GetName() + ".json";

	auto blockStateFile = m_BaseManager.GetManager<IFilesManager>()->Open(blockStateFileFullName);
	if (blockStateFile == nullptr)
	{
		Log::Error("AssetLoader::LoadBlockVariants: Unable to open file '%s'.", blockStateFileFullName.c_str());
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
		Log::Error("AssetLoader::LoadBlockVariants: File '%s' parse error '%s'.", blockStateFileFullName.c_str(), e.what());
		return;
	}

	// variants
	json variantsObject = blockStateRoot.value("variants", json());
	if (false == variantsObject.is_object())
	{
		Log::Error("AssetLoader: Blockstate file '%s' don't contains 'variants'.", blockStateFileFullName.c_str());
		
		auto blockVariant = LoadDefaultVariantBlock(ForBlock);
		if (blockVariant != nullptr)
			m_Cache.AddVariantModel(std::move(blockVariant));
	}

	std::map<std::string, json> variantsMap;
	for (const auto& kv : variantsObject.items())
		variantsMap[kv.key()] = kv.value();

	const auto& normalVariant = variantsMap.find("normal");
	if (normalVariant != variantsMap.end())
	{
		auto blockVariant = LoadBlockVariant(ForBlock, normalVariant->second);
		if (blockVariant != nullptr)
		{
			m_Cache.AddVariantModel(std::move(blockVariant));
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
				m_Cache.AddVariantModel(std::move(blockVariant));
			}
			else
			{
				Log::Error("Add variant error 2. '%s'.", ForBlock->GetName().c_str());
			}
		}
	}
}

void AssetLoader::LoadBlockVariants()
{
	for (const auto& blockIt : BlockRegistry::GetInstance()->GetAllBlocks())
	{
		LoadBlockVariants(blockIt.second);

		for (const auto& metaBlockIt : blockIt.second->GetBlockmetas())
		{
			LoadBlockVariants(metaBlockIt.second);
		}
	}
}

} // ns terra
