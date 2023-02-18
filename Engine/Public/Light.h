#pragma once

#include "Base.h"

BEGIN(Engine)

class CLight final : public CBase
{
private:
	CLight(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLight() = default;

public:
	const LIGHT_DESC* GetLightDesc() const { return &mLightDesc; }

public:
	HRESULT Initialize(const LIGHT_DESC& LightDesc);

public:
	static CLight* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const LIGHT_DESC& LightDesc);
	virtual void Free() override;

private:
	ID3D11Device* mDevice;
	ID3D11DeviceContext* mContext;
	LIGHT_DESC mLightDesc;

};

END