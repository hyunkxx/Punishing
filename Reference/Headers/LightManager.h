#pragma once

#include "Base.h"

BEGIN(Engine)

class CLightManager final : public CBase
{
	DECLARE_SINGLETON(CLightManager)
private:
	CLightManager() = default;
	virtual ~CLightManager() = default;

public:
	HRESULT AddLight(ID3D11Device* Device, ID3D11DeviceContext* Context, const LIGHT_DESC& LightDesc);
	const LIGHT_DESC* GetLightDesc(_uint Index);

public:
	virtual void Free() override;

private:
	list<class CLight*> mLights;
	typedef list<class CLight*> LIGHTS;

};

END