#include "SHADER_DEFINES.hpp"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_BoneMatrix[256];

vector g_vCamPosition;
vector g_vLightDir;

vector g_vLightDiffuse;
vector g_vLightAmbient;
vector g_vLightSpecular;

vector g_vMtrlAmbient = vector(0.5f, 0.5f, 0.5f, 1.f);
vector g_vMtrlSpecular = vector(1.f, 1.f, 1.f, 1.f);

texture2D g_DiffuseTexture;

//µðÁ¹ºê±â´É
texture2D g_DissolveTexture;
float g_fDissolveAmount = 1.f;
float3 g_vGlowColor = { 1.f, 0.7f, 0.4f };
float3 g_vGlowColorRed = { 1.f, 0.f, 0.f };
float g_fGlowRange = 0.005f;
float g_fGlowFalloff = 0.1f;

struct VS_IN
{
	float3 vPosition : POSITION;
	float3 vNormal : NORMAL;
	float2 vTexUV : TEXCOORD0;
	float3 vTangent : TANGENT;

	uint4 vBlendIndices : BLENDINDEX;
	float4 vBlendWeights : BLENDWEIGHT;
};

struct VS_OUT
{
	float4 vPosition : SV_POSITION;
	float4 vNormal : NORMAL;
	float2 vTexUV : TEXCOORD0;
	float4 vWorldPos : TEXCOORD1;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT Out = (VS_OUT)0;

	matrix matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	float fWeightW = 1.f - (In.vBlendWeights.x + In.vBlendWeights.y + In.vBlendWeights.z);

	matrix AnimMatrix =
		g_BoneMatrix[In.vBlendIndices.x] * In.vBlendWeights.x +
		g_BoneMatrix[In.vBlendIndices.y] * In.vBlendWeights.y +
		g_BoneMatrix[In.vBlendIndices.z] * In.vBlendWeights.z +
		g_BoneMatrix[In.vBlendIndices.w] * fWeightW;

	vector vPosition = mul(float4(In.vPosition, 1.f), AnimMatrix);
	vector vNormal = mul(float4(In.vNormal, 0.f), AnimMatrix);

	Out.vPosition = mul(vPosition, matWVP);
	Out.vNormal = normalize(mul(vNormal, g_WorldMatrix));
	Out.vTexUV = In.vTexUV;
	Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);

	return Out;
}

struct PS_IN
{
	float4 vPosition : SV_POSITION;
	float4 vNormal : NORMAL;
	float2 vTexUV : TEXCOORD0;
	float4 vWorldPos : TEXCOORD1;
};

struct PS_OUT
{
	float4 vColor : SV_TARGET0;
	float4 vColor1 : SV_TARGET1;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

	if (vMtrlDiffuse.a <= 0.1f)
		discard;

	float fShade = saturate(dot(normalize(g_vLightDir) * -1.f, In.vNormal));

	if (fShade > 0.9f)
		fShade = 1.f;
	else if (fShade > 0.7f)
		fShade = 0.7f;
	else if (fShade > 0.2f)
		fShade = 0.2f;
	else if (fShade > 0.0f)
		fShade = 0.0f;

	vector vReflect = reflect(normalize(g_vLightDir), In.vNormal);
	vector vLook = In.vWorldPos - g_vCamPosition;

	float fSpecular = pow(saturate(dot(normalize(vReflect) * -1.f, normalize(vLook))), 30.f);

	Out.vColor = (g_vLightDiffuse * vMtrlDiffuse) * (fShade + (g_vLightAmbient * g_vMtrlAmbient));
	Out.vColor1 = (g_vLightDiffuse * vMtrlDiffuse) * (fShade + (g_vLightAmbient * g_vMtrlAmbient));

	return Out;
}

PS_OUT PS_ALPHA(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

	if (vMtrlDiffuse.a <= 0.1f)
		discard;

	float fShade = saturate(dot(normalize(g_vLightDir) * -1.f, In.vNormal));

	if (fShade > 0.9f)
		fShade = 1.f;
	else if (fShade > 0.7f)
		fShade = 0.7f;
	else if (fShade > 0.2f)
		fShade = 0.2f;
	else if (fShade > 0.0f)
		fShade = 0.0f;

	vector vReflect = reflect(normalize(g_vLightDir), In.vNormal);
	vector vLook = In.vWorldPos - g_vCamPosition;

	float fSpecular = pow(saturate(dot(normalize(vReflect) * -1.f, normalize(vLook))), 30.f);

	Out.vColor = (g_vLightDiffuse * vMtrlDiffuse) * (fShade + (g_vLightAmbient * g_vMtrlAmbient));
	Out.vColor.a = 0.4f;
	
	return Out;
}

