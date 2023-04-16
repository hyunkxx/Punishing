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

//쌤꺼
PS_OUT ps_shadow(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	vector vMainColor = g_MainTexture.Sample(LinearClampSampler, In.vTexUV);
	vector vDepth = g_DepthTexture.Sample(LinearClampSampler, In.vTexUV);

	//vDepth.x 투영공간의 뎁스, vDepth.y 뷰공간의 뎁스
	float fViewZ = vDepth.y * 1000.f;

	//UV좌표를 투영 w나누기한것을 돌린것
	vector vPosition;
	vPosition.x = (In.vTexUV.x * 2.f - 1.f) * fViewZ;
	vPosition.y = (In.vTexUV.y * -2.f + 1.f) * fViewZ;
	vPosition.z = vDepth.x * fViewZ;
	vPosition.w = fViewZ;

	//뷰 스페이스
	vPosition = mul(vPosition, g_ProjInv);

	//월드 스페이스
	vPosition = mul(vPosition, g_ViewInv);

	//픽셀의 월드좌표를 -> 조명에서 바라본 장면의 뷰스페이스로
	vPosition = mul(vPosition, g_LightViewMatrix);

	//픽셀의 조명에서 바라본 뷰스페이스상 좌표를 투영스페이스의 좌표료 만들고 W나누기도 수행후 UV좌표로 만듬
	vector vUVPos = mul(vPosition, g_ProjMatrix);
	float2 vNewUV;
	vNewUV.x = (vUVPos.x / vUVPos.w) * 0.5f + 0.5f;
	vNewUV.y = (vUVPos.y / vUVPos.w) * -0.5f + 0.5f;

	//조명기준에서 바라본 장면의 그림자맵의 깊이와 현재 카메라에서 바라본 깊이를 비교한다.
	vector vShadowDepth = g_ShadowDepthTexture.Sample(LinearClampSampler, vNewUV);

	if (vPosition.z - 0.1f > vDepth.g * 1000.f)
		Out.vColor = vector(0.f, 1.0f, 0.0f, 1.f);
	else
		Out.vColor = vMainColor;

	return Out;
}

//내꺼
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