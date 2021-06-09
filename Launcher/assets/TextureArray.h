#ifndef TERRACOTTA_ASSETS_TEXTUREARRAY_H_
#define TERRACOTTA_ASSETS_TEXTUREARRAY_H_

#include <assets/blocks/BlockElement.h>

#include <cstddef>
#include <cstdint>

namespace terra
{


class TextureArray
{
public:
	TextureArray();

	TextureHandle Append(const std::string& filename, const std::string& texture);

	bool GetTexture(const std::string& filename, TextureHandle* handle);
	bool IsTransparent(TextureHandle handle) const;

	void Generate();
	void Bind();

private:
	unsigned int m_TextureId;

	std::vector<unsigned char> m_TextureData;
	std::unordered_map<std::string, TextureHandle> m_Textures;

	bool m_Transparency[2048];
};
} // ns terra

#endif
