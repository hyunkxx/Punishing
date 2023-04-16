#include "SHADER_DEFINES.hpp"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

float g_CurrentCount; //현재 인덱스
float2 g_SpriteXY; //가로 세로 갯수
texture2D g_DiffuseTexture;

struct VS_IN
{
	float3 vPosition : POSITION;
	float2 vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	float4 vPosition : SV_POSITION;
	float2 vTexUV : TEXCOORD0;
};

VS_OUT vs_main(VS_IN In)
{
	VS_OUT Out = (VS_OUT)0;
		   
	matrix matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;

	return Out;
}

struct PS_IN
{
	float4 vPosition : SV_POSITION;
	float2 vTexUV : TEXCOORD0;
};
struct PS_BLOOM
{
	float4 vColor : SV_TARGET0;
	float4 vBloom : SV_TARGET1;
};
struct PS_DISTORTION
{
	float4 vColor : SV_TARGET0;
	float4 vBloom : SV_TARGET1;
	float4 vDistortion : SV_TARGET2;
};

struct PS_OUT
{
	float4 vColor : SV_TARGET0;
};

PS_BLOOM ps_sprite(PS_IN In)
{
	PS_BLOOM Out = (PS_BLOOM)0;

	float2 uv = In.vTexUV;
	float2 SpriteCount = g_SpriteXY;

	float fHeight = floor(g_CurrentCount / g_SpriteXY.x);
	float fWidth = frac(g_CurrentCount / g_SpriteXY.x);

	uv.x = In.vTexUV.x / g_SpriteXY.x + fWidth;
	uv.y = In.vTexUV.y / g_SpriteXY.y + (fHeight * (1 / g_SpriteXY.y));

	Out.vColor = g_DiffuseTexture.Sample(LinearSampler, uv);
	Out.vBloom = Out.vColor * float4(0.f, 0.f, 0.f, 0.f);

	return Out;
}

PS_BLOOM ps_sprite_mask(PS_IN In)
{
	PS_BLOOM Out = (PS_BLOOM)0;

	float2 uv = In.vTexUV;
	float2 SpriteCount = g_SpriteXY;

	float fHeight = floor(g_CurrentCount / g_SpriteXY.x);
	float fWidth = frac(g_CurrentCount / g_SpriteXY.x);

	uv.x = In.vTexUV.x / g_SpriteXY.x + fWidth;
	uv.y = In.vTexUV.y / g_SpriteXY.y + (fHeight * (1 / g_SpriteXY.y));
	
	Out.vColor.rgb = g_DiffuseTexture.Sample(LinearSampler, uv).r;
	Out.vColor.a = g_DiffuseTexture.Sample(LinearSampler, uv).r;
	Out.vBloom = Out.vColor * float4(0.f, 0.f, 0.f, 0.f);

	return Out;
}


PS_BLOOM ps_sprite_mask_to_white(PS_IN In)
{
	PS_BLOOM Out = (PS_BLOOM)0;

	float2 uv = In.vTexUV;
	float2 SpriteCount = g_SpriteXY;

	float fHeight = floor(g_CurrentCount / g_SpriteXY.x);
	float fWidth = frac(g_CurrentCount / g_SpriteXY.x);

	uv.x = In.vTexUV.x / g_SpriteXY.x + fWidth;
	uv.y = In.vTexUV.y / g_SpriteXY.y + (fHeight * (1 / g_SpriteXY.y));

	Out.vColor.rgb = g_DiffuseTexture.Sample(LinearSampler, uv).r / g_DiffuseTexture.Sample(LinearSampler, uv).r;
	Out.vColor.a = g_DiffuseTexture.Sample(LinearSampler, uv).r;
	Out.vBloom = Out.vColor * float4(0.f, 0.f, 0.f, 0.f);

	return Out;
}


technique11 DefaultTechnique
{
	pass Pass0
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZTest_NoZWrite, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 vs_main();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 ps_sprite();
	}

	//마스크 이미지를 디퓨즈로 이용
	pass Pass1
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZTest_NoZWrite, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 vs_main();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 ps_sprite_mask();
	}
	
	//흰색으로
	pass Pass2
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZTest_NoZWrite, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 vs_main();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 ps_sprite_mask_to_white();
	}
}