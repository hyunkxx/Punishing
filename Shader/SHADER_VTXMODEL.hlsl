#include "SHADER_DEFINES.hpp"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_DiffuseTexture;
vector g_vMatAmbient = vector(0.4f, 0.4f, 0.4f, 1.f);
vector g_vMatSpecular = vector(1.f, 1.f, 1.f, 1.f);

vector g_vCamPosition;

vector g_vLightDir = vector(1.f, -1.f, 1.f, 0.f);
vector g_vLightPos = vector(5.f, 3.f, 5.f, 1.f);
float g_fLength = 10.f;
float g_fPower = 50.f;

float g_LightPower = 1.f;
float g_fTimeAcc = 0.f;

/* Á¶¸íÀÇ »ö»ó */
vector g_vLightDiffuse = vector(1.f, 1.f, 1.f, 1.f);
vector g_vLightAmbient = vector(1.f, 1.f, 1.f, 1.f);
vector g_vLightSpecular = vector(1.f, 1.f, 1.f, 1.f);

float4 g_GlowColor = float4(1.f, 1.f, 1.f, 1.f);

//µðÁ¹ºê±â´É
texture2D g_DissolveTexture;
float g_fDissolveAmount = 1.f;
float g_fGlowRange = 0.005f;
float g_fGlowFalloff = 0.1f;

//±×¸²ÀÚ
matrix g_LightViewMatrix, g_LightProjMatrix;
float4 g_LightPos;

struct VS_SHADOWOUT
{
	float4 vPosition : SV_POSITION;
	float3 vNormal : NORMAL;
	float2 vTexUV : TEXCOORD0;
	float4 vLook : TEXCOORD1;
	float4 vWorldPos : TEXCOORD2;
	float4 vDepth : TEXCOORD3;
	float4 vShadowDepth : TEXCOORD4;
};

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
	float4 vColor : SV_TARGET0;
};

struct PS_BLOOMOUT
{
	float4 vColor : SV_TARGET0;
	float4 vColorBloom : SV_TARGET1;
};

struct PS_DISTORTION
{
	float4 vColor : SV_TARGET0;
	float4 vColorBloom : SV_TARGET1;
	float4 vDistortion : SV_TARGET2;
};

struct PS_SHADOWOUT
{
	float4 vColor : SV_TARGET0;
	float4 vColorBloom : SV_TARGET1;
	float4 vDistortion : SV_TARGET2;
	float4 vDepth : SV_TARGET3;
	float4 vShadowDepth : SV_TARGET4;
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

VS_SHADOWOUT VS_SHADOW(VS_IN In)
{
	VS_SHADOWOUT Out = (VS_SHADOWOUT)0;

	matrix matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	vector worldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
	vector lookDir = worldPos - g_vCamPosition;

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;
	Out.vNormal = In.vNormal;
	Out.vLook = lookDir;

	//µª½º
	Out.vDepth = Out.vPosition;

	//½¦µµ¿ì µª½º
	Out.vShadowDepth = mul(In.vPosition, g_WorldMatrix);
	Out.vShadowDepth = mul(Out.vShadowDepth, g_LightViewMatrix);
	Out.vShadowDepth = mul(Out.vShadowDepth, g_LightProjMatrix);
	
	return Out;
}

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	vector vWorldNormal = normalize(mul(float4(In.vNormal, 0.f), g_WorldMatrix));
	float fShade = max(dot(normalize(g_vLightDir) * -1.f, vWorldNormal), 0.f);
	vector vReflect = reflect(normalize(g_vLightDir), vWorldNormal);

	float fSpecular = pow(max(dot(normalize(vReflect) * -1.f, normalize(In.vLook)), 0.f), g_fPower);
	vector vMatDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV) * float4(g_LightPower, g_LightPower, g_LightPower, g_LightPower);

	Out.vColor = (g_vLightDiffuse * vMatDiffuse);

	if (Out.vColor.a <= 0.1f)
		discard;

	return Out;
}

PS_SHADOWOUT PS_SHADOW(VS_SHADOWOUT In)
{
	PS_SHADOWOUT Out = (PS_SHADOWOUT)0;

	vector vWorldNormal = normalize(mul(float4(In.vNormal, 0.f), g_WorldMatrix));
	float fShade = max(dot(normalize(g_vLightDir) * -1.f, vWorldNormal), 0.f);
	vector vReflect = reflect(normalize(g_vLightDir), vWorldNormal);

	float fSpecular = pow(max(dot(normalize(vReflect) * -1.f, normalize(In.vLook)), 0.f), g_fPower);
	vector vMatDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV) * float4(g_LightPower, g_LightPower, g_LightPower, g_LightPower);

	Out.vColor = (g_vLightDiffuse * vMatDiffuse);

	if (Out.vColor.a <= 0.1f)
		discard;

	Out.vDepth = vector(In.vDepth.w / 1000.f, In.vDepth.z / In.vDepth.w, 0.f, 1.f);
	Out.vShadowDepth = vector(In.vShadowDepth.w / 1000.f, In.vShadowDepth.z / In.vShadowDepth.w, 0.f, 1.f);
	
	return Out;
}


