#include "stdafx.h"

// General
#include "MinecraftMaterial.h"

CMinecraftMaterial::CMinecraftMaterial(IRenderDevice& RenderDevice)
	: MaterialProxieT(RenderDevice.GetObjectsFactory().CreateMaterial("CMinecraftMaterial"))
{}

CMinecraftMaterial::~CMinecraftMaterial()
{}



//
// CMinecraftMaterial
//
void CMinecraftMaterial::SetDiffuseColor(const glm::vec4& diffuse)
{
	MaterialData().DiffuseColor = diffuse;
}

glm::vec4 CMinecraftMaterial::GetDiffuseColor() const
{
	return MaterialDataReadOnly().DiffuseColor;
}

void CMinecraftMaterial::SetTexture(std::shared_ptr<ITexture> Texture)
{
	MaterialData().HasTextureDiffuse = (Texture != nullptr);

	if (MaterialDataReadOnly().HasTextureDiffuse)
		__super::SetTexture(0, Texture);
}

std::shared_ptr<ITexture> CMinecraftMaterial::GetTexture() const
{
	return __super::GetTexture(0);
}