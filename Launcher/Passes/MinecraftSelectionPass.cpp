#include "stdafx.h"

// General
#include "MinecraftSelectionPass.h"

// Additional
#include "MinecraftMaterial.h"
#include "../SceneMinecraft.h"

CMinecraftSelectionPass::CMinecraftSelectionPass(IRenderDevice& RenderDevice, IScene& Scene)
	: RenderPassPipelined(RenderDevice)
	, m_MinecraftScene(Scene)
{
	m_PerObjectConstantBuffer = GetRenderDevice().GetObjectsFactory().CreateConstantBuffer(PerObject());
}

CMinecraftSelectionPass::~CMinecraftSelectionPass()
{}




//
// IRenderPass
//
void CMinecraftSelectionPass::Render(RenderEventArgs & e)
{
	CSceneMinecraft& sceneMinecraft = dynamic_cast<CSceneMinecraft&>(m_MinecraftScene);

	const CMinecraftBlock * selectedBlock = sceneMinecraft.GetSelectedBlock();
	if (selectedBlock == nullptr)
		return;

	glm::vec3 position = sceneMinecraft.GetSelectedBlockPosition();

	glm::mat4 model(1.0);
	model = glm::translate(model, position - 0.1f);
	//model = glm::rotate(model, entity->GetYaw(), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(1.2));

	m_PerObjectConstantBuffer->Set(PerObject(model));
	m_PerObjectShaderParameter->Bind();

	m_Material->Bind(GetRenderEventArgs().PipelineState->GetPixelShaderPtr());
	m_BBoxGeometry->Render(GetRenderEventArgs().PipelineState->GetVertexShaderPtr());
	m_Material->Unbind(GetRenderEventArgs().PipelineState->GetPixelShaderPtr());
}



//
// IRenderPassPipelined
//
std::shared_ptr<IRenderPassPipelined> CMinecraftSelectionPass::ConfigurePipeline(std::shared_ptr<IRenderTarget> RenderTarget)
{
	__super::ConfigurePipeline(RenderTarget);

	m_BBoxGeometry = GetRenderDevice().GetPrimitivesFactory().CreateBBox();
	m_Material = MakeShared(MaterialDebug, GetRenderDevice());

	{
		std::shared_ptr<IShader> vertexShader = GetRenderDevice().GetObjectsFactory().LoadShader(EShaderType::VertexShader, "3D/Debug.hlsl", "VS_main");
		vertexShader->LoadInputLayoutFromReflector();
		GetPipeline().SetShader(vertexShader);

		// Per object
		m_PerObjectShaderParameter = vertexShader->GetShaderParameterByName("PerObject");
		_ASSERT(m_PerObjectShaderParameter);
		m_PerObjectShaderParameter->SetConstantBuffer(m_PerObjectConstantBuffer);
	}

	{
		std::shared_ptr<IShader> pixelShader = GetRenderDevice().GetObjectsFactory().LoadShader(EShaderType::PixelShader, "3D/Debug.hlsl", "PS_main");
		GetPipeline().SetShader(pixelShader);
	}

	// PIPELINES
	GetPipeline().GetBlendState()->SetBlendMode(disableBlending);
	GetPipeline().GetDepthStencilState()->SetDepthMode(enableDepthWrites);
	GetPipeline().GetRasterizerState()->SetCullMode(IRasterizerState::CullMode::None);
	GetPipeline().GetRasterizerState()->SetFillMode(IRasterizerState::FillMode::Wireframe, IRasterizerState::FillMode::Wireframe);
	
	return shared_from_this();
}
