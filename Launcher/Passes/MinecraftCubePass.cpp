#include "stdafx.h"

// General
#include "MinecraftCubePass.h"

// Additional
#include "MinecraftMaterial.h"
#include "../SceneMinecraft.h"

CMinecraftCubePass::CMinecraftCubePass(IRenderDevice& RenderDevice, IScene& Scene)
	: RenderPassPipelined(RenderDevice)
	, m_MinecraftScene(Scene)
{
	m_PerObjectConstantBuffer = GetRenderDevice().GetObjectsFactory().CreateConstantBuffer(PerObject());
}

CMinecraftCubePass::~CMinecraftCubePass()
{}




//
// IRenderPass
//
void CMinecraftCubePass::Render(RenderEventArgs & e)
{
	//glm::mat4 matrix(1.0f);
	//matrix = glm::translate(matrix, e.Camera->GetPosition());
	//matrix = glm::scale(matrix, glm::vec3(1.0f));

	//m_PerObjectConstantBuffer->Set(PerObject(matrix));
	//m_PerObjectShaderParameter->Bind();

	CSceneMinecraft& sceneMinecraft = dynamic_cast<CSceneMinecraft&>(m_MinecraftScene);
	auto meshGen = sceneMinecraft.GetMeshGen();

	g_AssetCache->GetTextures().GetTexture()->Bind(0, GetPipeline().GetPixelShaderPtr(), IShaderParameter::EType::Texture);

	for (auto i = meshGen->begin(); i != meshGen->end(); i++)
	{
		glm::ivec3 chunk_base = i->first;

		glm::dvec3 min = glm::dvec3(chunk_base);
		glm::dvec3 max = glm::dvec3(chunk_base) + glm::dvec3(16, 16, 16);
		BoundingBox chunk_bounds(min, max);

		if (e.CameraForCulling->GetFrustum().cullBox(BoundingBox(chunk_bounds.getMin(), chunk_bounds.getMax())))
			continue;

		i->second->GetGeometry().Render(GetPipeline().GetVertexShaderPtr());
	}
}



//
// IRenderPassPipelined
//
std::shared_ptr<IRenderPassPipelined> CMinecraftCubePass::ConfigurePipeline(std::shared_ptr<IRenderTarget> RenderTarget)
{
	__super::ConfigurePipeline(RenderTarget);

	{
		std::shared_ptr<IShader> vertexShader = GetRenderDevice().GetObjectsFactory().LoadShader(EShaderType::VertexShader, "MinecraftCube.hlsl", "VS_main");
		std::vector<SCustomInputElement> customElements;
		customElements.push_back({ 0,  0, ECustomVertexElementType::FLOAT3, ECustomVertexElementUsage::POSITION,     0 });
		customElements.push_back({ 0, 12, ECustomVertexElementType::FLOAT2, ECustomVertexElementUsage::TEXCOORD,     0 });
		customElements.push_back({ 0, 20, ECustomVertexElementType::UINT1,  ECustomVertexElementUsage::TEXCOORD,     1 });
		customElements.push_back({ 0, 24, ECustomVertexElementType::FLOAT3,  ECustomVertexElementUsage::TEXCOORD,     2 });
		customElements.push_back({ 0, 36, ECustomVertexElementType::UINT1,  ECustomVertexElementUsage::TEXCOORD,     3 });
		vertexShader->LoadInputLayoutFromCustomElements(customElements);
		GetPipeline().SetShader(vertexShader);

		// Per object
		//m_PerObjectShaderParameter = vertexShader->GetShaderParameterByName("PerObject");
		//_ASSERT(m_PerObjectShaderParameter);
		//m_PerObjectShaderParameter->SetConstantBuffer(m_PerObjectConstantBuffer);
	}

	{
		std::shared_ptr<IShader> pixelShader = GetRenderDevice().GetObjectsFactory().LoadShader(EShaderType::PixelShader, "MinecraftCube.hlsl", "PS_main");
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
