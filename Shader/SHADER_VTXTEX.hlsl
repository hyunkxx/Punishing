#include "SHADER_DEFINES.hpp"

/* ������̺� : �ٲ� �� ���� ��. */
/* Ŭ���̾�Ʈ�������� ���� ���޹޾ƿ������� ���� */
matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

/* Ŭ�󿡼� ���� ���޴����� �ʾ��� ���, 0.f�� ����, ���޵� ���� �ִٸ� �ʱⰪ�� ���õȴ�. */
texture2D g_Texture;
texture2D g_MaskTexture;
float g_TimeAcc;

struct VS_IN
{
	float3			vPosition : POSITION;
	float2			vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	/* ��ġ���ʹ� �ݵ�� float4���߸� �Ѵ�. */
	float4			vPosition : SV_POSITION;
	float2			vTexUV : TEXCOORD0;
};

struct VS_EFFECT_OUT
{
	/* ��ġ���ʹ� �ݵ�� float4���߸� �Ѵ�. */
	float4			vPosition : SV_POSITION;
	float2			vTexUV0 : TEXCOORD0;
	float2			vTexUV1 : TEXCOORD1;
	float2			vTexUV2 : TEXCOORD2;
	float2			vTexUV3 : TEXCOORD3;
};

/* ���ؽ� ���̴� */
/* 1. ������ġ�� ��ȯ(����, ��, ����) */
/* 2. ������ ���� ������ �����Ѵ�.(������ ����� �߰�, �ʿ���� ����� �Ұ��Ѵ�) : VS_IN�� VS_OUT�� �� ���ƾ��� ������ ����.   */
VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT			Out = (VS_OUT)0;

	matrix			matWV, matWVP;
	
	/* ��� ����� ���� �����Ѵ�. ��, ���ϱⰡ ������ ��ĵ鿡 ���ؼ�.  */
	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;

	return Out;
}

/*������ ���̸� */
/* SV_POSITION ������ ���ؼ� W�����⸦ �����Ѵ�. */
/* ����Ʈ ��ȯ�� ��ġ��. */

/* �ش��������� ������� ������ ȭ�鿡 �������ϴ� ������ �ƴϴٶ�� �Ѵٶ�� discard�Ѵ�. */

/* �����Ͷ����� : ���� ������ �ѷ��׿��� �ȼ����� ���������� ��������Ͽ�(VS_OUT == PS_IN) �����Ѵ�. */
/* �̷��� ������� �ȼ����� �ϳ��ϳ� PS_MAIN�Լ��f ȣ���ϸ� ���ڷ� ���ٶ��Ѥ�. */

struct PS_IN
{
	float4			vPosition : SV_POSITION;
	float2			vTexUV : TEXCOORD0;
};

struct PS_OUT
{
	/* SV_TARGET0 : 0��° ����Ÿ�ٿ� �׸������� �����̴�. */
	float4			vColor : SV_TARGET0;
};

/* �ȼ����̴� */
/* �ȼ��� ������ ���������Ͽ� �ȼ��� ���� �����Ѵ� . */
PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;
	//Out.vColor = tex2D(DefaultSampler, In.vTexUV);
	Out.vColor = g_Texture.Sample(LinearSampler, In.vTexUV);

	return Out;
}

PS_OUT PS_MAIN_ALPHA(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = g_Texture.Sample(LinearSampler, In.vTexUV);

	return Out;
}

VS_OUT VS_EFFECT(VS_IN In)
{
	VS_OUT Out = (VS_OUT)0;

	matrix			matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);

	Out.vTexUV = In.vTexUV;

	//Out.vTexUV1 = In.vTexUV * g_TimeAcc;
	//Out.vTexUV1.y = Out.vTexUV1.y + 1.f * 1.f;

	//Out.vTexUV2 = In.vTexUV * g_TimeAcc;
	//Out.vTexUV2.y = Out.vTexUV2.y + 1.f * 0.8f;

	//Out.vTexUV3 = In.vTexUV * g_TimeAcc;
	//Out.vTexUV3.y = Out.vTexUV3.y + 1.f * 0.3f;

	return Out;
}

