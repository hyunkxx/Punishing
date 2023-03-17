#include "SHADER_DEFINES.hpp"

/* ������̺� : �ٲ� �� ���� ��. */
/* Ŭ���̾�Ʈ�������� ���� ���޹޾ƿ������� ���� */
matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

/* Ŭ�󿡼� ���� ���޴����� �ʾ��� ���, 0.f�� ����, ���޵� ���� �ִٸ� �ʱⰪ�� ���õȴ�. */
texture2D g_Texture;

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

	Out.vColor = float4(1.f, 0.f, 0.f, 0.2f) * g_Texture.Sample(LinearSampler, In.vTexUV);

	return Out;
}


technique11 DefaultTechnique
{
	/* ���ؽ�, �ȼ� ���̴��� �ٸ� �Լ��� ���ؼ� �����ϰ����Ҷ�. */
	/* �������������� �ٸ��� ������ �� �� */
	pass BackGround
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

	pass Alpha
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		/* �������������� �����Ѵ�. */
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_ALPHA();
	}

	pass UI
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

	pass Effect
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

}