PS_BLOOMOUT PS_BLACK(PS_IN In)
{
	PS_BLOOMOUT Out = (PS_BLOOMOUT)0;

	Out.vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	if (Out.vColor.a <= 0.1f)
		discard;

	Out.vColorBloom = float4(0.f, 0.f, 0.f, 0.f);

	return Out;
}

PS_OUT PS_BLACK_ALPHA(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;
	Out.vColor = float4(0.f, 0.f, 0.f, 0.3f);
	if (Out.vColor.a <= 0.1f)
		discard;

	return Out;
}

PS_OUT PS_WHITE_ALPHA(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;
	Out.vColor = float4(1.f, 1.f, 1.f, 0.3f);
	if (Out.vColor.a <= 0.1f)
		discard;

	return Out;
}


PS_OUT PS_WHITE(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	Out.vColor = float4(1.f, 1.f, 1.f, 1.f);
	if (Out.vColor.a <= 0.1f)
		discard;
	
	return Out;
}

PS_OUT PS_BACKGROUND(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	vector vWorldNormal = normalize(mul(float4(In.vNormal, 0.f), g_WorldMatrix));
	float fShade = max(dot(normalize(g_vLightDir) * -1.f, vWorldNormal), 0.f);

	if (fShade > 0.6f)
		fShade = 1.f;
	else if (fShade > 0.4f)
		fShade = 0.6f;
	else if (fShade > 0.1f)
		fShade = 0.4f;
	else if (fShade > 0.1f)
		fShade = 0.0f;

	vector vReflect = reflect(normalize(g_vLightDir), vWorldNormal);

	float fSpecular = pow(max(dot(normalize(vReflect) * -1.f, normalize(In.vLook)), 0.f), g_fPower);
	vector vMatDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

	Out.vColor = (g_vLightDiffuse * vMatDiffuse);// *saturate(fShade + (g_vLightAmbient * g_vMatAmbient));

	if (Out.vColor.a <= 0.1f)
		discard;

	return Out;
}

PS_BLOOMOUT PS_SKY(PS_IN In)
{
	PS_BLOOMOUT Out = (PS_BLOOMOUT)0;

	vector vWorldNormal = normalize(mul(float4(In.vNormal, 0.f), g_WorldMatrix));
	float fShade = max(dot(normalize(g_vLightDir) * -1.f, vWorldNormal), 0.f);

	if (fShade > 0.6f)
		fShade = 1.f;
	else if (fShade > 0.4f)
		fShade = 0.6f;
	else if (fShade > 0.1f)
		fShade = 0.4f;
	else if (fShade > 0.1f)
		fShade = 0.0f;

	vector vReflect = reflect(normalize(g_vLightDir), vWorldNormal);

	float fSpecular = pow(max(dot(normalize(vReflect) * -1.f, normalize(In.vLook)), 0.f), g_fPower);
	vector vMatDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

	Out.vColor = (g_vLightDiffuse * vMatDiffuse);// *saturate(fShade + (g_vLightAmbient * g_vMatAmbient));

	if (Out.vColor.a <= 0.1f)
		discard;
	
	float vCol = Out.vColor.r + Out.vColor.g + Out.vColor.b;
	Out.vColorBloom = Out.vColor;

	return Out;
}

PS_OUT PS_RIM(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	vector vCamDir = In.vLook;
	float rim = 0;
	rim = 1 - saturate(dot(In.vNormal, -vCamDir));

	rim = pow(rim, 80.f);

	float4 rimColor = float4(0.2f, 0.2f, 0.2f, 1.f);
	rimColor = rim * rimColor;

	Out.vColor = rimColor;
	return Out;
}

PS_OUT PS_ALPHA(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	vector vWorldNormal = normalize(mul(float4(In.vNormal, 0.f), g_WorldMatrix));
	float fShade = max(dot(normalize(g_vLightDir) * -1.f, vWorldNormal), 0.f);

	if (fShade > 0.6f)
		fShade = 1.f;
	else if (fShade > 0.4f)
		fShade = 0.6f;
	else if (fShade > 0.1f)
		fShade = 0.4f;
	else if (fShade > 0.1f)
		fShade = 0.0f;

	vector vReflect = reflect(normalize(g_vLightDir), vWorldNormal);

	float fSpecular = pow(max(dot(normalize(vReflect) * -1.f, normalize(In.vLook)), 0.f), g_fPower);
	vector vMatDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

	Out.vColor = (g_vLightDiffuse * vMatDiffuse);// *saturate(fShade + (g_vLightAmbient * g_vMatAmbient));

	if (Out.vColor.a <= 0.1f)
		discard;

	Out.vColor.a = 0.5f;

	return Out;
}

