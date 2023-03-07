#include "pch.h"
#include "..\Public\Level_GamePlay.h"

#include "Level_Loading.h"
#include "ApplicationManager.h"
#include "GameInstance.h"
#include "DynamicCamera.h"
#include "Enemy.h"

CLevel_GamePlay::CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_GamePlay::Initialize()
{
	if (FAILED(Ready_Light()))
		return E_FAIL;

	if(FAILED(Ready_Layer_BackGround(TEXT("layer_background"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player(TEXT("layer_player"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Enemy(TEXT("layer_enemy"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("layer_camera"))))
		return E_FAIL;

	return S_OK;
}

void CLevel_GamePlay::Tick(_double TimeDelta)
{
#ifdef _DEBUG
	CApplicationManager* pApplicationManager = CApplicationManager::GetInstance(); 
	pApplicationManager->SetTitle(L"LEVEL_GAMEPLAY");
#endif
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (pGameInstance->Input_KeyState_Custom(DIK_ESCAPE) == KEY_STATE::TAP)
	{
		if (FAILED(pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_BOSS))))
			return;
	}
}

HRESULT CLevel_GamePlay::Ready_Light()
{
	CGameInstance* GameInstance = CGameInstance::GetInstance();
	
	LIGHT_DESC LightDesc;
	ZeroMemory(&LightDesc, sizeof(LIGHT_DESC));
	
	LightDesc.eLightType = LIGHT_DESC::TYPE_DIRECTIONAL;
	LightDesc.vDirection = _float4(0.f, -1.f, 1.f, 0.f);
	LightDesc.vPosition = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	if (FAILED(GameInstance->AddLight(m_pDevice, m_pContext, LightDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_BackGround(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (nullptr == pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, L"proto_obj_city", L"city", pLayerTag))
		return E_FAIL;

	if (nullptr == pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, L"proto_obj_sky", L"sky", pLayerTag))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Camera(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	CCamera::CAMERA_DESC CameraDesc;
	ZeroMemory(&CameraDesc, sizeof(CCamera::CAMERA_DESC));

	CameraDesc.TransformDesc.fMoveSpeed = 20.f;
	CameraDesc.TransformDesc.fRotationSpeed = XMConvertToRadians(60.f);

	CameraDesc.vEye = _float3(0.f, 10.f, -5.f);
	CameraDesc.vAt = _float3(0.f, 0.f, 0.f);
	CameraDesc.vAxisY = _float3(0.f, 1.f, 0.f);

	CameraDesc.fFovy = XMConvertToRadians(45.f);
	CameraDesc.fAspect = g_iWinSizeX / (_float)g_iWinSizeY;
	CameraDesc.fNear = 0.1f;
	CameraDesc.fFar = 1000.f;

	//if (nullptr == pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("proto_obj_dynamic_camera"), L"dynamic_camera", pLayerTag, &CameraDesc))
	//	return E_FAIL;

	assert(mPlayer);

	if (nullptr == pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("proto_obj_player_camera"), L"player_camera", pLayerTag, mPlayer))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Player(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	mPlayer = pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("proto_obj_kamui"), L"kamui", pLayerTag);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Enemy(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	CGameObject* pGameObject = nullptr;

	if (pGameObject == pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("proto_obj_enemy01"), L"enemy01", pLayerTag, mPlayer))
		return E_FAIL;

	if (pGameObject == pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("proto_obj_enemy01"), L"enemy02", pLayerTag, mPlayer))
		return E_FAIL;

	if (pGameObject == pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("proto_obj_enemy01"), L"enemy03", pLayerTag, mPlayer))
		return E_FAIL;

	if (pGameObject == pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("proto_obj_enemy01"), L"enemy04", pLayerTag, mPlayer))
		return E_FAIL;

	if (pGameObject == pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("proto_obj_enemy02"), L"enemy05", pLayerTag, mPlayer))
		return E_FAIL;

	return S_OK;
}

CLevel_GamePlay* CLevel_GamePlay::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CLevel_GamePlay* pInstance = new CLevel_GamePlay(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		wstring message = L"Failed to Create : CLevel_GamePlay";
		MESSAGE(message);
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_GamePlay::Free()
{
	__super::Free();
}
