#include "CommonInclude.hlsl"

cbuffer Material : register(b2)
{
    float4 DiffuseColor;
	bool   HasDiffuseTexture;
};

struct VSIn
{
	float3 position : POSITION;
	float2 texCoord : TEXCOORD0;
	uint textureIndex : TEXCOORD1;
	float3 tint       : TEXCOORD2;
	uint ambientOcclusion : TEXCOORD3;
};


struct VSOut
{
	float4 position   : SV_POSITION;
	float2 texCoord   : TEXCOORD0;
	uint textureIndex : TEXCOORD1;
	float3 tint       : TEXCOORD2;
	uint ambientOcclusion : TEXCOORD3;
};

Texture2DArray DiffuseTexture : register(t0);

VSOut VS_main(VSIn IN)
{
	const float4x4 mvp = mul(PF.Projection, PF.View/*mul(PF.View, PO.Model)*/);

	VSOut OUT;
	OUT.position = mul(mvp, float4(IN.position, 1.0f));
	OUT.texCoord = float2(IN.texCoord.x, IN.texCoord.y);
	OUT.textureIndex = IN.textureIndex;
	OUT.tint = IN.tint * (0.55 + float(IN.ambientOcclusion) * 0.15);
	return OUT;
}

float4 PS_main(VSOut IN) : SV_TARGET
{
	//float4 resultColor = DiffuseColor;
	//if (HasDiffuseTexture)
	//	resultColor *= DiffuseTexture.Sample(LinearClampSampler, float3(IN.texCoord, IN.textureIndex));
	
	float4 color = DiffuseTexture.Sample(LinearClampSampler, float3(IN.texCoord, IN.textureIndex));
	if (color.a < 0.6f)
		discard;
	
    return color * float4(IN.tint, 1.0f);
}