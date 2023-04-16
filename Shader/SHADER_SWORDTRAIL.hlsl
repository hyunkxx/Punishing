#include "SHADER_DEFINES.hpp"

vector g_vCamPosition;
matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D g_DiffuseTexture;
texture2D g_MaskTexture;

float g_fTimeAcc = -1.f;
float g_fMaskValue = 0.f;

struct VS_IN
{
	float3 vPosition : POSITION;
	float3 vNormal : NORMAL;
	float2 vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	float4 vPosition : SV_POSITION;
	float3 vNormal : NORMAL;
	float2 vTexUV : TEXCOORD0;
	float4 vLook : TEXCOORD1;
};

struct PS_IN
{
	float4 vPosition : SV_POSITION;
	float3 vNormal : NORMAL;
	float2 vTexUV : TEXCOORD0;
	float4 vLook : TEXCOORD1;
};

struct PS_OUT
{
	float4 vMainColor : SV_TARGET0;
	float4 vBloomColor : SV_TARGET1;
};

struct PS_DISTORTION
{
	float4 vMainColor : SV_TARGET0;
	float4 vBloomColor : SV_TARGET1;
	float4 vDistortion : SV_TARGET2;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT Out = (VS_OUT)0;

	matrix matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	vector worldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
	vector lookDir = worldPos - g_vCamPosition;

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;
	Out.vNormal = In.vNormal;
	Out.vLook = lookDir;

	return Out;
}

PS_DISTORTION PS_DEFAULT(PS_IN In)
{
	PS_DISTORTION Out = (PS_DISTORTION)0;

	float2 FixUV = In.vTexUV;
	FixUV.y += g_fTimeAcc * 0.1f;
	FixUV.x += g_fTimeAcc;

	vector vDiffuse = g_DiffuseTexture.Sample(LinearClampSampler, FixUV);
	vector vMask = g_MaskTexture.Sample(LinearClampSampler, In.vTexUV) * vDiffuse;
	
	vDiffuse.g = vDiffuse.r;
	vDiffuse.r = 0.f;

	Out.vMainColor = vDiffuse;

	if (vMask.a < 0.1f)
		discard;

	if (Out.vMainColor.a < 0.1f)
		discard;
	
	if(Out.vMainColor.a < 0.125f)
		Out.vDistortion = float4(1.f, 1.f, 1.f, 1.f);

	return Out;
}

PS_OUT PS_PASS1(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	Out.vMainColor = float4(1.f, 1.f, 1.f, 1.5f);
	Out.vMainColor.a -= g_fTimeAcc * 5.f;
	
	Out.vBloomColor = float4(1.f, 0.f, 0.f, 1.f);
	Out.vBloomColor.a -= g_fTimeAcc;

	return Out;
}

technique11 DefaultTechnique
{
	pass Pass0
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_DEFAULT();
	}

	pass Pass1
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Not_ZTest_ZWrite, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_PASS1();
	}

}




