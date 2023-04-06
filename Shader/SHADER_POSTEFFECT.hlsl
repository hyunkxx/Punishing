#include "SHADER_DEFINES.hpp"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_MainTexture;
texture2D g_BloomTexture;

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

VS_OUT VS_MAIN(VS_IN In)
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
 
PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = g_MainTexture.Sample(LinearSampler, In.vTexUV);

	return Out;
}

PS_OUT PS_COMBINE(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	//글로우 톤 매핑 파라미터인데 잘모르겠다.
	//float exposure = 1.f;
	//const float gamma = 2.2;

	vector hdrColor = g_MainTexture.Sample(LinearSampler, In.vTexUV);
	vector bloomColor = g_BloomTexture.Sample(LinearSampler, In.vTexUV);

	if (bloomColor.a > 0.f)
	{
		//hdrColor += bloomColor;
		//float3 result = float3(1.f, 1.f, 1.f) - exp(-hdrColor * exposure);
		//result = pow(result, float3(1.0 / gamma, 1.0 / gamma, 1.0 / gamma));

		//Out.vColor = float4(result, 1.f);
		Out.vColor = hdrColor + bloomColor;
	}
	else
	{
		Out.vColor = hdrColor;
	}

	return Out;
}

technique11 DefaultTechnique
{
	pass PostEffect
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Not_ZTest_ZWrite, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass Pass1
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Not_ZTest_ZWrite, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_COMBINE();
	}
}