#pragma once

#include "../assets/AssetCache.h"

extern std::unique_ptr<terra::AssetCache> g_AssetCache;

class CMinecraftCubePass
	: public RenderPassPipelined
{
public:
	CMinecraftCubePass(IRenderDevice& RenderDevice, IScene& Scene);
	virtual ~CMinecraftCubePass();

	// IRenderPass
	void Render(RenderEventArgs& e) override;

	// IRenderPassPipelined
	std::shared_ptr<IRenderPassPipelined> ConfigurePipeline(std::shared_ptr<IRenderTarget> RenderTarget) override final;
	
private:
	IScene& m_MinecraftScene;

	std::shared_ptr<IConstantBuffer> m_PerObjectConstantBuffer;
	IShaderParameter* m_PerObjectShaderParameter;
};