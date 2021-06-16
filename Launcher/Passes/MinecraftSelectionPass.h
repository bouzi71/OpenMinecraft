#pragma once

class CMinecraftSelectionPass
	: public RenderPassPipelined
{
public:
	CMinecraftSelectionPass(IRenderDevice& RenderDevice, IScene& Scene);
	virtual ~CMinecraftSelectionPass();

	// IRenderPass
	void Render(RenderEventArgs& e) override;

	// IRenderPassPipelined
	std::shared_ptr<IRenderPassPipelined> ConfigurePipeline(std::shared_ptr<IRenderTarget> RenderTarget) override final;
	
private:
	IScene& m_MinecraftScene;

	std::shared_ptr<IConstantBuffer> m_PerObjectConstantBuffer;
	IShaderParameter* m_PerObjectShaderParameter;

private:
	std::shared_ptr<IGeometry> m_BBoxGeometry;
	std::shared_ptr<IMaterial> m_Material;
};