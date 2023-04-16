#include "SHADER_DEFINES.hpp"

/* 상수테이블 : 바뀔 수 없는 값. */
/* 클라이언트에서부터 값을 전달받아오기위한 변수 */
matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

/* 클라에서 값을 전달달하지 않았을 경우, 0.f로 사용됨, 전달된 값이 있다면 초기값은 무시된다. */
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
	/* 위치벡터는 반드시 float4여야만 한다. */
	float4			vPosition : SV_POSITION;
	float2			vTexUV : TEXCOORD0;
};

struct VS_EFFECT_OUT
{
	/* 위치벡터는 반드시 float4여야만 한다. */
	float4			vPosition : SV_POSITION;
	float2			vTexUV0 : TEXCOORD0;
	float2			vTexUV1 : TEXCOORD1;
	float2			vTexUV2 : TEXCOORD2;
	float2			vTexUV3 : TEXCOORD3;
};

/* 버텍스 셰이더 */
/* 1. 정점위치의 변환(월드, 뷰, 투영) */
/* 2. 정점의 구성 정보를 변형한다.(정점의 멤버를 추가, 필요없는 멤버를 소거한다) : VS_IN과 VS_OUT은 꼭 같아야할 이유는 없다.   */
VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT			Out = (VS_OUT)0;

	matrix			matWV, matWVP;
	
	/* 모든 행렬의 곱을 수행한다. 단, 곱하기가 가능한 행렬들에 대해서.  */
	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;

	return Out;
}

/*세개가 모이면 */
/* SV_POSITION 변수에 대해서 W나누기를 수행한다. */
/* 뷰포트 변환을 거치낟. */

/* 해당정점으로 만들어진 영역이 화면에 보여야하는 영역이 아니다라고 한다라면 discard한다. */

/* 래스터라이즈 : 정점 세개에 둘러쌓여진 픽셀들을 정점정보를 기반으로하여(VS_OUT == PS_IN) 생성한다. */
/* 이렇게 만들어진 픽셀들을 하나하나 PS_MAIN함수릃 호출하며 인자로 전다라한ㄷ. */

struct PS_IN
{
	float4			vPosition : SV_POSITION;
	float2			vTexUV : TEXCOORD0;
};

struct PS_OUT
{
	/* SV_TARGET0 : 0번째 렌더타겟에 그리기위한 색상이다. */
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

/* 픽셀셰이더 */
/* 픽셀의 정보를 바탕으로하여 픽셀의 색을 결정한다 . */
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

	//알파가 커질수록 검정색에 가깝게
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

		/* 렌더스테이츠를 정의한다. */
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

		/* 렌더스테이츠를 정의한다. */
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

		/* 렌더스테이츠를 정의한다. */
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