/* For.SamplerState */
sampler LinearSampler = sampler_state{
	filter = min_mag_mip_linear;
AddressU = wrap;
AddressV = wrap;
};

sampler LinearClampSampler = sampler_state{
	filter = min_mag_mip_linear;
AddressU = clamp;
AddressV = clamp;
};

sampler PointSampler = sampler_state{
	filter = min_mag_mip_Point;
AddressU = wrap;
AddressV = wrap;
};


/* For.RasterizerState */
RasterizerState RS_Default
{
	FillMode = Solid;
CullMode = None;
FrontCounterClockwise = false;
};

RasterizerState	RS_Sky
{
	FillMode = Solid;
CullMode = Front;
FrontCounterClockwise = false;
};


RasterizerState RS_Wireframe
{
	FillMode = Wireframe;
CullMode = Back;
FrontCounterClockwise = false;
};

/* For.DepthStencilState */
DepthStencilState DS_Default
{
	DepthEnable = true;
	DepthWriteMask = All;
	DepthFunc = less_equal;
};

DepthStencilState DS_Not_ZTest_ZWrite
{
	DepthEnable = false;
	DepthWriteMask = Zero;
};

DepthStencilState DS_ZTest_NoZWrite
{
	DepthEnable = true;
	DepthWriteMask = Zero;
};

/* For.BlendState */
BlendState BS_Default
{
	BlendEnable[0] = false;
};

BlendState BS_AlphaBlend
{
	/* 인덱스 지정 필수 */
	BlendEnable[0] = true;
	BlendEnable[1] = true;

/* 인덱스 지정 선택 */
BlendOp = add;
	SrcBlend = src_alpha;
	DestBlend = Inv_src_alpha;
};


