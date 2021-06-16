#pragma once

// FORWARD BEGIN
class AssetCache;
// FORWARD END

using TextureHandle = uint32_t;

class CMinecraftTexturesLoader
{
public:
	CMinecraftTexturesLoader(const IBaseManager& BaseManager, AssetCache& AssetCache);
	virtual ~CMinecraftTexturesLoader();

	bool GetTexture(const std::string& filename, TextureHandle* handle);
	bool IsTransparent(TextureHandle handle) const;

	void Generate();
	std::shared_ptr<ITexture> GetTexture() const;

private:
	TextureHandle Append(const std::string& filename, const std::string& texture);
	bool LoadTexture(const std::string& path, TextureHandle* handle);

private:
	const IBaseManager& m_BaseManager;
	AssetCache& m_AssetCache;

	std::shared_ptr<ITexture> m_TextureId;

	std::unordered_map<std::string, TextureHandle> m_Textures;
	std::map<TextureHandle, std::vector<unsigned char>> m_TextureData;
	std::map<TextureHandle, bool> m_TextureTransperency;
};
