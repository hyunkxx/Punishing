#include "SHADER_DEFINES.hpp"

/* 상수테이블 : 바뀔 수 없는 값. */
/* 클라이언트에서부터 값을 전달받아오기위한 변수 */
matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

/* 클라에서 값을 전달달하지 않았을 경우, 0.f로 사용됨, 전달된 값이 있다면 초기값은 무시된다. */
texture2D g_Texture;

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

	Out.vColor = float4(1.f, 0.f, 0.f, 0.2f) * g_Texture.Sample(LinearSampler, In.vTexUV);

	return Out;
}


technique11 DefaultTechnique
{
	/* 버텍스, 픽셀 셰이더를 다른 함수를 통해서 수행하고자할때. */
	/* 렌더스테이츠를 다르게 쓰고자 할 때 */
	pass BackGround
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

	pass Alpha
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		/* 렌더스테이츠를 정의한다. */
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

		/* 렌더스테이츠를 정의한다. */
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