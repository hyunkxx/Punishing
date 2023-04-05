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

struct PS_OUT
{
	float4 vColor : SV_TARGET0;
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

PS_OUT PS_BLACK(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	Out.vColor = float4(0.f, 0.f, 0.f, 1.f);
	if (Out.vColor.a <= 0.1f)
		discard;

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

	//Out.vColor *= Out.vColor * 3.f;
	//Out.vColor *= Out.vColor * 2.5f;

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

PS_OUT PS_FREEZEDARK(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	vector vWorldNormal = normalize(mul(float4(In.vNormal, 0.f), g_WorldMatrix));
	float fShade = max(dot(normalize(g_vLightDir) * -1.f, vWorldNormal), 0.f);
	vector vReflect = reflect(normalize(g_vLightDir), vWorldNormal);

	float fSpecular = pow(max(dot(normalize(vReflect) * -1.f, normalize(In.vLook)), 0.f), g_fPower);
	vector vMatDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV) * float4(g_LightPower, g_LightPower, g_LightPower, 1.f);

	Out.vColor = (g_vLightDiffuse * vMatDiffuse);

	if (Out.vColor.a <= 0.1f)
		discard;

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

	if (g_fTimeAcc <= 1.f)
		Out.vColor.a = g_fTimeAcc;
	else
		Out.vColor.a = 2.f - g_fTimeAcc;

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
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

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
		SetDepthStencilState(DS_Not_ZTest_ZWrite, 0);
		SetBlendState(BS_AlphaBlend, float4(1.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_BACKGROUND();
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

}