PS_BLOOMOUT PS_FREEZEDARK(PS_IN In)
{
	PS_BLOOMOUT Out = (PS_BLOOMOUT)0;

	vector vWorldNormal = normalize(mul(float4(In.vNormal, 0.f), g_WorldMatrix));
	float fShade = max(dot(normalize(g_vLightDir) * -1.f, vWorldNormal), 0.f);
	vector vReflect = reflect(normalize(g_vLightDir), vWorldNormal);

	float fSpecular = pow(max(dot(normalize(vReflect) * -1.f, normalize(In.vLook)), 0.f), g_fPower);
	vector vMatDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV) * float4(g_LightPower, g_LightPower, g_LightPower, 1.f);

	Out.vColor = (g_vLightDiffuse * vMatDiffuse);

	if (Out.vColor.a <= 0.1f)
		discard;

	Out.vColorBloom = float4(0.f, 0.f, 0.f, 0.f);

	return Out;
}

PS_OUT PS_GARD(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	vector vWorldNormal = normalize(mul(float4(In.vNormal, 0.f), g_WorldMatrix));
	
	float2 uv = In.vTexUV;
	uv.x += g_fTimeAcc;
		
	Out.vColor = g_DiffuseTexture.Sample(LinearSampler, uv);
	Out.vColor.r = Out.vColor.r;
	Out.vColor.gb = 0.f;

	if (g_fTimeAcc <= 0.6f)
		Out.vColor.a = g_fTimeAcc;
	else
		Out.vColor.a = 1.2f - g_fTimeAcc;

	return Out;
}

PS_BLOOMOUT PS_WHITEGLOW(PS_IN In)
{
	PS_BLOOMOUT Out = (PS_BLOOMOUT)0;

	Out.vColor = float4(1.f, 1.f, 1.f, 1.f);
	if (Out.vColor.a <= 0.1f)
		discard;
	
	Out.vColorBloom = g_GlowColor;

	return Out;
}

PS_BLOOMOUT PS_GREENGLOW(PS_IN In)
{
	PS_BLOOMOUT Out = (PS_BLOOMOUT)0;

	Out.vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	if (Out.vColor.a <= 0.1f)
		discard;

	if(Out.vColor.g >= Out.vColor.r + Out.vColor.b)
		Out.vColorBloom = g_GlowColor;

	return Out;
}

PS_BLOOMOUT PS_BOSSROOM(PS_IN In)
{
	PS_BLOOMOUT Out = (PS_BLOOMOUT)0;

	vector vWorldNormal = normalize(mul(float4(In.vNormal, 0.f), g_WorldMatrix));
	float fShade = max(dot(normalize(g_vLightDir) * -1.f, vWorldNormal), 0.f);
	vector vReflect = reflect(normalize(g_vLightDir), vWorldNormal);

	float fSpecular = pow(max(dot(normalize(vReflect) * -1.f, normalize(In.vLook)), 0.f), g_fPower);
	vector vMatDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV) * float4(g_LightPower, g_LightPower, g_LightPower, 1.f);

	Out.vColor = (g_vLightDiffuse * vMatDiffuse);
	if (Out.vColor.a <= 0.1f)
		discard;

	if (Out.vColor.r >= Out.vColor.g + Out.vColor.b)
		Out.vColorBloom = float4(Out.vColor.r, Out.vColor.g, Out.vColor.b, 1.f);

	return Out;
}

PS_BLOOMOUT PS_PLAYERTHORN(PS_IN In)
{
	PS_BLOOMOUT Out = (PS_BLOOMOUT)0;

	Out.vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

	Out.vColor.g = (Out.vColor.r * 10.f);
	Out.vColor.b = (Out.vColor.r * 10.f) * 0.6f;
	Out.vColor.r = 0.f;

	if (Out.vColor.a <= 0.1f)
		discard;

	if (Out.vColor.r >= Out.vColor.g + Out.vColor.b)
		Out.vColorBloom = float4(Out.vColor.r, Out.vColor.g, Out.vColor.b, 1.f);

	return Out;
}

