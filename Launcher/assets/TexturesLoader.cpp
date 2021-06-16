#include "stdafx.h"

// General
#include "TexturesLoader.h"

// Additional
#include "AssetCache.h"

#include "stb_image.h"


#if 0

struct Mipmap
{
	unsigned char* data;
	std::size_t dimension;

	Mipmap(unsigned char* data, std::size_t dimension) : data(data), dimension(dimension) {}

	int Sample(std::size_t x, std::size_t y, std::size_t color_offset)
	{
		return data[(y * dimension + x) * 4 + color_offset];
	}
};

float GetColorGamma(int a, int b, int c, int d)
{
	float an = a / 255.0f;
	float bn = b / 255.0f;
	float cn = c / 255.0f;
	float dn = d / 255.0f;

	return (std::pow(an, 2.2f) + std::pow(bn, 2.2f) + std::pow(cn, 2.2f) + std::pow(dn, 2.2f)) / 4.0f;
}

// Blend four samples into a final result after doing gamma conversions
int GammaBlend(int a, int b, int c, int d)
{
	float result = std::pow(GetColorGamma(a, b, c, d), 1.0f / 2.2f);

	return static_cast<int>(255.0f * result);
}

// Performs basic pixel averaging filter for generating mipmap.
void BoxFilterMipmap(std::vector<unsigned char>& previous, std::vector<unsigned char>& data, std::size_t dim)
{
	std::size_t size_per_tex = dim * dim * 4;
	std::size_t count = data.size() / size_per_tex;
	std::size_t prev_dim = dim * 2;

	unsigned int* pixel = (unsigned int*)data.data();
	for (std::size_t i = 0; i < count; ++i)
	{
		unsigned char* prev_tex = previous.data() + i * (prev_dim * prev_dim * 4);

		Mipmap source(prev_tex, prev_dim);

		for (std::size_t y = 0; y < dim; ++y)
		{
			for (std::size_t x = 0; x < dim; ++x)
			{
				int red, green, blue, alpha;

				const std::size_t red_index = 0;
				const std::size_t green_index = 1;
				const std::size_t blue_index = 2;
				const std::size_t alpha_index = 3;

				red = GammaBlend(source.Sample(x * 2, y * 2, red_index), source.Sample(x * 2 + 1, y * 2, red_index), source.Sample(x * 2, y * 2 + 1, red_index), source.Sample(x * 2 + 1, y * 2 + 1, red_index));

				green = GammaBlend(source.Sample(x * 2, y * 2, green_index), source.Sample(x * 2 + 1, y * 2, green_index), source.Sample(x * 2, y * 2 + 1, green_index), source.Sample(x * 2 + 1, y * 2 + 1, green_index));

				blue = GammaBlend(source.Sample(x * 2, y * 2, blue_index), source.Sample(x * 2 + 1, y * 2, blue_index), source.Sample(x * 2, y * 2 + 1, blue_index), source.Sample(x * 2 + 1, y * 2 + 1, blue_index));

				alpha = GammaBlend(source.Sample(x * 2, y * 2, alpha_index), source.Sample(x * 2 + 1, y * 2, alpha_index), source.Sample(x * 2, y * 2 + 1, alpha_index), source.Sample(x * 2 + 1, y * 2 + 1, alpha_index));

				// AA BB GG RR
				*pixel = ((alpha & 0xFF) << 24) | ((blue & 0xFF) << 16) | ((green & 0xFF) << 8) | (red & 0xFF);
				++pixel;
			}
		}
	}
}

#endif


CMinecraftTexturesLoader::CMinecraftTexturesLoader(const IBaseManager& BaseManager, AssetCache& AssetCache)
	: m_BaseManager(BaseManager)
	, m_AssetCache(AssetCache)
{
}

CMinecraftTexturesLoader::~CMinecraftTexturesLoader()
{}

bool CMinecraftTexturesLoader::GetTexture(const std::string& filename, TextureHandle* handle)
{
	if (filename.empty())
		return false;

	const auto& iter = m_Textures.find(filename);
	if (iter == m_Textures.end())
		return LoadTexture(filename, handle);

	*handle = iter->second;
	return true;
}

bool CMinecraftTexturesLoader::IsTransparent(TextureHandle handle) const
{
	const auto& textureTransperencyIt = m_TextureTransperency.find(handle);
	if (textureTransperencyIt == m_TextureTransperency.end())
		return false;

	return textureTransperencyIt->second;
}

void CMinecraftTexturesLoader::Generate()
{
	m_TextureId = m_BaseManager.GetApplication().GetRenderDevice().GetObjectsFactory().CreateEmptyTexture();

	std::vector<std::vector<uint8>> bytesArrays;
	for (const auto& texture : m_TextureData)
		bytesArrays.push_back(texture.second);
	m_TextureId->LoadTexture2DArrayFromBytes(bytesArrays, 16, 16, 4);

	/*int size = static_cast<int>(m_Textures.size());
	int dim = 16;
	for (int i = 1; i < levels; ++i)
	{
		dim /= 2;

		if (dim < 1) 
			dim = 1;

		std::size_t data_size = dim * dim * 4 * size;
		std::vector<unsigned char> previous(mipmap_data);
		mipmap_data = std::vector<unsigned char>(data_size);

		BoxFilterMipmap(previous, mipmap_data, dim);

		glTexSubImage3D(GL_TEXTURE_2D_ARRAY, i, 0, 0, 0, dim, dim, size, GL_RGBA, GL_UNSIGNED_BYTE, mipmap_data.data());
	}*/

	m_TextureData.clear();
}

std::shared_ptr<ITexture> CMinecraftTexturesLoader::GetTexture() const
{
	return m_TextureId;
}



//
// Private
//
TextureHandle CMinecraftTexturesLoader::Append(const std::string& filename, const std::string& texture)
{
	const auto& texturesIt = m_Textures.find(filename);
	if (texturesIt != m_Textures.end())
		return texturesIt->second;

	TextureHandle handle = static_cast<TextureHandle>(m_Textures.size());

	m_Textures[filename] = handle;
	m_TextureData[handle].assign((uint8*)texture.c_str(), (uint8*)(texture.c_str() + texture.size()));

	for (size_t i = 3; i < texture.size(); i += 4)
	{
		if (texture[i] == 0)
		{
			m_TextureTransperency[handle] = true;
			break;
		}
	}

	return handle;
}

bool CMinecraftTexturesLoader::LoadTexture(const std::string& path, TextureHandle * handle)
{
	std::string textureFileFullName = "assets/minecraft/textures/" + path + ".png";

	auto blockStateFile = m_BaseManager.GetManager<IFilesManager>()->Open(textureFileFullName);
	if (blockStateFile == nullptr)
	{
		Log::Error("CMinecraftTexturesLoader::LoadTexture: Unable to open file '%s'.", textureFileFullName.c_str());
		return false;
	}

	int width, height, channels;
	unsigned char* image = stbi_load_from_memory(blockStateFile->getData(), blockStateFile->getSize(), &width, &height, &channels, STBI_rgb_alpha);
	if (image == nullptr)
	{
		Log::Error("CMinecraftTexturesLoader::LoadTexture: Image for file '%s' is nullptr.", textureFileFullName.c_str());
		return false;
	}

	if (width == 16)
	{
		std::size_t size = 16 * 16 * 4;
		*handle = Append(path, std::string((char*)image, size));
	}

	stbi_image_free(image);

	return true;
}

