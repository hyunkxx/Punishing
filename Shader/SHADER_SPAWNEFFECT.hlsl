#include "SHADER_DEFINES.hpp"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D g_DiffuseTexture;
texture2D g_MaskTexture;

float g_fTimeAcc = 0.f;

vector g_vMatAmbient = vector(0.4f, 0.4f, 0.4f, 1.f);
vector g_vMatSpecular = vector(1.f, 1.f, 1.f, 1.f);

vector g_vCamPosition;

vector g_vLightDir = vector(1.f, -1.f, 1.f, 0.f);
vector g_vLightPos = vector(5.f, 3.f, 5.f, 1.f);
float g_fLength = 10.f;
float g_fPower = 50.f;

/* 조명의 색상 */
vector g_vLightDiffuse = vector(1.f, 1.f, 1.f, 1.f);
vector g_vLightAmbient = vector(1.f, 1.f, 1.f, 1.f);
vector g_vLightSpecular = vector(1.f, 1.f, 1.f, 1.f);

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

struct PS_BLOOMOUT
{
	float4 vColor : SV_TARGET0;
	float4 vBloomColor : SV_TARGET1;
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

PS_BLOOMOUT PS_MAIN(PS_IN In)
{
	PS_BLOOMOUT Out = (PS_BLOOMOUT)0;

	float2 uv = In.vTexUV;
	uv.y += g_fTimeAcc * 2.f;
	uv.x -= g_fTimeAcc;

	Out.vColor = g_DiffuseTexture.Sample(LinearSampler, uv);

	if (Out.vColor.a <= 0.1f)
		discard;

	Out.vColor.a = g_DiffuseTexture.Sample(LinearSampler, uv).a * (1.2f - g_fTimeAcc);

	if (Out.vColor.a > 0.f)
	{
		// float4(1.f, 0.7f, 0.4f)
		Out.vColor = float4(1.f, 0.7f, 0.4f, Out.vColor.a);
		Out.vBloomColor = Out.vColor;
	}

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
		PixelShader = compile ps_5_0 PS_MAIN();
	}

}




