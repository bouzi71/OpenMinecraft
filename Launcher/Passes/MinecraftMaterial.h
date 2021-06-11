#pragma once

namespace
{
	__declspec(align(16)) struct SMinecraftMaterialProperties
	{
		SMinecraftMaterialProperties()
			: DiffuseColor(1.0f, 1.0f, 1.0f, 1.0f)
			, HasTextureDiffuse(false)
		{}
		glm::vec4 DiffuseColor;
		//-------------------------- ( 16 bytes )
		uint32    HasTextureDiffuse;
		glm::vec3 _padding;
		//-------------------------- ( 16 bytes )
	};
}

class CMinecraftMaterial
	: public MaterialProxieT<SMinecraftMaterialProperties>
{
public:
	CMinecraftMaterial(IRenderDevice& RenderDevice);
	virtual ~CMinecraftMaterial();

	void SetDiffuseColor(const glm::vec4& diffuse);
	glm::vec4 GetDiffuseColor() const;
	void SetTexture(std::shared_ptr<ITexture> Texture);
	std::shared_ptr<ITexture> GetTexture() const;
};