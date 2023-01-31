#include "pch.h"
#include "..\Public\Level_Logo.h"

#include "ApplicationManager.h"
#include "GameInstance.h"

#include "Level_Loading.h"

CLevel_Logo::CLevel_Logo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Logo::Initialize()
{
	if (FAILED(Ready_Layer_BackGround(TEXT("LAYER_BACKGROUND"))))
		return E_FAIL;

	return S_OK;
}

void CLevel_Logo::Tick(_double TimeDelta)
{
#ifdef _DEBUG
	CApplicationManager* pApplicationManager = CApplicationManager::GetInstance();
	pApplicationManager->SetTitle(L"LEVEL_LOGO");
#endif

	if (GetKeyState(VK_SPACE) & 0x8000)
	{
		CGameInstance* pGameInstance = CGameInstance::GetInstance();

		if (FAILED(pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_GAMEPLAY))))
			return;
	}
}

HRESULT CLevel_Logo::Ready_Layer_BackGround(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOGO, TEXT("PROTO_OBJ_BACKGROUND"), pLayerTag)))
		return E_FAIL;

	return S_OK;
}

CLevel_Logo * CLevel_Logo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Logo* pInstance = new CLevel_Logo(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		wstring message = L"Failed to Create : CLevel_Logo";
		MESSAGE(message);
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Logo::Free()
{
	__super::Free();
}
