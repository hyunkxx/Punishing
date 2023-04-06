#include "SHADER_DEFINES.hpp"

float distortion = 0.2;        // the bias of the barrel distortion
float strength = 0.01;         // how much edge blur is applied (to obscure the r, g, b separation)
float separation = 0.1;        // how much to separate the r, g and b

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_RTT;
float g_fTimeAcc;

struct VS_IN
{
	float3 vPosition : POSITION;
	float2 vTexUV : TEXCOORD0;
};
struct PS_IN
{
	float4 vPosition : SV_POSITION;
	float2 vTexUV : TEXCOORD0;
};
struct PS_OUT
{
	float4 vColor : SV_TARGET0;
};

PS_IN VS_MAIN(VS_IN In)
{
	PS_IN			Out = (PS_IN)0;

	matrix			matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;

	return Out;
}

PS_OUT PS_POSTEFFECT_RGBBLUR(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;
	float2 uv = In.vTexUV;
	uv.x -= 0.5f;
	uv.y -= 0.5f;

	//uv = uv / iResolution.xy - 0.5;

	float fDistortion = distortion * g_fTimeAcc;
	float fStrength = strength * g_fTimeAcc;
	float fSeparation = separation * g_fTimeAcc;

	vector A = vector(0, 0, 0, 1);

	float2 UV;

	for (float i = -10.0; i < 10.0; i++)
	{
		UV = (uv / sqrt(1.0 + (i * fStrength + (fDistortion + fSeparation)) * dot(uv, uv))).xy + 0.5f;
		vector color = g_RTT.Sample(LinearClampSampler, UV);

		A.r += color.r;
	}

	for (float i = -10.0; i < 10.0; i++)
	{
		UV = (uv / sqrt(1.0 + (i * fStrength + fDistortion) * dot(uv, uv))).xy + 0.5f;
		vector color = g_RTT.Sample(LinearClampSampler, UV);
		A.g += color.g;
	}

	for (float i = -10.0; i < 10.0; i++)
	{
		UV = (uv / sqrt(1.0 + (i * fStrength + (fDistortion - fSeparation)) * dot(uv, uv))).xy + 0.5f;
		vector color = g_RTT.Sample(LinearClampSampler, UV);
		A.b += color.b;
	}

	A /= 10.0 * 2.0;

	Out.vColor = A;

	return Out;
}

technique11 DefaultTechnique
{
	pass Pass0
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Not_ZTest_ZWrite, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_POSTEFFECT_RGBBLUR();
	}
}