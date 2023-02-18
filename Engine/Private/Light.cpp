#include "..\Public\Light.h"

CLight::CLight(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: mDevice(pDevice)
	, mContext(pContext)
{
	Safe_AddRef(mDevice);
	Safe_AddRef(mContext);
}

HRESULT CLight::Initialize(const LIGHT_DESC& LightDesc)
{
	mLightDesc = LightDesc;

	return S_OK;
}

CLight* CLight::Create(ID3D11Device* Device, ID3D11DeviceContext* Context, const LIGHT_DESC& LightDesc)
{
	CLight*	pInstance = new CLight(Device, Context);
	if (FAILED(pInstance->Initialize(LightDesc)))
	{
		MSG_BOX("Failed to Created : CLight");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLight::Free()
{
	Safe_Release(mDevice);
	Safe_Release(mContext);
}


