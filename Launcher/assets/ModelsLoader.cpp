#include "stdafx.h"

// General
#include "ModelsLoader.h"

// Addutional
#include "AssetCache.h"

#include <common/Json.h>


namespace
{
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
}



//
// CMinecraftModelsLoader
//
CMinecraftModelsLoader::CMinecraftModelsLoader(const IBaseManager & BaseManager, AssetCache& AssetCache)
	: m_BaseManager(BaseManager)
	, m_AssetCache(AssetCache)
{}

CMinecraftModelsLoader::~CMinecraftModelsLoader()
{}

const BlockModel* CMinecraftModelsLoader::GetBlockModel(const std::string& path)
{
	auto iter = m_BlockModels.find(path);
	if (iter == m_BlockModels.end())
		return LoadBlockModel(path);

	return iter->second.get();
}

void CMinecraftModelsLoader::AddBlockModel(const std::string& path, std::unique_ptr<BlockModel> model)
{
	if (m_BlockModels.find(path) != m_BlockModels.end())
	{
		Log::Warn("CMinecraftModelsLoader::AddBlockModel: Model with filename '%s' already registered.", path.c_str());
		return;
	}
	m_BlockModels[path] = std::move(model);
}



//
// Private
//
std::unique_ptr<BlockModel> CMinecraftModelsLoader::LoadBlockModel(const std::string& path /* block/sdfsdfsdf.json */, std::unordered_map<std::string, std::string>& texture_map, std::vector<IntermediateElement>& intermediates)
{
	std::string blockModelFileFullName = "assets/minecraft/models/" + path;

	auto blockStateFile = m_BaseManager.GetManager<IFilesManager>()->Open(blockModelFileFullName);
	if (blockStateFile == nullptr)
	{
		Log::Error("CMinecraftModelsLoader::LoadBlockModel: Unable to open file '%s'.", blockModelFileFullName.c_str());
		return nullptr;
	}

	json root;
	try
	{
		root = json::parse(std::string((char*)blockStateFile->getDataEx(), blockStateFile->getSize()));
	}
	catch (json::parse_error& e)
	{
		Log::Error("CMinecraftModelsLoader::LoadBlockModel: File '%s' parse error '%s'.", blockModelFileFullName.c_str(), e.what());
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
	for (const auto& element_node : elements_node)
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

BlockModel* CMinecraftModelsLoader::LoadBlockModel(const std::string& ModelName)
{
	std::unordered_map<std::string, std::string> texture_map;
	std::vector<IntermediateElement> intermediates;

	auto model = LoadBlockModel(ModelName, texture_map, intermediates);
	if (model == nullptr)
		return nullptr;

	for (const IntermediateElement& intermediate : intermediates)
	{
		BlockElement element(intermediate.from, intermediate.to);
		element.SetShouldShade(intermediate.shade);
		element.SetRotation(intermediate.rotation);

		for (const auto& intermediate_renderable : intermediate.faces)
		{
			std::string texture = intermediate_renderable.texture;
			while (false == texture.empty() && texture[0] == '#')
			{
				texture = texture_map[texture.substr(1)];
			}

			TextureHandle handle;
			if (m_AssetCache.GetTexturesLoader().GetTexture(texture, &handle))
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

	BlockModel* blockModelPtr = model.get();
	AddBlockModel(ModelName, std::move(model));
	return blockModelPtr;
}