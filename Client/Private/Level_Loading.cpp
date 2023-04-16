#include "pch.h"
#include "..\Public\Level_Loading.h"

#include "ApplicationManager.h"
#include "GameInstance.h"

#include "Loader.h"
#include "Level_Logo.h"
#include "Level_GamePlay.h"
#include "Level_BossRoom.h"
#include "BackGround.h"

CLevel_Loading::CLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Loading::Initialize(LEVEL_ID eNextLevel)
{
	m_eNextLevel = eNextLevel;
	
	m_pLoader = CLoader::Create(m_pDevice, m_pContext, m_eNextLevel);
	if (nullptr == m_pLoader)
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (nullptr == (m_pBackground = pGameInstance->Add_GameObject(LEVEL_LOADING, TEXT("proto_obj_background"), L"Layer_Background", L"background")))
		return E_FAIL;

	return S_OK;
}

void CLevel_Loading::Tick(_double TimeDelta)
{
	if (nullptr == m_pLoader)
		return;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	
	value -= 0.2f * TimeDelta;
	
	if (value <= 0.f)
	{
		pGameInstance->StopSound(SOUND_LOGO);
	}
	else
	{
		pGameInstance->SetSoundVolume(SOUND_LOGO, SOUND_VOLUME::CUSTOM_VOLUM, value);
	}

	//static bool bBgm = false;
	//if (!bBgm)
	//{
	//	pGameInstance->PlaySoundEx(L"mainBGM2.mp3", SOUND_CHANNEL::SOUND_BGM, CUSTOM_VOLUM, 0.15f);
	//	bBgm = true;
	//}

	if (true == m_pLoader->IsFinished())
	{
		static_cast<CBackGround*>(m_pBackground)->SetEnd();
	}
	
	if (static_cast<CBackGround*>(m_pBackground)->IsEnd())
	{
		CLevel*	pLevel = { nullptr };

		switch (m_eNextLevel)
		{
		case LEVEL_LOGO:
			pLevel = CLevel_Logo::Create(m_pDevice, m_pContext);
			break;
		case LEVEL_GAMEPLAY:
 			pLevel = CLevel_GamePlay::Create(m_pDevice, m_pContext);
			break;
		case LEVEL_BOSS:
			pLevel = CLevel_BossRoom::Create(m_pDevice, m_pContext);
			break;
		}

		if (nullptr == pLevel)
			return;

		if (FAILED(pGameInstance->Open_Level(m_eNextLevel, pLevel)))
			return;

		return;
	}

#ifdef _DEBUG
	CApplicationManager* pApplicationManager = CApplicationManager::GetInstance();
	wstring strTitleText = L"LOADING ";
	pApplicationManager->SetTitle(strTitleText.append(m_pLoader->GetLoadingStateText()));
#endif
}

CLevel_Loading* CLevel_Loading::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL_ID eNextLevel)
{
	CLevel_Loading* pInstance = new CLevel_Loading(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevel)))
	{
		wstring message = L"Failed to Create : CLevel_Loading";
		MESSAGE(message);
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Loading::Free()
{
	__super::Free();
	Safe_Release(m_pLoader);
}
