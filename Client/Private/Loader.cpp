#include "pch.h"
#include "..\Public\Loader.h"
#include "GameInstance.h"

#include "PlayerCamera.h"
#include "BackGround.h"
#include "Terrain.h"
#include "City.h"
#include "Skybox.h"
#include "Character.h"
#include "Enemy.h"
#include "Weapon.h"

CLoader::CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

unsigned int APIENTRY ThreadEntry(void* pArg)
{
	CLoader* pLoader = ((CLoader*)pArg);

	EnterCriticalSection(pLoader->Get_CriticalSectionPtr());

	switch (pLoader->Get_NextLevel())
	{
	case LEVEL_LOGO:
		pLoader->Load_Level_Logo();
		break;
	case LEVEL_GAMEPLAY:
		pLoader->Load_Level_GamePlay();
		break;
	}

	LeaveCriticalSection(pLoader->Get_CriticalSectionPtr());
	return 0;
}

HRESULT CLoader::Initialize(LEVEL_ID eNextLevel)
{
	CoInitializeEx(nullptr, 0);
	 
	m_eNextLevel = eNextLevel;
	InitializeCriticalSection(&m_hCriticalSection);
	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, ThreadEntry, this, 0, nullptr);

	if (0 == m_hThread)
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Load_Level_Logo()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	//Logo Component
#pragma region COMPONENTS
	
	m_szLoadingStateText = L"텍스쳐를 로딩중입니다.";

	m_szLoadingStateText = L"정점버퍼를 로딩중입니다.";

	m_szLoadingStateText = L"모델를 로딩중입니다.";
	
	m_szLoadingStateText = L"셰이더를 로딩중입니다.";

#pragma endregion

	//Logo GameObject
#pragma region GAMEOBJECTS
	
	m_szLoadingStateText = L"객체 원형을 준비중";

#pragma endregion

	m_szLoadingStateText = L"Load Completed";
	m_isFinish = true;
	return S_OK;
}

HRESULT CLoader::Load_Level_GamePlay()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	//GamePlay Component
#pragma region COMPONENTS
	m_szLoadingStateText = L"Texture..";

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("proto_com_texture_terrain"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Terrain/Tile0.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("proto_com_vibuffer_terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Terrain/Height.bmp")))))
		return E_FAIL;

	m_szLoadingStateText = L"Buffer..";


	m_szLoadingStateText = L"Collision..";

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("proto_com_sphere_collider"),
		CSphereCollider::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("proto_com_obb_collider"),
		COBBCollider::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	m_szLoadingStateText = L"Model..";

	_matrix	cityMatrix = XMMatrixIdentity();
	cityMatrix = XMMatrixRotationY(XMConvertToRadians(180.f));

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"proto_com_model_city",
		CModel::Create(m_pDevice, m_pContext, CModel::MESH_TYPE::STATIC_MESH, "../../Resource/Mesh/Level/Load/Load.fbx", cityMatrix))))
		return E_FAIL;
	
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"proto_com_model_sky",
		CModel::Create(m_pDevice, m_pContext, CModel::MESH_TYPE::STATIC_MESH, "../../Resource/Mesh/Level/Load/Sky.fbx", cityMatrix))))
		return E_FAIL;

	// 무기는 정방향
	_matrix	LocalMatrix = XMMatrixIdentity();

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"proto_com_model_kamui_weapon",
		CModel::Create(m_pDevice, m_pContext, CModel::MESH_TYPE::STATIC_MESH, "../../Resource/Mesh/Character/Kamui/Weapon/Weapon.fbx", LocalMatrix))))
		return E_FAIL;

	// 플레이어 모델부터 로컬 메트릭스 설정 (Y축 회전 180도)
	LocalMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"proto_com_model_kamui",
		CModel::Create(m_pDevice, m_pContext, CModel::MESH_TYPE::SKELETAL_MESH, "../../Resource/Mesh/Character/Kamui/Body/Kamui.fbx", LocalMatrix, CCharacter::CLIP_END))))
		return E_FAIL;

	// Enemy01
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"proto_com_model_enemy01",
		CModel::Create(m_pDevice, m_pContext, CModel::MESH_TYPE::SKELETAL_MESH, "../../Resource/Mesh/Enemy/01/Enemy01.fbx", LocalMatrix, CCharacter::CLIP_END))))
		return E_FAIL;

	// Enemy02
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"proto_com_model_enemy02",
		CModel::Create(m_pDevice, m_pContext, CModel::MESH_TYPE::SKELETAL_MESH, "../../Resource/Mesh/Enemy/02/Enemy02.fbx", LocalMatrix, CCharacter::CLIP_END))))
		return E_FAIL;

	m_szLoadingStateText = L"Shader..";

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("proto_com_shader_vtxnortex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../Shader/SHADER_VTXNORTEX.hlsl"), VTXNORTEX_DECLARATION::Elements, VTXNORTEX_DECLARATION::ElementCount))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("proto_com_shader_vtxanimmodel"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../Shader/SHADER_VTXANIMMODEL.hlsl"), VTXANIMMODEL_DECLARATION::Elements, VTXANIMMODEL_DECLARATION::ElementCount))))
		return E_FAIL;

#pragma endregion

	//GamePlay GameObject
#pragma region GAMEOBJECTS

	if (FAILED(pGameInstance->Add_Prototype(TEXT("proto_obj_terrain"), CTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("proto_obj_city"), CCity::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("proto_obj_sky"), CSkybox::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("proto_obj_kamui"), CCharacter::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("proto_obj_enemy01"), CEnemy::Create(m_pDevice, m_pContext, CEnemy::TYPE::HUMANOID))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("proto_obj_enemy02"), CEnemy::Create(m_pDevice, m_pContext, CEnemy::TYPE::ANIMAL))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("proto_obj_kamui_weapon"), CWeapon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("proto_obj_player_camera"), CPlayerCamera::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma endregion

	m_szLoadingStateText = L"Load Completed";
	m_isFinish = true;
	return S_OK;
}

CLoader* CLoader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL_ID eNextLevel)
{
	CLoader* pInstance = new CLoader(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevel)))
	{
		wstring message = L"Failed to Create : CLoader";
		MESSAGE(message);
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoader::Free()
{
	WaitForSingleObject(m_hThread, INFINITE);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	DeleteCriticalSection(&m_hCriticalSection);
	CloseHandle(m_hThread);
}
