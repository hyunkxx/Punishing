#include "SHADER_DEFINES.hpp"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_BufferTexture;
texture2D g_DistortionTexture;

float TimeAcc;

float EffectDuration = 0.5;
float EffectFadeInTimeFactor = 0.5;
float EffectWidth = 0.8;
float EffectMaxTexelOffset = 20.0;

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
	float4			vBloom : SV_TARGET1;
	float4			vDistortion : SV_TARGET2;
};

float2 GetOffsetFromCenter(float2 screenCoords, float2 screenSize)
{
	float2 halfScreenSize = screenSize / 2.0;
	return (screenCoords.xy - halfScreenSize) / min(halfScreenSize.x, halfScreenSize.y);
}

float3 GetTextureOffset(float2 coords, float2 textureSize, float2 texelOffset)
{
	float2 texelSize = 1.0 / textureSize;
	float2 offsetCoords = coords + texelSize * texelOffset;

	float2 halfTexelSize = texelSize / 2.0;
	float2 clampedOffsetCoords = clamp(offsetCoords, halfTexelSize, 1.0 - halfTexelSize);

	return g_BufferTexture.Sample(LinearSampler, coords);
}

float2 GetDistortionTexelOffset(float2 offsetDirection, float offsetDistance, float time)
{
	float progress = fmod(time, EffectDuration) / EffectDuration;

	float halfWidth = EffectWidth / 2.0f;
	float lower = 1.0 - smoothstep(progress - halfWidth, progress, offsetDistance);
	float upper = smoothstep(progress, progress + halfWidth, offsetDistance);

	float band = 1.0 - (upper + lower);

	float strength = 2.0 - progress;
	float fadeStrength = smoothstep(0.0, EffectFadeInTimeFactor, progress);

	float distortion = band * strength * fadeStrength;

	return distortion * offsetDirection * EffectMaxTexelOffset;
}

PS_OUT PS_DISTORTION(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	//float time = TimeAcc;

	//float2 screenCoords = { 1280, 720 };
	//float2 screenSize = { 1280, 720 };

	//float2 offsetFromCenter = GetOffsetFromCenter(screenCoords, screenSize);
	//float2 offsetDirection = normalize(-offsetFromCenter);
	//float offsetDistance = length(offsetFromCenter);

	//float2 offset = GetDistortionTexelOffset(offsetDirection, offsetDistance, time);

	//float2 coords = (In.vTexUV.xy / screenSize);
	//coords.y = 1.0 + coords.y;

	//float3 background = GetTextureOffset(coords, screenSize, offset);

	float2 Trans = In.vTexUV;
	Trans.x -= TimeAcc;
	//float2 Trans = In.vTexUV + TimeAcc;
	float4 Noise = g_DistortionTexture.Sample(LinearClampSampler, Trans);
	float2 UV = In.vTexUV;// +Noise.xy * 0.01f;
	UV += Trans * 0.01f;

	float4 Orig = g_BufferTexture.Sample(LinearClampSampler, UV);

	Out.vColor = Orig;
	Out.vBloom = Orig;
	Out.vDistortion = Orig;

	return Out;
}


PS_OUT PS_DISTORTION_2(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float2 Trans = In.vTexUV;
	Trans.x -= TimeAcc;

	float4 Noise = g_DistortionTexture.Sample(LinearClampSampler, Trans);

	float2 UV = In.vTexUV + Noise.xy * 0.05f;
	float4 Orig = g_BufferTexture.Sample(LinearClampSampler, UV);

	Out.vColor = Orig;

	return Out;
}

technique11 DefaultTechnique
{
	pass Pass0
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Not_ZTest_ZWrite, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_BLUR();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_DISTORTION();
	}

	pass Pass1
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Not_ZTest_ZWrite, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_BLUR();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_DISTORTION_2();
	}
}