#ifndef TERRACOTTA_ASSETS_TEXTUREARRAY_H_
#define TERRACOTTA_ASSETS_TEXTUREARRAY_H_

#include <assets/blocks/BlockElement.h>


namespace terra
{

class TextureArray
{
public:
	TextureArray(const IBaseManager& BaseManager);

	TextureHandle Append(const std::string& filename, const std::string& texture);
	std::shared_ptr<ITexture> GetTexture();

	bool GetTexture(const std::string& filename, TextureHandle* handle);
	bool IsTransparent(TextureHandle handle) const;

	void Generate();

private:
	const IBaseManager& m_BaseManager;
	std::shared_ptr<ITexture> m_TextureId;

	std::map<TextureHandle, std::vector<unsigned char>> m_TextureData;
	std::unordered_map<std::string, TextureHandle> m_Textures;

	bool m_Transparency[2048];
};
} // ns terra

#endif
