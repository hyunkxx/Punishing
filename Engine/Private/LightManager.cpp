#include "..\Public\LightManager.h"
#include "Light.h"

IMPLEMENT_SINGLETON(CLightManager)

HRESULT CLightManager::AddLight(ID3D11Device* Device, ID3D11DeviceContext* Context, const LIGHT_DESC& LightDesc)
{
	CLight* Light = CLight::Create(Device, Context, LightDesc);
	if (nullptr == Light)
		return E_FAIL;

	mLights.push_back(Light);

	return S_OK;
}

const LIGHT_DESC* CLightManager::GetLightDesc(_uint Index)
{
	auto iter = mLights.begin();

	for (_uint i = 0; i < Index; ++i)
		++iter;

	return (*iter)->GetLightDesc();
}

void CLightManager::Free()
{
	for (auto& Light : mLights)
		Safe_Release(Light);

	mLights.clear();
}
