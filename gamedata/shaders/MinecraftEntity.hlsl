#include "CommonInclude.hlsl"

struct VSIn
{
	float3 position : POSITION;
};

struct VSOut
{
	float4 position : SV_POSITION;
};

VSOut VS_main(VSIn IN)
{
	const float4x4 mvp = mul(PF.Projection, mul(PF.View, PO.Model));

	VSOut OUT;
	OUT.position = mul(mvp, float4(IN.position, 1.0f));
	return OUT;
}

float4 PS_main(VSOut IN) : SV_TARGET
{
    return float4(0.2f, 0.8f, 0.2f, 0.8f);
}