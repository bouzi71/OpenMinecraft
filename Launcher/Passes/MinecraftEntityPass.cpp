#include "stdafx.h"

// General
#include "MinecraftEntityPass.h"

// Additional
#include "MinecraftMaterial.h"
#include "../SceneMinecraft.h"

CMinecraftEntityPass::CMinecraftEntityPass(IRenderDevice& RenderDevice, IScene& Scene)
	: RenderPassPipelined(RenderDevice)
	, m_MinecraftScene(Scene)
{
	m_PerObjectConstantBuffer = GetRenderDevice().GetObjectsFactory().CreateConstantBuffer(PerObject());

	m_EntityGeometry = RenderDevice.GetPrimitivesFactory().CreateCube();

}

CMinecraftEntityPass::~CMinecraftEntityPass()
{}




//
// IRenderPass
//
void CMinecraftEntityPass::Render(RenderEventArgs & e)
{
	CSceneMinecraft& sceneMinecraft = dynamic_cast<CSceneMinecraft&>(m_MinecraftScene);

	auto entity_manager = sceneMinecraft.GetNetworkClient().GetEntityManager();
	for (auto f = entity_manager->begin(); f != entity_manager->end(); ++f)
	{
		auto&& entity = f->second;
		if (entity_manager->GetPlayerEntity() == entity)
			continue;

		glm::dvec3 position = entity->GetPosition() + glm::dvec3(0, 0.5, 0);

		glm::mat4 model(1.0);
		model = glm::translate(model, glm::vec3(position.x, position.y + 0.5f, position.z));
		model = glm::rotate(model, entity->GetYaw(), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1, 2, 1));

		m_PerObjectConstantBuffer->Set(PerObject(model));
		m_PerObjectShaderParameter->Bind();

		m_EntityGeometry->Render(GetPipeline().GetVertexShaderPtr());
	}
}



//
// IRenderPassPipelined
//
std::shared_ptr<IRenderPassPipelined> CMinecraftEntityPass::ConfigurePipeline(std::shared_ptr<IRenderTarget> RenderTarget)
{
	__super::ConfigurePipeline(RenderTarget);

	{
		std::shared_ptr<IShader> vertexShader = GetRenderDevice().GetObjectsFactory().LoadShader(EShaderType::VertexShader, "MinecraftEntity.hlsl", "VS_main");
		vertexShader->LoadInputLayoutFromReflector();
		//std::vector<SCustomInputElement> customElements;
		//customElements.push_back({ 0,  0, ECustomVertexElementType::FLOAT3, ECustomVertexElementUsage::POSITION,     0 });
		//customElements.push_back({ 0, 12, ECustomVertexElementType::FLOAT2, ECustomVertexElementUsage::TEXCOORD,     0 });
		//customElements.push_back({ 0, 20, ECustomVertexElementType::UINT1,  ECustomVertexElementUsage::TEXCOORD,     1 });
		//customElements.push_back({ 0, 24, ECustomVertexElementType::FLOAT3,  ECustomVertexElementUsage::TEXCOORD,     2 });
		//customElements.push_back({ 0, 36, ECustomVertexElementType::UINT1,  ECustomVertexElementUsage::TEXCOORD,     3 });
		//vertexShader->LoadInputLayoutFromCustomElements(customElements);
		GetPipeline().SetShader(vertexShader);

		// Per object
		m_PerObjectShaderParameter = vertexShader->GetShaderParameterByName("PerObject");
		_ASSERT(m_PerObjectShaderParameter);
		m_PerObjectShaderParameter->SetConstantBuffer(m_PerObjectConstantBuffer);
	}

	{
		std::shared_ptr<IShader> pixelShader = GetRenderDevice().GetObjectsFactory().LoadShader(EShaderType::PixelShader, "MinecraftEntity.hlsl", "PS_main");
		GetPipeline().SetShader(pixelShader);
	}

	// PIPELINES
	GetPipeline().GetBlendState()->SetBlendMode(disableBlending);
	GetPipeline().GetDepthStencilState()->SetDepthMode(enableDepthWrites);
	GetPipeline().GetRasterizerState()->SetCullMode(IRasterizerState::CullMode::Back);
	GetPipeline().GetRasterizerState()->SetFillMode(IRasterizerState::FillMode::Solid, IRasterizerState::FillMode::Solid);
	
	GetPipeline().GetSampler(0)->SetFilter(ISamplerState::MinFilter::MinLinear, ISamplerState::MagFilter::MagNearest, ISamplerState::MipFilter::MipNearest);
	GetPipeline().GetSampler(1)->SetFilter(ISamplerState::MinFilter::MinLinear, ISamplerState::MagFilter::MagNearest, ISamplerState::MipFilter::MipNearest);

	return shared_from_this();
}
