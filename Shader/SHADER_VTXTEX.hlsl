#include "SHADER_DEFINES.hpp"

/* ������̺� : �ٲ� �� ���� ��. */
/* Ŭ���̾�Ʈ�������� ���� ���޹޾ƿ������� ���� */
matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

/* Ŭ�󿡼� ���� ���޴����� �ʾ��� ���, 0.f�� ����, ���޵� ���� �ִٸ� �ʱⰪ�� ���õȴ�. */
texture2D g_Texture;
texture2D g_MaskTexture;
float g_TimeAcc;

float g_WidthCount;
float g_CurrentCount;

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

struct PS_BLOOM
{
	float4			vColor : SV_TARGET0;
	float4			vBloom : SV_TARGET1;
};

struct PS_DISTORTION
{
	float4 vColor : SV_TARGET0;
	float4 vBloom : SV_TARGET1;
	float4 vDistortion : SV_TARGET2;
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

PS_BLOOM PS_WHITE_TO_RED(PS_IN In)
{
	PS_BLOOM			Out = (PS_BLOOM)0;

	Out.vColor = g_Texture.Sample(LinearSampler, In.vTexUV);

	if (g_TimeAcc >= 0.9f)
		Out.vColor = g_Texture.Sample(LinearSampler, In.vTexUV);
	else
	{
		Out.vColor.r = Out.vColor.r;
		Out.vColor.r = Out.vColor.r * g_TimeAcc;
		Out.vColor.g = 0.f;
		Out.vColor.b = 0.f;
	}
	
	Out.vColor.a = Out.vColor.a * g_TimeAcc;
	//Out.vBloom = Out.vColor;

	return Out;
}

PS_BLOOM PS_BUFF(PS_IN In)
{
	PS_BLOOM Out = (PS_BLOOM)0;

	Out.vColor = g_Texture.Sample(LinearSampler, In.vTexUV);

	if (g_TimeAcc >= 0.9f)
		Out.vColor = g_Texture.Sample(LinearSampler, In.vTexUV);
	else
	{
		Out.vColor.r = 0.f;
		Out.vColor.g = Out.vColor.r;
		Out.vColor.g = Out.vColor.r * g_TimeAcc;
		Out.vColor.b = 0.f;
	}

	//Out.vBloom = Out.vColor;
	Out.vColor.a = Out.vColor.a * g_TimeAcc;

	return Out;
}


PS_OUT PS_VOICETEX(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	Out.vColor = g_Texture.Sample(LinearSampler, In.vTexUV);
	//Out.vColor.a = Out.vColor.a * g_TimeAcc;

	return Out;
}

PS_OUT PS_STAGE(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	Out.vColor = g_Texture.Sample(LinearSampler, In.vTexUV);
	Out.vColor.a = 0.9f;

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

PS_DISTORTION PS_EFFECT1(PS_IN In)
{
	PS_DISTORTION Out = (PS_DISTORTION)0;
	
	float2 Trans = In.vTexUV;

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
	//Out.vBloom = Out.vColor;
	//Out.vDistortion = noise1;

	return Out;
}


PS_DISTORTION PS_EFFECT2(PS_IN In)
{
	PS_DISTORTION Out = (PS_DISTORTION)0;

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

	Out.vBloom = Out.vColor;
	//Out.vDistortion = noise1;

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

PS_BLOOM PS_BLOOMMAIN(PS_IN In)
{
	PS_BLOOM Out = (PS_BLOOM)0;

	Out.vColor = g_Texture.Sample(LinearSampler, In.vTexUV);
	Out.vBloom = Out.vColor;

	return Out;
}

PS_BLOOM PS_SKILL(PS_IN In)
{
	PS_BLOOM Out = (PS_BLOOM)0;

	Out.vColor = g_Texture.Sample(LinearSampler, In.vTexUV);
	Out.vColor.rgb *= 0.6f;
	Out.vColor.a = saturate(Out.vColor.a * g_TimeAcc);

	Out.vBloom = g_MaskTexture.Sample(LinearSampler, In.vTexUV);
	if (Out.vBloom.r + Out.vBloom.g + Out.vBloom.b >= 1.f && Out.vBloom.r > Out.vBloom.g)
	{
		Out.vBloom.r = 1.f;
		Out.vBloom.g = 1.f;
		Out.vBloom.b = 1.f;
		Out.vBloom.a = saturate(Out.vBloom.a * g_TimeAcc);
	}
	else
	{
		Out.vBloom.g = Out.vBloom.r;
		Out.vBloom.r = 0.f;
		Out.vBloom.b *= 0.5f;
		Out.vBloom.a = saturate(Out.vBloom.a * g_TimeAcc);
	}

	return Out;
}

PS_BLOOM PS_ROBBYBACK(PS_IN In)
{
	PS_BLOOM Out = (PS_BLOOM)0;

	float2 uv = In.vTexUV;
	float4 color = g_Texture.Sample(LinearSampler, In.vTexUV);
	float4 mask = g_MaskTexture.Sample(LinearSampler, In.vTexUV);

	//���İ� Ŀ������ �������� ������
	color.a -= mask.a * 2.f;

	Out.vColor = saturate(color - (1.f - g_TimeAcc));

	return Out;
}

PS_BLOOM PS_ROBBYBACK2(PS_IN In)
{
	PS_BLOOM Out = (PS_BLOOM)0;

	float2 uv = In.vTexUV;
	float4 color = g_Texture.Sample(LinearSampler, In.vTexUV);
	Out.vColor = color * g_TimeAcc;

	return Out;
}

PS_DISTORTION PS_DISTORTION_X(PS_IN In)
{
	PS_DISTORTION Out = (PS_DISTORTION)0;

	float2 uv = In.vTexUV;
	uv += g_TimeAcc;
	Out.vColor = g_Texture.Sample(LinearSampler, In.vTexUV) * 0.f;

	float4 distortion = g_Texture.Sample(LinearSampler, uv);
	Out.vDistortion = distortion;

	return Out;
}

PS_BLOOM PS_SPRITE_XY(PS_IN In)
{
	PS_BLOOM	Out = (PS_BLOOM)0;

	float2 uv = In.vTexUV;

	float fHeight = floor(g_CurrentCount / g_WidthCount);
	float fWidth = frac(g_CurrentCount / g_WidthCount);

	uv.x = In.vTexUV.x / g_WidthCount + fWidth;
	uv.y = In.vTexUV.y / g_WidthCount + (fHeight * (1 / g_WidthCount));

	Out.vColor = g_Texture.Sample(LinearSampler, uv);
	Out.vColor.a = Out.vColor.a;
	Out.vBloom = Out.vColor;

	return Out;
}

technique11 DefaultTechnique
{
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

	pass Bloom_Pass6
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_EFFECT();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_BLOOMMAIN();
	}

	pass Skill_Pass7
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_SKILL();
	}

	pass RobbyBackground_Pass8
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_ROBBYBACK();
	}
	pass RobbyBackground_Pass9
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_ROBBYBACK2();
	}

	pass PS_DISTORTION_X_Pass10
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_DISTORTION_X();
	}

	pass PS_SPRITE_XY_PASS11
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZTest_NoZWrite, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_SPRITE_XY();
	}

	pass PS_WHITE_TO_RED_Pass12
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Not_ZTest_ZWrite, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_WHITE_TO_RED();
	}

	pass PS_WHITE_TO_RED_Pass13
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Not_ZTest_ZWrite, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_BUFF();
	}

	pass PS_VOICETEX_Pass14
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Not_ZTest_ZWrite, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_VOICETEX();
	}

	pass PS_STAGE_Pass15
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Not_ZTest_ZWrite, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_STAGE();
	}
}