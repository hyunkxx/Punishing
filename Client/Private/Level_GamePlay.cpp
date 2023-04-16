#include "pch.h"
#include "..\Public\Level_GamePlay.h"

#include "Layer.h"

#include "Level_Loading.h"
#include "ApplicationManager.h"
#include "StageCollisionManager.h"
#include "SkillBallSystem.h"
#include "GameInstance.h"
#include "DynamicCamera.h"
#include "Enemy.h"
#include "Character.h"
#include "EnemySpawner.h"
#include "SwordTrail.h"


#include "PlayerCamera.h"
#include "EnemyHealthBar.h"
#include "Flower.h"
#include "Cloud.h"

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

	if (FAILED(Ready_Layer_Wall(TEXT("layer_wall"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player(TEXT("layer_player"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("layer_camera"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Enemy(TEXT("layer_enemy"), mPlayer)))
		return E_FAIL;

	if (FAILED(Ready_Layer_Spawner(TEXT("layer_spawner"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Effect(TEXT("layer_effect"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_UI(TEXT("layer_ui"))))
		return E_FAIL;

	static_cast<CCharacter*>(mPlayer)->SetHealthUI(static_cast<CEnemyHealthBar*>(m_pHealthBar));

	return S_OK;
}

void CLevel_GamePlay::Tick(_double TimeDelta)
{
#ifdef _DEBUG
	CApplicationManager* pApplicationManager = CApplicationManager::GetInstance(); 
	pApplicationManager->SetTitle(L"LEVEL_GAMEPLAY");
#endif
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if(CApplicationManager::GetInstance()->IsLevelFinish(CApplicationManager::LEVEL::GAMEPLAY))
	{
		if (FAILED(pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_BOSS))))
			return;
	}

	static bool bBgm = false;
	static float fVolum = 0.1f;

	if (!bBgm)
	{
		pGameInstance->PlaySoundEx(L"mainBGM.mp3", SOUND_CHANNEL::SOUND_BGM, CUSTOM_VOLUM, 0.12f);
		bBgm = true;
	}

	////콜리전 생성
	//if (pGameInstance->Input_KeyState_Custom(DIK_INSERT) == KEY_STATE::TAP)
	//{
	//	CStageCollisionManager* pStageManager = CStageCollisionManager::GetInstance();
	//	pStageManager->AddWall(m_pDevice, m_pContext);
	//}

	CSkillBallSystem* pSkillSystem = CSkillBallSystem::GetInstance();
	pSkillSystem->PushReadyTimer(TimeDelta);

	if (pGameInstance->Input_KeyState_Custom(DIK_1) == KEY_STATE::TAP)
		pSkillSystem->PushSkill(m_pDevice, m_pContext, CSkillBase::TYPE::RED);

	else if (pGameInstance->Input_KeyState_Custom(DIK_2) == KEY_STATE::TAP)
		pSkillSystem->PushSkill(m_pDevice, m_pContext, CSkillBase::TYPE::BLUE);

	else if (pGameInstance->Input_KeyState_Custom(DIK_3) == KEY_STATE::TAP)
		pSkillSystem->PushSkill(m_pDevice, m_pContext, CSkillBase::TYPE::YELLOW);

	else if (pGameInstance->Input_KeyState_Custom(DIK_4) == KEY_STATE::TAP)
		pSkillSystem->Clear();

}

HRESULT CLevel_GamePlay::Ready_Light()
{
	CGameInstance* GameInstance = CGameInstance::GetInstance();

	LIGHT_DESC LightDesc;
	ZeroMemory(&LightDesc, sizeof(LIGHT_DESC));
	
	LightDesc.eLightType = LIGHT_DESC::TYPE_DIRECTIONAL;
	LightDesc.vDirection = _float4(-1.f, -0.8f, 1.f, 0.f);
	LightDesc.vPosition = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	
	if (FAILED(GameInstance->AddLight(m_pDevice, m_pContext, LightDesc)))
		return E_FAIL;

	ZeroMemory(&LightDesc, sizeof(LIGHT_DESC));

	LightDesc.eLightType = LIGHT_DESC::TYPE_DIRECTIONAL;
	LightDesc.vDirection = _float4(-0.8f, -1.f, 0.f, 0.f);
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

	if (nullptr == pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, L"proto_obj_city", pLayerTag, L"city"))
		return E_FAIL;

	if (nullptr == pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, L"proto_obj_sky", pLayerTag, L"sky"))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Wall(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

 	vector<CWall::CUBE_DESC> CubeDescs;
	CubeDescs.reserve(20);
	_uint iCount = 0;
	CStageCollisionManager* pStageManager = CStageCollisionManager::GetInstance();
	pStageManager->Clear();
	pStageManager->LoadCollisionData(L"../../CollisionData/GamePlayCollisionData.bin", &CubeDescs, &iCount);
	 for (_uint i = 0; i < iCount; ++i)
	{
		wstring strName = L"wall_" + to_wstring(i);
		if (nullptr == pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, L"proto_obj_wall", pLayerTag, strName.c_str(), &CubeDescs[i]))
			return E_FAIL;
	}

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

	_vector vLightEye = XMVectorSet(0.f, 20.f, -5.f, 1.f);
	_vector vLightAt = XMVectorSet(30.f, 0.f, 30.f, 1.f);
	_matrix vLightViewMatrix = XMMatrixLookAtLH(vLightEye, vLightAt, VECTOR_UP);
	_matrix vLightProjMatrix = XMMatrixPerspectiveFovLH(CameraDesc.fFovy, CameraDesc.fAspect, CameraDesc.fNear, CameraDesc.fFar);

	pGameInstance->SetLightMatrix(vLightViewMatrix, LIGHT_MATRIX::LIGHT_VIEW);
	pGameInstance->SetLightMatrix(vLightProjMatrix, LIGHT_MATRIX::LIGHT_PROJ);
	pGameInstance->SetLightPosition(vLightEye);
	//if (nullptr == pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("proto_obj_dynamic_camera"), L"dynamic_camera", pLayerTag, &CameraDesc))
	//	return E_FAIL;

	assert(mPlayer);
	
	mCamera = static_cast<CPlayerCamera*>(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("proto_obj_player_camera"), pLayerTag, L"player_camera", mPlayer));
	if (nullptr == mCamera)
		return E_FAIL;
	
	static_cast<CCharacter*>(mPlayer)->SetPlayerCamera(mCamera);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Player(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	mPlayer = pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("proto_obj_kamui"), pLayerTag, L"kamui");
	
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Enemy(const _tchar * pLayerTag, CGameObject* pPlayer)
{
	if (nullptr == pPlayer)
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	CGameObject* pGameObject = nullptr;

	if (pGameInstance->Input_KeyState_Custom(DIK_INSERT) == KEY_STATE::TAP)
	{
	}

	if (nullptr == (pGameObject = pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("proto_obj_enemy01"), pLayerTag, L"enemy01", pPlayer)))
		return E_FAIL;
	static_cast<CEnemy*>(pGameObject)->SetupCamera(mCamera);

	if (nullptr == (pGameObject = pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("proto_obj_enemy01"), pLayerTag, L"enemy02", pPlayer)))
		return E_FAIL;
	static_cast<CEnemy*>(pGameObject)->SetupCamera(mCamera);

	if (nullptr == (pGameObject = pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("proto_obj_enemy02"), pLayerTag, L"enemy03", pPlayer)))
		return E_FAIL;
	static_cast<CEnemy*>(pGameObject)->SetupCamera(mCamera);

	if (nullptr == (pGameObject = pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("proto_obj_enemy01"), pLayerTag, L"enemy04", pPlayer)))
		return E_FAIL;
	static_cast<CEnemy*>(pGameObject)->SetupCamera(mCamera);

	if (nullptr == (pGameObject = pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("proto_obj_enemy01"), pLayerTag, L"enemy05", pPlayer)))
		return E_FAIL;
	static_cast<CEnemy*>(pGameObject)->SetupCamera(mCamera);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Spawner(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	CGameObject* pGameObject = nullptr;

	if (nullptr == (pGameObject = pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("proto_obj_spawner"), pLayerTag, L"spawner01")))
		return E_FAIL;
	static_cast<CEnemySpawner*>(pGameObject)->SetupCamera(mCamera);
	
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Effect(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	CGameObject* pGameObject = nullptr;

   	if (nullptr == pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, L"proto_obj_freeze_area", pLayerTag, L"freeze", mPlayer))
		return E_FAIL;

	if (nullptr == pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, L"proto_obj_flower", pLayerTag, L"flower", mPlayer))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_UI(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	CGameObject* pGameObject = nullptr;

	m_pHealthBar = pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, L"proto_obj_enemyhp", pLayerTag, L"enemyhp");
	if(m_pHealthBar == nullptr)
		return E_FAIL;

	//pGameObject = pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, L"proto_obj_damagefont", pLayerTag, L"damagefont");
	//if (pGameObject == nullptr)
	//	return E_FAIL;

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

	CSkillBallSystem* pSkillSystem = CSkillBallSystem::GetInstance();
	pSkillSystem->Clear();

}
