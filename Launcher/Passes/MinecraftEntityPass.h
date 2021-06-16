#pragma once

class CMinecraftEntityPass
	: public RenderPassPipelined
{
public:
	CMinecraftEntityPass(IRenderDevice& RenderDevice, IScene& Scene);
	virtual ~CMinecraftEntityPass();

	// IRenderPass
	void Render(RenderEventArgs& e) override;

	// IRenderPassPipelined
	std::shared_ptr<IRenderPassPipelined> ConfigurePipeline(std::shared_ptr<IRenderTarget> RenderTarget) override final;
	
private:
	IScene& m_MinecraftScene;

	std::shared_ptr<IConstantBuffer> m_PerObjectConstantBuffer;
	IShaderParameter* m_PerObjectShaderParameter;

	std::shared_ptr<IGeometry> m_EntityGeometry;
};