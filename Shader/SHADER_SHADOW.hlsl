#include "SHADER_DEFINES.hpp"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

matrix g_ViewInv, g_ProjInv;

matrix g_LightViewMatrix, g_LightProjMatrix, g_ProjMatrix2;

texture2D g_MainTexture;
texture2D g_DepthTexture;
texture2D g_ShadowDepthTexture;

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

	float4 vWorldPos = mul(In.vPosition, g_WorldMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
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
};

//�ܲ�
PS_OUT ps_shadow(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	vector vMainColor = g_MainTexture.Sample(LinearClampSampler, In.vTexUV);
	vector vDepth = g_DepthTexture.Sample(LinearClampSampler, In.vTexUV);

	//vDepth.x ���������� ����, vDepth.y ������� ����
	float fViewZ = vDepth.y * 1000.f;

	//UV��ǥ�� ���� w�������Ѱ��� ������
	vector vPosition;
	vPosition.x = (In.vTexUV.x * 2.f - 1.f) * fViewZ;
	vPosition.y = (In.vTexUV.y * -2.f + 1.f) * fViewZ;
	vPosition.z = vDepth.x * fViewZ;
	vPosition.w = fViewZ;

	//�� �����̽�
	vPosition = mul(vPosition, g_ProjInv);

	//���� �����̽�
	vPosition = mul(vPosition, g_ViewInv);

	//�ȼ��� ������ǥ�� -> ������ �ٶ� ����� �佺���̽���
	vPosition = mul(vPosition, g_LightViewMatrix);

	//�ȼ��� ������ �ٶ� �佺���̽��� ��ǥ�� ���������̽��� ��ǥ�� ����� W�����⵵ ������ UV��ǥ�� ����
	vector vUVPos = mul(vPosition, g_ProjMatrix);
	float2 vNewUV;
	vNewUV.x = (vUVPos.x / vUVPos.w) * 0.5f + 0.5f;
	vNewUV.y = (vUVPos.y / vUVPos.w) * -0.5f + 0.5f;

	//������ؿ��� �ٶ� ����� �׸��ڸ��� ���̿� ���� ī�޶󿡼� �ٶ� ���̸� ���Ѵ�.
	vector vShadowDepth = g_ShadowDepthTexture.Sample(LinearClampSampler, vNewUV);

	if (vPosition.z - 0.1f > vDepth.g * 1000.f)
		Out.vColor = vector(0.f, 1.0f, 0.0f, 1.f);
	else
		Out.vColor = vMainColor;

	return Out;
}

//����
PS_OUT ps_shadowEx(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	g_MainTexture;
	vector vDepthInfo = g_DepthTexture.Sample(LinearClampSampler, In.vTexUV);
	float fViewZ = vDepthInfo.x * 1000.f;

	vector		vCameraToLight;
	vector		vPosition;
	vPosition.x = (In.vTexUV.x * 2.f - 1.f) * fViewZ;
	vPosition.y = (In.vTexUV.y * -2.f + 1.f) * fViewZ;
	vPosition.z = vDepthInfo.y * fViewZ;
	vPosition.w = fViewZ;

	vPosition = mul(vPosition, g_ProjInv);
	vPosition = mul(vPosition, g_ViewInv);
	vPosition = mul(vPosition, g_LightViewMatrix);

	vector vUVPos = mul(vPosition, g_ProjMatrix);
	float2 vNewUV;

	vNewUV.x = (vUVPos.x / vUVPos.w) * 0.5f + 0.5f;
	vNewUV.y = (vUVPos.y / vUVPos.w) * -0.5f + 0.5f;

	vector vShadowDepthInfo = g_ShadowDepthTexture.Sample(LinearClampSampler, vNewUV);

	if (vPosition.z - 0.1f >= vShadowDepthInfo.r * 1000.f)
		Out.vColor = vector(1.f, 0.f, 0.f, 1.f);

	return Out;
}

technique11 DefaultTechnique
{
	pass Pass0
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Not_ZTest_ZWrite, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 vs_main();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 ps_shadowEx();
	}
}