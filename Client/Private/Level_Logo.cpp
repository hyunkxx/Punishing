#include "pch.h"
#include "..\Public\Level_Logo.h"

#include "ApplicationManager.h"
#include "GameInstance.h"

#include "Level_Loading.h"
#include "Robby.h"

CLevel_Logo::CLevel_Logo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Logo::Initialize()
{
	if (FAILED(Ready_Layer_BackGround(TEXT("layer_background"))))
		return E_FAIL;

	return S_OK;
}

void CLevel_Logo::Tick(_double TimeDelta)
{

#ifdef _DEBUG
	CApplicationManager* pApplicationManager = CApplicationManager::GetInstance();
	pApplicationManager->SetTitle(L"LEVEL_LOGO");
#endif
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	static bool bBgm = false;
	if (!bBgm)
	{
		pGameInstance->PlaySoundEx(L"LogoBgm.mp3", SOUND_CHANNEL::SOUND_LOGO, CUSTOM_VOLUM, 0.3f);
		bBgm = true;
	}

	if (pGameInstance->Input_KeyState_Custom(DIK_ESCAPE) == KEY_STATE::TAP)
	{
		if (FAILED(pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_GAMEPLAY))))
			return;
	}
}

HRESULT CLevel_Logo::Ready_Layer_BackGround(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (nullptr == (m_pRobby = (CRobby*)pGameInstance->Add_GameObject(LEVEL_LOGO, TEXT("proto_obj_robby"), L"robby", pLayerTag)))
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