PS_BLOOMOUT PS_DESSOLVE(PS_IN In)
{
	PS_BLOOMOUT Out = (PS_BLOOMOUT)0;

	float dissolve = g_DissolveTexture.Sample(LinearSampler, In.vTexUV).r;
	dissolve = dissolve * 0.999f;
	float isVisible = dissolve - (g_fDissolveAmount * 0.2f);
	clip(isVisible);

	float isGlowing = smoothstep(g_fGlowRange + g_fGlowFalloff, g_fGlowRange, isVisible);
	float3 vGlowColor = isGlowing * float4(0.5f, 0.5f, 0.5f, 1.f);

	vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

	if (vMtrlDiffuse.a <= 0.1f)
		discard;

	float fShade = saturate(dot(normalize(g_vLightDir) * -1.f, In.vNormal));

	if (fShade > 0.9f)
		fShade = 1.f;
	else if (fShade > 0.7f)
		fShade = 0.7f;
	else if (fShade > 0.4f)
		fShade = 0.4f;
	else
		fShade = 0.0f;

	Out.vColor = (g_vLightDiffuse * vMtrlDiffuse);
	Out.vColor.xyz += vGlowColor;

	return Out;
}

PS_OUT PS_UV_ROTX(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	float2 uv = In.vTexUV;

	uv.x += g_fTimeAcc;

	float fShade = saturate(dot(normalize(g_vLightDir) * -1.f, In.vNormal));
	if (fShade > 0.9f)
		fShade = 1.f;
	else if (fShade > 0.7f)
		fShade = 0.7f;
	else if (fShade > 0.4f)
		fShade = 0.4f;
	else
		fShade = 0.0f;

	Out.vColor = g_DiffuseTexture.Sample(LinearSampler, uv);
	Out.vColor = Out.vColor * (1.f - g_fTimeAcc);

	return Out;
}

PS_BLOOMOUT PS_UV_ROTX2(PS_IN In)
{
	PS_BLOOMOUT Out = (PS_BLOOMOUT)0;

	float2 uv = In.vTexUV;

	uv.x += g_fTimeAcc;

	float fShade = saturate(dot(normalize(g_vLightDir) * -1.f, In.vNormal));
	if (fShade > 0.9f)
		fShade = 1.f;
	else if (fShade > 0.7f)
		fShade = 0.7f;
	else if (fShade > 0.4f)
		fShade = 0.4f;
	else
		fShade = 0.0f;

	Out.vColor = g_DiffuseTexture.Sample(LinearSampler, uv);
	Out.vColor.r = 0.f;
	Out.vColor.g = 0.f;
	Out.vColor.b = 0.f;
	Out.vColor = Out.vColor * (1.f - g_fTimeAcc);
	Out.vColorBloom = Out.vColor;

	return Out;
}

PS_DISTORTION PS_EvolitionEffect(PS_IN In)
{
	PS_DISTORTION Out = (PS_DISTORTION)0;

	float2 uv = In.vTexUV;

	uv.x += g_fTimeAcc * 2.f;

	float fShade = saturate(dot(normalize(g_vLightDir) * -1.f, In.vNormal));
	if (fShade > 0.9f)
		fShade = 1.f;
	else if (fShade > 0.7f)
		fShade = 0.7f;
	else if (fShade > 0.4f)
		fShade = 0.4f;
	else
		fShade = 0.0f;

	Out.vColor = g_DiffuseTexture.Sample(LinearSampler, uv);
	Out.vColor = Out.vColor * (1.f - g_fTimeAcc);
	Out.vColor.r = Out.vColor.a;

	Out.vDistortion = Out.vColor;
	Out.vColor = float4(0.f, 0.f, 0.f, 0.f);

	return Out;
}

technique11 DefaultTechnique
{
	pass BackGround
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_BACKGROUND();
	}

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

	pass Black
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_BLACK();
	}

	pass Alpha
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

	pass FreezeDark4
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_FREEZEDARK();
	}

	pass SKY5
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZTest_NoZWrite, 0);
		SetBlendState(BS_Default, float4(1.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_SKY();
	}

	pass GARD6
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZTest_NoZWrite, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_GARD();
	}

	pass Pass7
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_RIM();
	}

	pass White8
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_WHITE();
	}

	pass BlackAlpha9
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_BLACK_ALPHA();
	}

	pass BlackAlpha10
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_WHITE_ALPHA();
	}

	pass GlowPass11
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_WHITEGLOW();
	}

	pass GlowGreen12
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_GREENGLOW();
	}

	pass GlowLevel13
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_BOSSROOM();
	}

	pass PlayerThorn14
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_PLAYERTHORN();
	}

	pass DESSOLVE15
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

	pass UV_ROTX16
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_UV_ROTX();
	}

	pass UV_ROTX217
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_UV_ROTX2();
	}

	pass EvolitionEffect_Pass18
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_EvolitionEffect();
	}

	pass Shadow_Pass19
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_SHADOW();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_SHADOW();
	}
}




