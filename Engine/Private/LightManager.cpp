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

void CLightManager::SetLightMatrix(_fmatrix LightMatrix, LIGHT_MATRIX eLightMatrix)
{
	switch (eLightMatrix)
	{
	case Engine::CLightManager::LIGHT_VIEW:
		XMStoreFloat4x4(&m_LightMatrix[LIGHT_VIEW], LightMatrix);
		break;
	case Engine::CLightManager::LIGHT_PROJ:
		XMStoreFloat4x4(&m_LightMatrix[LIGHT_PROJ], LightMatrix);
		break;
	default:
		break;
	}
}

_float4x4 CLightManager::GetLightFloat4x4(LIGHT_MATRIX eLightMatrix)
{
	return m_LightMatrix[eLightMatrix];
}

_float4x4 CLightManager::GetLightInverseFloat4x4(LIGHT_MATRIX eLightMatrix)
{
	_float4x4 InvMatrix;
	XMStoreFloat4x4(&InvMatrix, XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_LightMatrix[eLightMatrix])));
	return InvMatrix;
}

void CLightManager::SetLightPosition(_fvector vLightPos)
{
	XMStoreFloat4(&m_vLightPos, vLightPos);
}

void CLightManager::Free()
{
	for (auto& Light : mLights)
		Safe_Release(Light);

	mLights.clear();
}