PS_OUT PS_EFFECT1(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;
	
	//float4 noise1, noise2, noise3, finNoise;

	//noise1 = g_MaskTexture.Sample(LinearSampler, In.vTexUV1);
	//noise2 = g_MaskTexture.Sample(LinearSampler, In.vTexUV2);
	//noise3 = g_MaskTexture.Sample(LinearSampler, In.vTexUV3);

	//float2 distortion1, distortion2, distortion3;
	////distortion1.xy = 0.1f, 0.f;
	////distortion2.xy = 0.f, 0.1f;
	////distortion3.xy = -0.1f, 0.f;

	//noise1.xy = noise1.xy;// * distortion1;
	//noise2.xy = noise2.xy;// * distortion2;
	//noise3.xy = noise3.xy;// * distortion3;
	//finNoise = noise1 + noise2 + noise3;

	//float perturb = ((1.f - In.vTexUV0.y) * 0.1f) + 1.f;
	//float2 noiseCoords;
	//noiseCoords.xy = (finNoise.xy * perturb) + In.vTexUV0.xy;
	
	//float2 uv = In.vTexUV;
	//uv.y = g_TimeAcc;

	//Out.vColor = g_Texture.Sample(LinearSampler, uv);
	////Out.vColor.a = g_MaskTexture.Sample(LinearSampler, uv).r;


	float2 Trans = In.vTexUV;
	//Trans.x -= g_TimeAcc;
	Trans = In.vTexUV + g_TimeAcc;
	float4 Noise = g_MaskTexture.Sample(LinearSampler, Trans);
	float2 UV = In.vTexUV +Noise.xy * 0.05f;

	float4 noise1 = g_MaskTexture.Sample(LinearSampler, In.vTexUV);
	noise1.xy = noise1.xy * Noise;
	float perturb = ((1.f - In.vTexUV.y) * 0.05f) + 3.f;
	float2 noiseCoords;
	noiseCoords.xy = (noise1.xy * perturb) + UV;

	float4 Orig = g_Texture.Sample(LinearSampler, noiseCoords) * Noise;
	Out.vColor = Orig;
	Out.vColor.a = noise1.r;

	/*if (Out.vColor.a <= 0.01f)
		discard;*/

	return Out;
}


PS_OUT PS_EFFECT2(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	float2 Trans = In.vTexUV;
	//Trans.x -= g_TimeAcc;
	Trans = In.vTexUV - g_TimeAcc;
	float4 Noise = g_MaskTexture.Sample(LinearSampler, Trans);
	float2 UV = In.vTexUV + Noise.xy * 0.01f;

	float4 noise1 = g_MaskTexture.Sample(LinearSampler, In.vTexUV);
	noise1.xy = noise1.xy * Noise;
	float perturb = ((1.f - In.vTexUV.y) * 0.05f) + 1.f;
	float2 noiseCoords;
	noiseCoords.xy = (noise1.xy * perturb) + UV;

	float4 Orig = g_Texture.Sample(LinearSampler, noiseCoords) * Noise;
	Out.vColor = Orig;
	Out.vColor.a = noise1.r;

	/*if (Out.vColor.a <= 0.01f)
	discard;*/

	return Out;
}

PS_OUT PS_EFFECT3(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = g_Texture.Sample(LinearSampler, In.vTexUV);
	Out.vColor.b = 0;

	if (Out.vColor.a <= 0.0f)
		discard;

	if (g_TimeAcc >= 2.f)
		Out.vColor.a = saturate(Out.vColor.a * 4.f - g_TimeAcc);

	return Out;
}

technique11 DefaultTechnique
{
	/* ���ؽ�, �ȼ� ���̴��� �ٸ� �Լ��� ���ؼ� �����ϰ����Ҷ�. */
	/* �������������� �ٸ��� ������ �� �� */
	pass BackGround0
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		/* �������������� �����Ѵ�. */
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass Alpha1
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(1.0f, 0.f, 0.f, 0.f), 0xffffffff);

		/* �������������� �����Ѵ�. */
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_ALPHA();
	}

	pass UI2
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		/* �������������� �����Ѵ�. */
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass Effect_3
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZTest_NoZWrite, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_EFFECT();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_EFFECT1();
	}

	pass Effect2_4
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZTest_NoZWrite, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_EFFECT();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_EFFECT2();
	}

	pass Effect3_5
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZTest_NoZWrite, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_EFFECT();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_EFFECT3();
	}
}