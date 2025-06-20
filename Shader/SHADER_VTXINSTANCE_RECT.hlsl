#include "Shader_Defines.hpp"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_DiffuseTexture;
float3 g_vCamPosition;

float g_WidthCount;
float g_CurrentCount;

struct VS_IN
{
	float3 vPosition : POSITION;
	float2 vTexUV : TEXCOORD0;

	float4 vRight : TEXCOORD1;
	float4 vUp : TEXCOORD2;
	float4 vLook : TEXCOORD3;
	float4 vTranslation : TEXCOORD4;
};

struct VS_OUT
{
	float4 vPosition : SV_POSITION;
	float2 vTexUV : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT Out = (VS_OUT)0;

	matrix matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	float4x4 TransformMatrix = float4x4(In.vRight, In.vUp, In.vLook, In.vTranslation);

	vector vPosition = mul(float4(In.vPosition, 1.f), TransformMatrix);
	Out.vPosition = mul(vPosition, matWVP);
	Out.vTexUV = In.vTexUV;

	return Out;
}

VS_OUT VS_Billboard(VS_IN In)
{
	VS_OUT Out = (VS_OUT)0;

	matrix matWV, matWVP;

	matrix TransformMatrix = matrix(In.vRight, In.vUp, In.vLook, In.vTranslation);
	vector vPosition = mul(float4(In.vPosition, 1.f), TransformMatrix);

	vector vLook = normalize(float4(g_vCamPosition, 1.f) - vPosition) * length(In.vLook);
	float3 vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook.xyz)) * length(In.vRight);
	float3 vUp = normalize(cross(vLook.xyz, vRight)) * length(In.vUp);

	TransformMatrix[0] = float4(vRight, 0.f);
	TransformMatrix[1] = float4(vUp, 0.f);
	TransformMatrix[2] = vLook;

	vPosition = mul(float4(In.vPosition, 1.f), TransformMatrix);
	
	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);
	Out.vPosition = mul(vPosition, matWVP);
	Out.vTexUV = In.vTexUV;

	return Out;
}

struct PS_IN
{
	float4 vPosition : SV_POSITION;
	float2 vTexUV : TEXCOORD0;
};

struct PS_OUT
{
	float4 vColor : SV_TARGET0;
	float4 vBloom : SV_TARGET1;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT	Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(PointSampler, In.vTexUV);

	if (Out.vColor.a <= 0.9f)
		discard;

	return Out;
}

PS_OUT PS_Pass1_Alpha(PS_IN In)
{
	PS_OUT	Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(PointSampler, In.vTexUV);
	Out.vBloom = Out.vColor;

	return Out;
}

PS_OUT PS_Pass2_Alpha(PS_IN In)
{
	PS_OUT	Out = (PS_OUT)0;

	float2 uv = In.vTexUV;

	float fHeight = floor(g_CurrentCount / g_WidthCount);
	float fWidth = frac(g_CurrentCount / g_WidthCount);

	uv.x = In.vTexUV.x / g_WidthCount + fWidth;
	uv.y = In.vTexUV.y / g_WidthCount + (fHeight * (1 / g_WidthCount));

	Out.vColor = g_DiffuseTexture.Sample(PointSampler, uv);
	Out.vBloom = Out.vColor;

	return Out;
}

technique11 DefaultTechnique
{
	pass Particle
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

	pass Pass1
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_Pass1_Alpha();
	}

	pass Pass2
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_Billboard();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_Pass2_Alpha();
	}

}