PS_OUT PS_RIMALLY(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	vector vCamDir = normalize(In.vWorldPos - g_vCamPosition);
	float rim = 0;
	rim = 1 - saturate(dot(In.vNormal, -vCamDir));

	rim = pow(rim, 30.0f);

	float4 rimColor = float4(0.f, 0.f, 0.f, 1.f);
	rimColor = rim * rimColor;

	Out.vColor = rimColor;
	return Out;
}

PS_OUT PS_RIMENEMY(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	vector vCamDir = normalize(In.vWorldPos - g_vCamPosition);
	float rim = 0;
	rim = 1 - saturate(dot(In.vNormal, -vCamDir));

	rim = pow(rim, 30.0f);

	float4 rimColor = float4(1.f, 0.f, 0.f, 0.5f);
	rimColor = rim * rimColor;

	Out.vColor = rimColor;
	return Out;
}

PS_OUT PS_DESSOLVE(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	float dissolve = g_DissolveTexture.Sample(LinearSampler, In.vTexUV).r;
	dissolve = dissolve * 0.999f;
	float isVisible = dissolve - (g_fDissolveAmount * 0.3f);
	clip(isVisible);

	float isGlowing = smoothstep(g_fGlowRange + g_fGlowFalloff, g_fGlowRange, isVisible);
	float3 vGlowColor = isGlowing * g_vGlowColor;

	vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

	if (vMtrlDiffuse.a <= 0.1f)
		discard;

	float fShade = saturate(dot(normalize(g_vLightDir) * -1.f, In.vNormal));

	if (fShade > 0.9f)
		fShade = 1.f;
	else if (fShade > 0.7f)
		fShade = 0.7f;
	else if (fShade > 0.2f)
		fShade = 0.2f;
	else if (fShade > 0.0f)
		fShade = 0.0f;

	vector vReflect = reflect(normalize(g_vLightDir), In.vNormal);
	vector vLook = In.vWorldPos - g_vCamPosition;

	float fSpecular = pow(saturate(dot(normalize(vReflect) * -1.f, normalize(vLook))), 30.f);

	Out.vColor = (g_vLightDiffuse * vMtrlDiffuse) * (fShade + (g_vLightAmbient * g_vMtrlAmbient));
	Out.vColor.xyz += vGlowColor;

	return Out;
}

PS_OUT PS_DESSOLVE_COLOR_RED(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	float dissolve = g_DissolveTexture.Sample(LinearSampler, In.vTexUV).r;
	dissolve = dissolve * 0.999f;
	float isVisible = dissolve - (g_fDissolveAmount * 0.5f);
	clip(isVisible);

	float isGlowing = smoothstep(g_fGlowRange + g_fGlowFalloff, g_fGlowRange, isVisible);
	float3 vGlowColor = isGlowing * g_vGlowColorRed;

	vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

	if (vMtrlDiffuse.a <= 0.1f)
		discard;

	float fShade = saturate(dot(normalize(g_vLightDir) * -1.f, In.vNormal));

	if (fShade > 0.9f)
		fShade = 1.f;
	else if (fShade > 0.7f)
		fShade = 0.7f;
	else if (fShade > 0.2f)
		fShade = 0.2f;
	else if (fShade > 0.0f)
		fShade = 0.0f;

	vector vReflect = reflect(normalize(g_vLightDir), In.vNormal);
	vector vLook = In.vWorldPos - g_vCamPosition;

	float fSpecular = pow(saturate(dot(normalize(vReflect) * -1.f, normalize(vLook))), 30.f);

	Out.vColor = (g_vLightDiffuse * vMtrlDiffuse) * (fShade + (g_vLightAmbient * g_vMtrlAmbient));
	Out.vColor.xyz += vGlowColor;

	return Out;
}

technique11 DefaultTechnique
{
	pass Model
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass ModelAlpha
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_ALPHA();
	}

	pass RimAlly
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_RIMALLY();
	}

	pass RimEnemy
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_RIMENEMY();
	}

	pass Dessolve
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_DESSOLVE();
	}

	pass DessolveRed
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_DESSOLVE_COLOR_RED();
	}
}
