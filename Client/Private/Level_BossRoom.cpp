#include "pch.h"
#include "..\Public\Level_BossRoom.h"

#include "ApplicationManager.h"
#include "GameInstance.h"
#include "DynamicCamera.h"
#include "PlayerCamera.h"
#include "Enemy.h"

#include "Boss.h"
#include "Character.h"
#include "StageCollisionManager.h"
#include "SkillBallSystem.h"
#include "EnemyHealthBar.h"

CLevel_BossRoom::CLevel_BossRoom(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_BossRoom::Initialize()
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

	if (FAILED(Ready_Layer_Enemy(TEXT("layer_enemy"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Effect(TEXT("layer_effect"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_UI(TEXT("layer_ui"))))
		return E_FAIL;

	static_cast<CCharacter*>(mPlayer)->SetHealthUI(static_cast<CEnemyHealthBar*>(m_pHealthBar));

	return S_OK;
}

void CLevel_BossRoom::Tick(_double TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	CSkillBallSystem* pSkillSystem = CSkillBallSystem::GetInstance();
	pSkillSystem->PushReadyTimer(TimeDelta);

	////콜리전 생성
	//if (pGameInstance->Input_KeyState_Custom(DIK_INSERT) == KEY_STATE::TAP)
	//{
	//	CStageCollisionManager* pStageManager = CStageCollisionManager::GetInstance();
	//	pStageManager->AddWall(m_pDevice, m_pContext);
	//}

	if (pGameInstance->Input_KeyState_Custom(DIK_1) == KEY_STATE::TAP)
		pSkillSystem->PushSkill(m_pDevice, m_pContext, CSkillBase::TYPE::RED);

	else if (pGameInstance->Input_KeyState_Custom(DIK_2) == KEY_STATE::TAP)
		pSkillSystem->PushSkill(m_pDevice, m_pContext, CSkillBase::TYPE::BLUE);

	else if (pGameInstance->Input_KeyState_Custom(DIK_3) == KEY_STATE::TAP)
		pSkillSystem->PushSkill(m_pDevice, m_pContext, CSkillBase::TYPE::YELLOW);

	else if (pGameInstance->Input_KeyState_Custom(DIK_4) == KEY_STATE::TAP)
		pSkillSystem->Clear();

#ifdef _DEBUG
	CApplicationManager* pApplicationManager = CApplicationManager::GetInstance(); 
	pApplicationManager->SetTitle(L"LEVEL_BOSSROOM");
#endif
}

HRESULT CLevel_BossRoom::Ready_Light()
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

HRESULT CLevel_BossRoom::Ready_Layer_BackGround(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (nullptr == pGameInstance->Add_GameObject(LEVEL_BOSS, L"proto_obj_bossroom", L"bossroom", pLayerTag))
		return E_FAIL;

	if (nullptr == pGameInstance->Add_GameObject(LEVEL_BOSS, L"proto_obj_sky", L"sky", pLayerTag))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_BossRoom::Ready_Layer_Wall(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	vector<CWall::CUBE_DESC> CubeDescs;
	CubeDescs.reserve(20);
	_uint iCount = 0;

	CStageCollisionManager* pStageManager = CStageCollisionManager::GetInstance();
	pStageManager->Clear();
	pStageManager->LoadCollisionData(L"../../CollisionData/BossRoomCollisionData.bin", &CubeDescs, &iCount);
	 for (_uint i = 0; i < iCount; ++i)
	{
		wstring strName = L"wall_2" + to_wstring(i);
		if (nullptr == pGameInstance->Add_GameObject(LEVEL_BOSS, L"proto_obj_wall", pLayerTag, strName.c_str(), &CubeDescs[i]))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_BossRoom::Ready_Layer_Camera(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	CCamera::CAMERA_DESC CameraDesc;
	ZeroMemory(&CameraDesc, sizeof(CCamera::CAMERA_DESC));

	CameraDesc.TransformDesc.fMoveSpeed = 20.f;
	CameraDesc.TransformDesc.fRotationSpeed = XMConvertToRadians(60.f);

	CameraDesc.vEye = _float3(0.f, 1.f, -5.f);
	CameraDesc.vAt = _float3(0.f, 0.f, 0.f);
	CameraDesc.vAxisY = _float3(0.f, 1.f, 0.f);

	CameraDesc.fFovy = XMConvertToRadians(45.f);
	CameraDesc.fAspect = g_iWinSizeX / (_float)g_iWinSizeY;
	CameraDesc.fNear = 0.1f;
	CameraDesc.fFar = 1000.f;

	//if (nullptr == pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("proto_obj_dynamic_camera"), L"dynamic_camera", pLayerTag, &CameraDesc))
	//	return E_FAIL;

	assert(mPlayer);

	mCamera = static_cast<CPlayerCamera*>(pGameInstance->Add_GameObject(LEVEL_BOSS, TEXT("proto_obj_player_camera"), pLayerTag, L"player_camera", mPlayer));
	if (nullptr == mCamera)
		return E_FAIL;
	
	static_cast<CCharacter*>(mPlayer)->SetPlayerCamera(mCamera);
	return S_OK;
}

HRESULT CLevel_BossRoom::Ready_Layer_Player(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	mPlayer = pGameInstance->Add_GameObject(LEVEL_BOSS, TEXT("proto_obj_kamui"), pLayerTag, L"kamui");

	return S_OK;
}

HRESULT CLevel_BossRoom::Ready_Layer_Enemy(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	CGameObject* pGameObject = nullptr;

	if (nullptr == (pGameObject =  pGameInstance->Add_GameObject(LEVEL_BOSS, TEXT("proto_obj_boss"), L"boss", pLayerTag, mPlayer)))
		return E_FAIL;
	static_cast<CBoss*>(pGameObject)->SetupCamera(mCamera);

	return S_OK;
}

HRESULT CLevel_BossRoom::Ready_Layer_Effect(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	CGameObject* pGameObject = nullptr;

	if (nullptr == pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, L"proto_obj_freeze_area", pLayerTag, L"freeze", mPlayer))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_BossRoom::Ready_Layer_UI(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	CGameObject* pGameObject = nullptr;

	m_pHealthBar = pGameInstance->Add_GameObject(LEVEL_BOSS, L"proto_obj_enemyhp", pLayerTag, L"enemyhp");
	if (m_pHealthBar == nullptr)
		return E_FAIL;

	return S_OK;
}

CLevel_BossRoom* CLevel_BossRoom::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CLevel_BossRoom* pInstance = new CLevel_BossRoom(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		wstring message = L"Failed to Create : CLevel_BossRoom";
		MESSAGE(message);
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_BossRoom::Free()
{
	__super::Free();
}
