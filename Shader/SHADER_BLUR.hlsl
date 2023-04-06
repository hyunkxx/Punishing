#include "SHADER_DEFINES.hpp"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_RTT;

float fWeight[13] =
{
	0.0561, 0.1353, 0.278, 0.4868, 0.7271, 0.9231, 1,
	0.9231, 0.7261, 0.4868, 0.278, 0.1353, 0.0561
};

float fTotal = 6.2108;

struct VS_IN
{
	float3			vPosition : POSITION;
	float2			vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	float4			vPosition : SV_POSITION;
	float2			vTexUV : TEXCOORD0;
};

VS_OUT VS_BLUR(VS_IN In)
{
	VS_OUT			Out = (VS_OUT)0;

	matrix			matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;

	return Out;
}

struct PS_IN
{
	float4			vPosition : SV_POSITION;
	float2			vTexUV : TEXCOORD0;
};

struct PS_OUT
{
	float4			vColor : SV_TARGET0;
};

PS_OUT PS_XBLUR(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float2 t = In.vTexUV;
	float2 uv = 0;
	float tu = 1.f / (1280 / 2.f) ;

	for (int i = -6; i < 6; ++i)
	{
		uv = t + float2(tu * i, 0);
		Out.vColor += (fWeight[6 + i]) * g_RTT.Sample(LinearClampSampler, uv);
	}

	Out.vColor /= fTotal;

	return Out;
}

PS_OUT PS_YBLUR(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float2 t = In.vTexUV;
	float2 uv = 0;
	float tv = 1.f / (720 / 2.f);

	for (int i = -6; i < 6; ++i)
	{
		uv = t + float2(0, tv * i);
		Out.vColor += fWeight[6 + i] * g_RTT.Sample(LinearClampSampler, uv);
	}

	Out.vColor /= fTotal;

	return Out;
}

technique11 DefaultTechnique
{
	//X블러
	pass Pass0
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Not_ZTest_ZWrite, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_BLUR();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_XBLUR();
	}

	//Y블러
	pass Pass1
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Not_ZTest_ZWrite, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_BLUR();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_YBLUR();
	}
}