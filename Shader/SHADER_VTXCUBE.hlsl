matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
float4 vColor = float4(1.f, 1.f, 1.f, 1.f);

sampler LinearSampler = sampler_state {
	filter = min_mag_mip_linear;
	AddressU = wrap;
	AddressV = wrap;
};

sampler PointSampler = sampler_state {
	filter = min_mag_mip_Point;
	AddressU = wrap;
	AddressV = wrap;
};

struct VS_IN
{
	float3 vPosition : POSITION;
	float3 vNormal : NORMAL;
};

struct VS_OUT
{
	float4 vPosition : SV_POSITION;
};

VS_OUT vs_main(VS_IN In)
{
	VS_OUT Out = (VS_OUT)0;

	matrix matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);

	return Out;
}

struct PS_IN
{
	float4 vPosition : SV_POSITION;
};

struct PS_OUT
{
	float4 vColor : SV_TARGET0;
};

PS_OUT ps_nonclick(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	Out.vColor = float4(1.f, 1.f, 1.f, 1.f);

	return Out;
}

PS_OUT ps_click(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;
	Out.vColor = float4(0.62f, 0.98f, 0.59f, 1.f);

	return Out;
}

technique11 DefaultTachnique
{
	pass NonClickColor
	{
		VertexShader = compile vs_5_0 vs_main();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 ps_nonclick();
	}

	pass ClickColor
	{
		VertexShader = compile vs_5_0 vs_main();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 ps_click();
	}
}