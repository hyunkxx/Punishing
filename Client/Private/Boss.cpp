#include "pch.h"
#include "..\Public\Boss.h"

#include "ApplicationManager.h"

#include "PipeLine.h"
#include "GameInstance.h"

#include "Bone.h"
#include "Character.h"
#include "PlayerCamera.h"

#include "Thorn.h"

#define LINE_ONE 0
#define LINE_TWO 1
#define LINE_TRE 2

CBoss::CBoss(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEnemy(pDevice, pContext)
{
}

CBoss::CBoss(const CBoss & rhs)
	: CEnemy(rhs)
{
}

HRESULT CBoss::Initialize_Prototype()
{
	ZeroMemory(&m_State, sizeof(ENEMY_STATE));
	m_State.fMaxHp = 20000.f;
	m_State.fCurHp = 20000.f;
	
	return S_OK;
}

HRESULT CBoss::Initialize(void * pArg)
{
	m_pAppManager = CApplicationManager::GetInstance();

	AddComponents();

	if (nullptr != pArg)
	{
		m_pPlayer = (CCharacter*)pArg;
		m_pPlayerTransform = static_cast<CTransform*>(m_pPlayer->Find_Component(L"com_transform"));
	}

	bone = model->GetBonePtr("Bip001Pelvis");
	XMStoreFloat4x4(&m_RootBoneMatrix, XMLoadFloat4x4(&bone->GetOffSetMatrix()) * XMLoadFloat4x4(&bone->GetCombinedMatrix()) * XMLoadFloat4x4(&model->GetLocalMatrix()) * XMLoadFloat4x4(&transform->Get_WorldMatrix()));
	
	transform->Set_Scale(_float3(1.5f, 1.5f, 1.5f));
	transform->Set_State(CTransform::STATE_POSITION, XMVectorSet(1.3f, 0.f, 21.f, 1.f));
	transform->SetRotation(VECTOR_UP, XMConvertToRadians(180.f));

	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	CTransform::TRANSFORM_DESC TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORM_DESC));
	//콜라이더 트렌스폼 설정
	//LineAttack
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("proto_com_transform"), TEXT("com_transform_0"), (CComponent**)&m_pColliderTransform[0], &TransformDesc)))
		return E_FAIL;
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("proto_com_transform"), TEXT("com_transform_1"), (CComponent**)&m_pColliderTransform[1], &TransformDesc)))
		return E_FAIL;
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("proto_com_transform"), TEXT("com_transform_2"), (CComponent**)&m_pColliderTransform[2], &TransformDesc)))
		return E_FAIL;
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("proto_com_transform"), TEXT("com_transform_3"), (CComponent**)&m_pColliderTransform[3], &TransformDesc)))
		return E_FAIL;
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("proto_com_transform"), TEXT("com_transform_4"), (CComponent**)&m_pColliderTransform[4], &TransformDesc)))
		return E_FAIL;
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("proto_com_transform"), TEXT("com_transform_5"), (CComponent**)&m_pColliderTransform[5], &TransformDesc)))
		return E_FAIL;

	//CloseAttack
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("proto_com_transform"), TEXT("com_transform_6"), (CComponent**)&m_pColliderTransform[6], &TransformDesc)))
		return E_FAIL;
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("proto_com_transform"), TEXT("com_transform_7"), (CComponent**)&m_pColliderTransform[7], &TransformDesc)))
		return E_FAIL;
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("proto_com_transform"), TEXT("com_transform_8"), (CComponent**)&m_pColliderTransform[8], &TransformDesc)))
		return E_FAIL;

	//Colliders
	CCollider::COLLIDER_DESC collDesc;
	collDesc.owner = this;
	collDesc.vCenter = _float3(0.f, 1.f, 0.f);
	collDesc.vExtents = _float3(1.8f, 1.8f, 1.8f);
	collDesc.vRotation = _float3(0.f, 0.f, 0.f);
	if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("proto_com_sphere_collider"), TEXT("com_collider0"), (CComponent**)&m_pColliderLine[0], &collDesc)))
		return E_FAIL;
	if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("proto_com_sphere_collider"), TEXT("com_collider1"), (CComponent**)&m_pColliderLine[1], &collDesc)))
		return E_FAIL;
	if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("proto_com_sphere_collider"), TEXT("com_collider2"), (CComponent**)&m_pColliderLine[2], &collDesc)))
		return E_FAIL;
	if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("proto_com_sphere_collider"), TEXT("com_collider3"), (CComponent**)&m_pColliderLine[3], &collDesc)))
		return E_FAIL;
	if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("proto_com_sphere_collider"), TEXT("com_collider4"), (CComponent**)&m_pColliderLine[4], &collDesc)))
		return E_FAIL;
	if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("proto_com_sphere_collider"), TEXT("com_collider5"), (CComponent**)&m_pColliderLine[5], &collDesc)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("proto_com_sphere_collider"), TEXT("com_collider6"), (CComponent**)&m_pCloseAttack[0], &collDesc)))
		return E_FAIL;
	m_pCloseAttack[0]->SetActive(false);
	if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("proto_com_sphere_collider"), TEXT("com_collider7"), (CComponent**)&m_pCloseAttack[1], &collDesc)))
		return E_FAIL;
	m_pCloseAttack[1]->SetActive(false);
	if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("proto_com_sphere_collider"), TEXT("com_collider8"), (CComponent**)&m_pCloseAttack[2], &collDesc)))
		return E_FAIL;
	m_pCloseAttack[2]->SetActive(false);

	for (int j = 0; j < 6; j++)
	{
		// 3방향 가시 라인
		for (int i = 0; i < 25; ++i)
		{
			_tchar pName1[MAX_PATH] = L"";
			_tchar pName2[MAX_PATH] = L"";
			wsprintfW(pName1, L"thorn1_%d_%d", i, j);
			if (nullptr == (m_pThorn1[i][j] = static_cast<CThorn*>(pGameInstance->Add_GameObject(LEVEL_BOSS, L"proto_obj_thorn", L"layer_effect", pName1))))
				return E_FAIL;

			wsprintfW(pName2, L"thorn2_%d_%d", i, j);
			if (nullptr == (m_pThorn2[i][j] = static_cast<CThorn*>(pGameInstance->Add_GameObject(LEVEL_BOSS, L"proto_obj_thorn", L"layer_effect", pName2))))
				return E_FAIL;

			m_pThorn1[i][j]->SetType(CThorn::TYPE::THORN);
			m_pThorn2[i][j]->SetType(CThorn::TYPE::THORN);
		}
	}

	//가시 미사일 1
	for (int i = 0; i < 3; ++i)
	{
		_tchar pName[MAX_PATH] = L"";
		wsprintfW(pName, L"thornMissileLeft_%d", i);
		if (nullptr == (m_pThornMissileLeft[i] = static_cast<CThorn*>(pGameInstance->Add_GameObject(LEVEL_BOSS, L"proto_obj_thorn", L"layer_effect", pName))))
			return E_FAIL;

		m_pThornMissileLeft[i]->SetType(CThorn::TYPE::MISSILE);
	}
	for (int i = 0; i < 3; ++i)
	{
		_tchar pName[MAX_PATH] = L"";
		wsprintfW(pName, L"thornMissileMiddle_%d", i);
		if (nullptr == (m_pThornMissileMiddle[i] = static_cast<CThorn*>(pGameInstance->Add_GameObject(LEVEL_BOSS, L"proto_obj_thorn", L"layer_effect", pName))))
			return E_FAIL;

		m_pThornMissileMiddle[i]->SetType(CThorn::TYPE::MISSILE);
	}
	for (int i = 0; i < 3; ++i)
	{
		_tchar pName[MAX_PATH] = L"";
		wsprintfW(pName, L"thornMissileRight_%d", i);
		if (nullptr == (m_pThornMissileRight[i] = static_cast<CThorn*>(pGameInstance->Add_GameObject(LEVEL_BOSS, L"proto_obj_thorn", L"layer_effect", pName))))
			return E_FAIL;

		m_pThornMissileRight[i]->SetType(CThorn::TYPE::MISSILE);
	}

	//근접공격용 가시
	for (int i = 0; i < 9; ++i)
	{
		_tchar pName[MAX_PATH] = L"";
		wsprintfW(pName, L"thornClose%d", i);
		if (nullptr == (m_pThornClose[i] = static_cast<CThorn*>(pGameInstance->Add_GameObject(LEVEL_BOSS, L"proto_obj_thorn", L"layer_effect", pName))))
			return E_FAIL;

		m_pThornClose[i]->SetType(CThorn::TYPE::THORN);
	}


	//2페이즈 기본 근접공격
	for (int i = 0; i < 9; ++i)
	{
		_tchar pName[MAX_PATH] = L"";
		wsprintfW(pName, L"thornCloseRight%d", i);
		if (nullptr == (m_pThornCloseRight[i] = static_cast<CThorn*>(pGameInstance->Add_GameObject(LEVEL_BOSS, L"proto_obj_thorn", L"layer_effect", pName))))
			return E_FAIL;

		m_pThornCloseRight[i]->SetType(CThorn::TYPE::THORN);
	}

	for (int i = 0; i < 9; ++i)
	{
		_tchar pName[MAX_PATH] = L"";
		wsprintfW(pName, L"thornCloseLeft%d", i);
		if (nullptr == (m_pThornCloseLeft[i] = static_cast<CThorn*>(pGameInstance->Add_GameObject(LEVEL_BOSS, L"proto_obj_thorn", L"layer_effect", pName))))
			return E_FAIL;

		m_pThornCloseLeft[i]->SetType(CThorn::TYPE::THORN);
	}

	for (int i = 0; i < 9; ++i)
	{
		_tchar pName[MAX_PATH] = L"";
		wsprintfW(pName, L"thornCloseFront%d", i);
		if (nullptr == (m_pThornCloseFront[i] = static_cast<CThorn*>(pGameInstance->Add_GameObject(LEVEL_BOSS, L"proto_obj_thorn", L"layer_effect", pName))))
			return E_FAIL;

		m_pThornCloseFront[i]->SetType(CThorn::TYPE::THORN);
	}

	for (int i = 0; i < 9; ++i)
	{
		_tchar pName[MAX_PATH] = L"";
		wsprintfW(pName, L"thornCloseLeftFront%d", i);
		if (nullptr == (m_pThornCloseLeftFront[i] = static_cast<CThorn*>(pGameInstance->Add_GameObject(LEVEL_BOSS, L"proto_obj_thorn", L"layer_effect", pName))))
			return E_FAIL;

		m_pThornCloseLeftFront[i]->SetType(CThorn::TYPE::THORN);
	}

	for (int i = 0; i < 9; ++i)
	{
		_tchar pName[MAX_PATH] = L"";
		wsprintfW(pName, L"thornCloseRightFront%d", i);
		if (nullptr == (m_pThornCloseRightFront[i] = static_cast<CThorn*>(pGameInstance->Add_GameObject(LEVEL_BOSS, L"proto_obj_thorn", L"layer_effect", pName))))
			return E_FAIL;

		m_pThornCloseRightFront[i]->SetType(CThorn::TYPE::THORN);
	}

	return S_OK;
}

void CBoss::Tick(_double TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	m_bAlpha = false;
	TimeDelta = Freeze(TimeDelta);
	
	if (!m_bDie)
	{
		if (m_State.fCurHp <= 0.f)
		{
			if (model->AnimationIsFinishEx())
			{
				m_bDie = true;
			}
		}
		//LookTarget(TimeDelta, 0.1f);

		//테스트용 코드
		if (pGameInstance->Input_KeyState_Custom(DIK_T) == KEY_STATE::TAP)
			m_State.fCurHp = 900;

		if (pGameInstance->Input_KeyState_Custom(DIK_Y) == KEY_STATE::TAP)
			m_bCloseAttackExStart = true;

		if (pGameInstance->Input_KeyState_Custom(DIK_PGUP) == KEY_STATE::TAP)
			iAnim++;
		if (pGameInstance->Input_KeyState_Custom(DIK_PGDN) == KEY_STATE::TAP)
			iAnim--;

		//체력 절반이하일때 2페이즈 돌입
		if (!m_bEvolutionStart)
		{
			if (m_State.fCurHp <= m_State.fMaxHp * 0.5f)
			{
				if (m_bBurrowable && !m_bBrrow)
					m_bEvolutionStart = true;
			}
		}
		if (!m_bEvolutionFinish)
			Evolution(TimeDelta);

		//1페이즈 기본스킬들
		if (!m_bEvolution)
		{
			if (m_bCloseAttackStart)
				CloseAttack(TimeDelta);

			LineSkill(TimeDelta, m_iCurrentLine);
			Burrow(TimeDelta);
		}

		//Add Collisions
		pGameInstance->AddCollider(collider);
		pGameInstance->AddCollider(m_pOverlapCollider);


		if (m_bLineAttackExCollActive)
		{
			//LineAttackEx Coll
			for (int i = 0; i < 6; ++i)
				pGameInstance->AddCollider(m_pColliderLine[i]);
		}
		else if (m_bLineAttackCollActive)
		{
			//LineAttack Coll
			for (int i = 0; i < 3; ++i)
				pGameInstance->AddCollider(m_pColliderLine[i]);
		}

		//CloseAttack Coll
		if (m_bCloseAttackCollActive)
		{
			for (int i = 0; i < 3; ++i)
				pGameInstance->AddCollider(m_pCloseAttack[i]);
		}
	}
	else
	{
		//죽음
		if (model->AnimationCompare(BOSS_CLIP::DEATH))
		{
			if (model->AnimationIsFinishEx())
			{
				m_fDieWaitAcc += TimeDelta;
				if (m_fDieWaitAcc > m_fDieWaitTime)
				{
					m_bRender = false;
				}
			}
		}

	}

}

void CBoss::LateTick(_double TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	TimeDelta = Freeze(TimeDelta);

	//각종 애니미에션 설정 및 제어
	AnimationController(TimeDelta);
	//콜라이더 피직스 업데이트에 전달
	SetupColliders();

	if (nullptr != renderer && m_bSpawn && m_bRender)
		renderer->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CBoss::Render()
{
	if (FAILED(SetupShaderResources()))
		return E_FAIL;

	_uint MeshCount = model->Get_MeshCount();
	for (_uint i = 0; i < MeshCount; ++i)
	{
		//몸통 -> 얘 이상하게 아래로 내려가져있어서 강제로 올림 애니메이션마다 다름
		if (i == 0)
		{
			if (!m_bEvolution)
			{
				if (model->AnimationCompare(BOSS_CLIP::STAND2))
					continue;
				else
				{
					if(FAILED(transform->Setup_ShaderResource(shader, "g_WorldMatrix")))
						return E_FAIL;
				}
			}
			else
			{
				if (model->AnimationCompare(BOSS_CLIP::STANDEX))
				{
					if (FAILED(m_pBodyTransform->Setup_ShaderResource(shader, "g_WorldMatrix")))
						return E_FAIL;
				}
				else
				{
					if (FAILED(transform->Setup_ShaderResource(shader, "g_WorldMatrix")))
						return E_FAIL;
				}

			}
		}
		else
		{
			if (FAILED(transform->Setup_ShaderResource(shader, "g_WorldMatrix")))
				return E_FAIL;
		}

		if (!strcmp("Cheek", model->GetMeshName(i)) || !strcmp("Cheek01", model->GetMeshName(i)))
			continue;

		model->Setup_ShaderMaterialResource(shader, "g_DiffuseTexture", i, aiTextureType::aiTextureType_DIFFUSE);
		model->Setup_BoneMatrices(shader, "g_BoneMatrix", i);

		if (m_bAlpha)
			shader->Begin(1);
		else
			shader->Begin(0);

		model->Render(i);
	}

	return S_OK;
}

void CBoss::RenderGUI()
{
}

HRESULT CBoss::AddComponents()
{
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("proto_com_renderer"), TEXT("com_renderer"), (CComponent**)&renderer)))
		return E_FAIL;

	CTransform::TRANSFORM_DESC TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORM_DESC));
	
	TransformDesc.fMoveSpeed = 1.5f;
	TransformDesc.fRotationSpeed = 5.f;

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("proto_com_transform"), TEXT("com_transform"), (CComponent**)&transform, &TransformDesc)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("proto_com_transform"), TEXT("com_transform_body"), (CComponent**)&m_pBodyTransform, &TransformDesc)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("proto_com_shader_vtxanimmodel"), TEXT("com_shader"), (CComponent**)&shader)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("proto_com_model_boss"), TEXT("com_model"), (CComponent**)&model)))
		return E_FAIL;

	CCollider::COLLIDER_DESC collDesc;
	collDesc.owner = this;
	collDesc.vCenter = _float3(0.f, 1.f, 0.f);
	collDesc.vExtents = _float3(1.5f, 2.f, 1.5f);
	collDesc.vRotation = _float3(0.f, 0.f, 0.f);

	CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("proto_com_obb_collider"), TEXT("com_collider"), (CComponent**)&collider, &collDesc);

	ZeroMemory(&collDesc, sizeof collDesc);
	collDesc.owner = this;
	collDesc.vCenter = _float3(0.f, 1.3f, 0.f);
	collDesc.vExtents = _float3(1.1f, 1.1f, 1.1f);
	collDesc.vRotation = _float3(0.f, 0.f, 0.f);

	CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("proto_com_sphere_collider"), TEXT("com_collder_enemy"), (CComponent**)&m_pOverlapCollider, &collDesc);

	return S_OK;
}

HRESULT CBoss::SetupShaderResources()
{
	if (nullptr == shader)
		return E_FAIL;

	CGameInstance* pInstance = CGameInstance::GetInstance();

	if (FAILED(transform->Setup_ShaderResource(shader, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(shader->SetMatrix("g_ViewMatrix", &pInstance->Get_Transform_float4x4(CPipeLine::TS_VIEW))))
		return E_FAIL;

	if (FAILED(shader->SetMatrix("g_ProjMatrix", &pInstance->Get_Transform_float4x4(CPipeLine::TS_PROJ))))
		return E_FAIL;

	if (FAILED(shader->SetRawValue("g_vCamPosition", &pInstance->Get_CamPosition(), sizeof(_float3))))
		return E_FAIL;

	const LIGHT_DESC* LightDesc = pInstance->GetLightDesc(0);
	if (nullptr == LightDesc)
		return E_FAIL;

	if (FAILED(shader->SetRawValue("g_vLightDir", &LightDesc->vDirection, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(shader->SetRawValue("g_vLightDiffuse", &LightDesc->vDiffuse, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(shader->SetRawValue("g_vLightSpecular", &LightDesc->vSpecular, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(shader->SetRawValue("g_vLightAmbient", &LightDesc->vAmbient, sizeof(_float4))))
		return E_FAIL;

	return S_OK;
}

void CBoss::Spawn()
{
	//가까운지 체크하고 스폰
	if (!m_bSpawn && GetLengthFromPlayer() <= 5.f)
	{
		m_bSpawn = true;
		m_bBrrow = true;
		SetupState(BOSS_CLIP::BORN, CAnimation::TYPE::ONE, false);
	}
}

void CBoss::MoveForward(_double TimeDelta)
{
	if (m_bMoveBackward)
	{
		m_bBack = false;
		m_bMoveBackward = false;
	}

	//애니메이션 설정
	SetupState(BOSS_CLIP::ATK_FOWARD, CAnimation::TYPE::ONE, false);

	//뒤로가기 전 현재 위치 저장
	_vector vCurPos = transform->Get_State(CTransform::STATE_POSITION);
	if (!m_bBack)
	{
		transform->LookAt(m_pPlayerTransform->Get_State(CTransform::STATE_POSITION));
		_vector vLook = XMVector3Normalize(transform->Get_State(CTransform::STATE_LOOK));
		XMStoreFloat3(&m_vPrevLook, vLook);
		XMStoreFloat3(&m_vPrevPos, vCurPos);
	}

	m_bBack = true;

	_vector vLook = XMVector3Normalize(transform->Get_State(CTransform::STATE_LOOK));
	//뒤로 가야될 위치
	_vector vBackPos = XMLoadFloat3(&m_vPrevPos) + XMLoadFloat3(&m_vPrevLook) * 6.f;
	vCurPos = XMVectorLerp(vCurPos, vBackPos, TimeDelta * 6.f);
	transform->Set_State(CTransform::STATE_POSITION, vCurPos);

	if (model->AnimationCompare(BOSS_CLIP::ATK_FOWARD))
	{
		if (model->AnimationIsFinishEx())
		{
			m_bBack = false;
			m_bMoveForward = false;
		}
	}
}

void CBoss::MoveBackword(_double TimeDelta)
{
	// ****** 뒤로가는MoveBack이랑 변수 같이씀
	if (m_bMoveForward)
	{
		m_bBack = false;
		m_bMoveForward = false;
	}

	//애니메이션 설정
	SetupState(BOSS_CLIP::ATK_BACK, CAnimation::TYPE::ONE, false);

	//앞으로가기 전 현재 위치 저장
	_vector vCurPos = transform->Get_State(CTransform::STATE_POSITION);
	if (!m_bBack)
	{
		transform->LookAt(m_pPlayerTransform->Get_State(CTransform::STATE_POSITION));
		_vector vLook = XMVector3Normalize(transform->Get_State(CTransform::STATE_LOOK));
		XMStoreFloat3(&m_vPrevLook, vLook);
		XMStoreFloat3(&m_vPrevPos, vCurPos);
	}

	m_bBack = true;

	_vector vLook = XMVector3Normalize(transform->Get_State(CTransform::STATE_LOOK));
	//앞으로 가야될 위치
	_vector vBackPos = XMLoadFloat3(&m_vPrevPos) - XMLoadFloat3(&m_vPrevLook) * 6.f;
	vCurPos = XMVectorLerp(vCurPos, vBackPos, TimeDelta * 6.f);
	transform->Set_State(CTransform::STATE_POSITION, vCurPos);

	if (model->AnimationCompare(BOSS_CLIP::ATK_BACK))
	{
		if (model->AnimationIsFinishEx())
		{
			m_bBack = false;
			m_bMoveBackward = false;
		}
	}
}

//3타 라인스킬
void CBoss::LineSkill(_double TimeDelta, _int iIndex)
{
	if (m_bUseLineSkill)
	{
		m_bBurrowable = false;
		LookTarget(TimeDelta, 0.8f);
		SetupState(BOSS_CLIP::ATK4, CAnimation::TYPE::ONE, false);
		if (!m_bLineAttack)
		{
			if (model->AnimationCompare(BOSS_CLIP::ATK4))
			{
				if (model->AnimationIsPreFinishCustom(0.25))
				{
					m_bLineAttack = true;
					m_bLineSkillStart[0] = true;
					m_bLineAttackCollActive = true;
				}
			}
		}
	}
	else
	{
		m_bLineAttack = false;
	}

	if (m_pColliderLine[0]->IsActive())
	{
		_vector vCurPos = m_pColliderTransform[0]->Get_State(CTransform::STATE_POSITION) + XMLoadFloat3(&m_vLineOneDir[0]) * m_fLineCollSpeed * TimeDelta;
		m_pColliderTransform[0]->Set_State(CTransform::STATE_POSITION, vCurPos);
	}

	if (m_pColliderLine[1]->IsActive())
	{
		_vector vCurPos = m_pColliderTransform[1]->Get_State(CTransform::STATE_POSITION) + XMLoadFloat3(&m_vLineOneDir[1]) * m_fLineCollSpeed * TimeDelta;
		m_pColliderTransform[1]->Set_State(CTransform::STATE_POSITION, vCurPos);
	}

	if (m_pColliderLine[2]->IsActive())
	{
		_vector vCurPos = m_pColliderTransform[2]->Get_State(CTransform::STATE_POSITION) + XMLoadFloat3(&m_vLineOneDir[2]) * m_fLineCollSpeed * TimeDelta;
		m_pColliderTransform[2]->Set_State(CTransform::STATE_POSITION, vCurPos);
	}


	if (m_bLineSkillStart[0])
	{
		_vector vBossPos = transform->Get_State(CTransform::STATE_POSITION);

		if (!m_bCheckDir[0])
		{
			_vector vDir = XMVector3Normalize(m_pPlayerTransform->Get_State(CTransform::STATE_POSITION) - transform->Get_State(CTransform::STATE_POSITION));
			XMStoreFloat3(&m_vLineOneDir[0], vDir);

			m_pColliderTransform[0]->Set_State(CTransform::STATE_POSITION, vBossPos);
			m_pColliderLine[0]->SetActive(true);
			m_bCheckDir[0] = true;
		}

		if (!m_pThorn1[m_iCurrentOneIndex[0]][LINE_ONE]->IsRender())
		{
			int iRandomPos = rand() % 3 - 1;

			int iRandom = rand() % 3 + 1;
			int iRandomAngleX = rand() % 90 - 45;
			int iRandomAngleZ = rand() % 90 - 45;

			_vector vThornPos;
			vThornPos = vBossPos + XMLoadFloat3(&m_vLineOneDir[0]) * (m_iCurrentOneIndex[0] * 0.5f);
			m_pThorn1[m_iCurrentOneIndex[0]][LINE_ONE]->SetPosition(vThornPos);
			m_pThorn1[m_iCurrentOneIndex[0]][LINE_ONE]->SetRotationXYZ(_float3(iRandomAngleX, 0.f, iRandomAngleZ));
			m_pThorn1[m_iCurrentOneIndex[0]][LINE_ONE]->SetupScaleUpStart(max((iRandom + m_iCurrentOneIndex[LINE_ONE]) * 0.4f, 4.f));
		}

		if (!m_pThorn2[m_iCurrentTwoIndex[0]][LINE_ONE]->IsRender())
		{
			int iRandomPos = rand() % 3 - 1;

			int iRandom = rand() % 3 + 1;
			int iRandomAngleX = rand() % 90 - 45;
			int iRandomAngleZ = rand() % 90 - 45;

			_vector vThornPos;
			vThornPos = vBossPos + XMLoadFloat3(&m_vLineOneDir[0]) * (m_iCurrentTwoIndex[0] * 0.5f);
			m_pThorn2[m_iCurrentTwoIndex[0]][LINE_ONE]->SetPosition(vThornPos);
			m_pThorn2[m_iCurrentTwoIndex[0]][LINE_ONE]->SetRotationXYZ(_float3(iRandomAngleX, 0.f, iRandomAngleZ));
			m_pThorn2[m_iCurrentTwoIndex[0]][LINE_ONE]->SetupScaleUpStart(max((iRandom + m_iCurrentTwoIndex[LINE_ONE]) * 0.4f, 4.f));
		}

		if (m_iCurrentOneIndex[0] < 24 && m_pThorn1[m_iCurrentOneIndex[0]][LINE_ONE]->IsScaleFinish())
			m_iCurrentOneIndex[0]++;

		if (m_iCurrentTwoIndex[0] < 24 && m_pThorn2[m_iCurrentTwoIndex[0]][LINE_ONE]->IsScaleFinish())
			m_iCurrentTwoIndex[0]++;

		if (m_iCurrentOneIndex[0] == 24 && m_iCurrentTwoIndex[0] == 24)
		{
			m_iCurrentOneIndex[0] = 0;
			m_iCurrentTwoIndex[0] = 0;
			m_bLineSkillStart[0] = false;
			m_bLineSkillErase[0] = true;
			m_bCheckDir[0] = false;

			m_bLineSkillStart[1] = true;
			m_pColliderLine[0]->SetActive(false);
		}
	}
	else if (m_bLineSkillStart[1])
	{
		_vector vBossPos = transform->Get_State(CTransform::STATE_POSITION);
		if (!m_bCheckDir[1])
		{
			_vector vDir = XMVector3Normalize(m_pPlayerTransform->Get_State(CTransform::STATE_POSITION) - transform->Get_State(CTransform::STATE_POSITION));
			XMStoreFloat3(&m_vLineOneDir[1], vDir);
			m_pColliderTransform[1]->Set_State(CTransform::STATE_POSITION, vBossPos);
			m_pColliderLine[1]->SetActive(true);
			m_bCheckDir[1] = true;
		}

		if (!m_pThorn1[m_iCurrentOneIndex[1]][LINE_TWO]->IsRender())
		{
			int iRandomPos = rand() % 3 - 1;

			int iRandom = rand() % 3 + 1;
			int iRandomAngleX = rand() % 90 - 45;
			int iRandomAngleZ = rand() % 90 - 45;

			_vector vThornPos;
			vThornPos = vBossPos + XMLoadFloat3(&m_vLineOneDir[1]) * (m_iCurrentOneIndex[1] * 0.5f);
			m_pThorn1[m_iCurrentOneIndex[1]][LINE_TWO]->SetPosition(vThornPos);
			m_pThorn1[m_iCurrentOneIndex[1]][LINE_TWO]->SetRotationXYZ(_float3(iRandomAngleX, 0.f, iRandomAngleZ));
			m_pThorn1[m_iCurrentOneIndex[1]][LINE_TWO]->SetupScaleUpStart(max((iRandom + m_iCurrentOneIndex[LINE_TWO]) * 0.4f, 4.f));
		}

		if (!m_pThorn2[m_iCurrentTwoIndex[1]][LINE_TWO]->IsRender())
		{
			int iRandomPos = rand() % 3 - 1;

			int iRandom = rand() % 3 + 1;
			int iRandomAngleX = rand() % 90 - 45;
			int iRandomAngleZ = rand() % 90 - 45;

			_vector vThornPos;
			vThornPos = vBossPos + XMLoadFloat3(&m_vLineOneDir[1]) * (m_iCurrentTwoIndex[1] * 0.5f);
			m_pThorn2[m_iCurrentTwoIndex[1]][LINE_TWO]->SetPosition(vThornPos);
			m_pThorn2[m_iCurrentTwoIndex[1]][LINE_TWO]->SetRotationXYZ(_float3(iRandomAngleX, 0.f, iRandomAngleZ));
			m_pThorn2[m_iCurrentTwoIndex[1]][LINE_TWO]->SetupScaleUpStart(max((iRandom + m_iCurrentTwoIndex[LINE_TWO]) * 0.4f, 3.f));
		}

		if (m_iCurrentOneIndex[1] < 24 && m_pThorn1[m_iCurrentOneIndex[1]][LINE_TWO]->IsScaleFinish())
			m_iCurrentOneIndex[1]++;

		if (m_iCurrentTwoIndex[1] < 24 && m_pThorn2[m_iCurrentTwoIndex[1]][LINE_TWO]->IsScaleFinish())
			m_iCurrentTwoIndex[1]++;

		if (m_iCurrentOneIndex[1] == 24 && m_iCurrentTwoIndex[1] == 24)
		{
			m_iCurrentOneIndex[1] = 0;
			m_iCurrentTwoIndex[1] = 0;
			m_bLineSkillStart[1] = false;
			m_bLineSkillErase[1] = true;
			m_bCheckDir[1] = false;

			m_bLineSkillStart[2] = true;

			m_pColliderLine[1]->SetActive(false);
		}
	}
	else if (m_bLineSkillStart[2])
	{
		m_bUseLineSkill = false;
		_vector vBossPos = transform->Get_State(CTransform::STATE_POSITION);
		if (!m_bCheckDir[2])
		{
			_vector vDir = XMVector3Normalize(m_pPlayerTransform->Get_State(CTransform::STATE_POSITION) - transform->Get_State(CTransform::STATE_POSITION));
			XMStoreFloat3(&m_vLineOneDir[2], vDir);
			m_pColliderTransform[2]->Set_State(CTransform::STATE_POSITION, vBossPos);
			m_pColliderLine[2]->SetActive(true);
			m_bCheckDir[2] = true;
		}

		if (!m_pThorn1[m_iCurrentOneIndex[2]][LINE_TRE]->IsRender())
		{
			int iRandomPos = rand() % 3 - 1;

			int iRandom = rand() % 3 + 1;
			int iRandomAngleX = rand() % 90 - 45;
			int iRandomAngleZ = rand() % 90 - 45;

			_vector vThornPos;
			vThornPos = vBossPos + XMLoadFloat3(&m_vLineOneDir[2]) * (m_iCurrentOneIndex[2] * 0.5f);
			m_pThorn1[m_iCurrentOneIndex[2]][LINE_TRE]->SetPosition(vThornPos);
			m_pThorn1[m_iCurrentOneIndex[2]][LINE_TRE]->SetRotationXYZ(_float3(iRandomAngleX, 0.f, iRandomAngleZ));
			m_pThorn1[m_iCurrentOneIndex[2]][LINE_TRE]->SetupScaleUpStart(max((iRandom + m_iCurrentOneIndex[LINE_TRE]) * 0.4f, 3.f));
		}

		if (!m_pThorn2[m_iCurrentTwoIndex[2]][LINE_TRE]->IsRender())
		{
			int iRandomPos = rand() % 3 - 1;

			int iRandom = rand() % 3 + 1;
			int iRandomAngleX = rand() % 90 - 45;
			int iRandomAngleZ = rand() % 90 - 45;

			_vector vThornPos;
			vThornPos = vBossPos + XMLoadFloat3(&m_vLineOneDir[2]) * (m_iCurrentTwoIndex[2] * 0.5f);
			m_pThorn2[m_iCurrentTwoIndex[2]][LINE_TRE]->SetPosition(vThornPos);
			m_pThorn2[m_iCurrentTwoIndex[2]][LINE_TRE]->SetRotationXYZ(_float3(iRandomAngleX, 0.f, iRandomAngleZ));
			m_pThorn2[m_iCurrentTwoIndex[2]][LINE_TRE]->SetupScaleUpStart(max((iRandom + m_iCurrentTwoIndex[LINE_TRE]) * 0.4f, 3.f));
		}

		if (m_iCurrentOneIndex[2] < 24 && m_pThorn1[m_iCurrentOneIndex[2]][LINE_TRE]->IsScaleFinish())
			m_iCurrentOneIndex[2]++;

		if (m_iCurrentTwoIndex[2] < 24 && m_pThorn2[m_iCurrentTwoIndex[2]][LINE_TRE]->IsScaleFinish())
			m_iCurrentTwoIndex[2]++;

		if (m_iCurrentOneIndex[2] == 24 && m_iCurrentTwoIndex[2] == 24)
		{
			m_iCurrentOneIndex[2] = 0;
			m_iCurrentTwoIndex[2] = 0;
			m_bLineSkillStart[2] = false;
			m_bLineSkillErase[2] = true;
			m_bCheckDir[2] = false;

			m_pColliderLine[2]->SetActive(false);
		}
	}


	if (m_bLineSkillErase[0])
	{
		m_fEraseAcc[0] += TimeDelta;
		if (m_fEraseAcc[0] >= m_fEraseTime)
		{
			m_fEraseAcc[0] = 0.f;
			//m_pThorn1[m_iEraseIndex[0]][0]->SetRender(false);
			//m_pThorn2[m_iEraseIndex[0]][0]->SetRender(false);
			//m_pThorn1[m_iEraseIndex[0]][0]->Reset();
			//m_pThorn2[m_iEraseIndex[0]][0]->Reset();

			m_pThorn1[m_iEraseIndex[0]][0]->SetupScaleSmoothDownStart();
			m_pThorn2[m_iEraseIndex[0]][0]->SetupScaleSmoothDownStart();

			m_iEraseIndex[0]++;
		}

		if (m_iEraseIndex[0] == 25)
		{
			m_iEraseIndex[0] = 0;
			m_bLineSkillErase[0] = false;
		}
	}
	else if (m_bLineSkillErase[1])
	{
		m_fEraseAcc[1] += TimeDelta;
		if (m_fEraseAcc[1] >= m_fEraseTime)
		{
			m_fEraseAcc[1] = 0.f;
			//m_pThorn1[m_iEraseIndex[1]][1]->SetRender(false);
			//m_pThorn2[m_iEraseIndex[1]][1]->SetRender(false);

			m_pThorn1[m_iEraseIndex[1]][1]->SetupScaleSmoothDownStart();
			m_pThorn2[m_iEraseIndex[1]][1]->SetupScaleSmoothDownStart();

			m_iEraseIndex[1]++;
		}

		if (m_iEraseIndex[1] == 25)
		{
			m_iEraseIndex[1] = 0;
			m_bLineSkillErase[1] = false;
		}
	}
	else if (m_bLineSkillErase[2])
	{
		m_fEraseAcc[2] += TimeDelta;
		if (m_fEraseAcc[2] >= m_fEraseTime)
		{
			m_fEraseAcc[2] = 0.f;
			//m_pThorn1[m_iEraseIndex[2]][2]->SetRender(false);
			//m_pThorn2[m_iEraseIndex[2]][2]->SetRender(false);

			m_pThorn1[m_iEraseIndex[2]][2]->SetupScaleSmoothDownStart();
			m_pThorn2[m_iEraseIndex[2]][2]->SetupScaleSmoothDownStart();

			m_iEraseIndex[2]++;
		}

		if (m_iEraseIndex[2] == 25)
		{
			m_iEraseIndex[2] = 0;
			m_bLineSkillErase[2] = false;
			m_bBurrowable = true;
		}
	}
}

void CBoss::Missile1(_double TimeDelta)
{
	_vector vBossPos = transform->Get_State(CTransform::STATE_POSITION);
	_vector vPlayerDir = XMVector3Normalize(m_pPlayerTransform->Get_State(CTransform::STATE_POSITION) - vBossPos);
	_vector vRight = XMVector3Cross(VECTOR_UP, vPlayerDir);

	LookTarget(TimeDelta, 1.5f);

	if (m_bEvolution)
	{
		if (model->AnimationCompare(BOSS_CLIP::ATK12))
		{
			if (model->AnimationIsFinishEx())
				m_bUseMissile1 = false;

			if (model->AnimationIsPreFinishCustom(0.4))
			{
				for (int i = 0; i < 3; ++i)
				{
					m_pThornMissileLeft[i]->SetMoveStart();
					m_pThornMissileMiddle[i]->SetMoveStart();
					m_pThornMissileRight[i]->SetMoveStart();
				}
			}
		}
		else
		{
			enum { LEFT, MIDDLE, RIGHT };
			for (int i = 0; i < 3; ++i)
			{
				_vector vMissilePos = XMVectorSetY(vBossPos, 1.f * i + 1) + XMVector3Normalize(vPlayerDir + -vRight) * 2.f;
				m_pThornMissileLeft[i]->SetRotationToTarget(vPlayerDir * 2 - vRight);
				m_pThornMissileLeft[i]->SetPosition(vMissilePos);
				m_pThornMissileLeft[i]->SetupScaleUpStart(2.f);
			}
			for (int i = 0; i < 3; ++i)
			{
				_vector vMissilePos = XMVectorSetY(vBossPos, 1.f * i + 1) + vPlayerDir * 2.f;
				m_pThornMissileMiddle[i]->SetRotationToTarget(vPlayerDir);
				m_pThornMissileMiddle[i]->SetPosition(vMissilePos);
				m_pThornMissileMiddle[i]->SetupScaleUpStart(2.f);
			}
			for (int i = 0; i < 3; ++i)
			{
				_vector vMissilePos = XMVectorSetY(vBossPos, 1.f * i + 1) + XMVector3Normalize(vPlayerDir + vRight) * 2.f;
				m_pThornMissileRight[i]->SetRotationToTarget(vPlayerDir * 2 + vRight);
				m_pThornMissileRight[i]->SetPosition(vMissilePos);
				m_pThornMissileRight[i]->SetupScaleUpStart(2.f);
			}
		}
	}
	else
	{
		if (model->AnimationCompare(BOSS_CLIP::ATK3))
		{
			if (model->AnimationIsFinishEx())
				m_bUseMissile1 = false;

			if (model->AnimationIsPreFinishCustom(0.5))
			{
				for (int i = 0; i < 3; ++i)
				{
					m_pThornMissileLeft[i]->SetMoveStart();
					m_pThornMissileMiddle[i]->SetMoveStart();
					m_pThornMissileRight[i]->SetMoveStart();
				}
			}
		}
		else
		{
			enum { LEFT, MIDDLE, RIGHT };
			for (int i = 0; i < 3; ++i)
			{
				_vector vMissilePos = XMVectorSetY(vBossPos, 1.f * i + 1) + XMVector3Normalize(vPlayerDir + -vRight) * 2.f;
				m_pThornMissileLeft[i]->SetRotationToTarget(vPlayerDir * 2 - vRight);
				m_pThornMissileLeft[i]->SetPosition(vMissilePos);
				m_pThornMissileLeft[i]->SetupScaleUpStart(2.f);
			}
			for (int i = 0; i < 3; ++i)
			{
				_vector vMissilePos = XMVectorSetY(vBossPos, 1.f * i + 1) + vPlayerDir * 2.f;
				m_pThornMissileMiddle[i]->SetRotationToTarget(vPlayerDir);
				m_pThornMissileMiddle[i]->SetPosition(vMissilePos);
				m_pThornMissileMiddle[i]->SetupScaleUpStart(2.f);
			}
			for (int i = 0; i < 3; ++i)
			{
				_vector vMissilePos = XMVectorSetY(vBossPos, 1.f * i + 1) + XMVector3Normalize(vPlayerDir + vRight) * 2.f;
				m_pThornMissileRight[i]->SetRotationToTarget(vPlayerDir * 2 + vRight);
				m_pThornMissileRight[i]->SetPosition(vMissilePos);
				m_pThornMissileRight[i]->SetupScaleUpStart(2.f);
			}
		}
	}


	if(m_bEvolution)
		SetupState(BOSS_CLIP::ATK12, CAnimation::TYPE::ONE, false);
	else
		SetupState(BOSS_CLIP::ATK3, CAnimation::TYPE::ONE, false);

}

void CBoss::CloseAttack(_double TimeDelta)
{
	_vector vBossPos = transform->Get_State(CTransform::STATE_POSITION);
	_vector vPlayerPos = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	_vector vDir = XMVector3Normalize(vPlayerPos - vBossPos);

	if (!m_bCloseAttack)
	{
		m_pPlayer->Hit();
		m_pPlayer->RecvDamage(50.f);

		m_bCloseAttack = true;
		for (int i = 0; i < 9; ++i)
		{
			int iRandomPos = rand() % 3 - 1;
			int iRandom = rand() % 9 + 3;
			int iRandomAngleX = rand() % 120 - 60;
			int iRandomAngleZ = rand() % 120 - 60;

			int iRandomPosX = rand() % 3 - 1;
			int iRandomPosZ = rand() % 3 - 1;

			_vector vThornPos = vBossPos + vDir * 1.5f;
			vThornPos = XMVectorSetX(vThornPos, XMVectorGetX(vThornPos) + iRandomPosX * 0.5f);
			vThornPos = XMVectorSetZ(vThornPos, XMVectorGetZ(vThornPos) + iRandomPosZ * 0.5f);

			m_pThornClose[i]->SetPosition(vThornPos);
			m_pThornClose[i]->SetRotationXYZ(_float3(iRandomAngleX, 0.f, iRandomAngleZ));
			m_pThornClose[i]->SetupScaleUpStart(iRandom);
		}
	}

	m_fCloseAttackActiveAcc += TimeDelta;
	if (m_fCloseAttackActiveAcc >= m_fCloseAttackActiveTime)
	{
		m_bCloseAttack = false;
		m_bCloseAttackStart = false;
		m_fCloseAttackActiveAcc = 0.f;
		for (int i = 0; i < 9; ++i)
		{
			m_pThornClose[i]->SetupScaleSmoothDownStart();
		}
	}

}

void CBoss::Burrow(_double TimeDelta)
{
	if (m_bBrrow)
		LookTarget(TimeDelta, 1.f);

	if (!m_bBurrowStart)
		return;

	if (!m_bBurrowable || m_bLineAttack || m_bUseMissile1 || m_bUseLineSkill)
		return;

	m_bBrrow = true;

	//if (model->AnimationCompare(BOSS_CLIP::STANDEX) || model->AnimationCompare(BOSS_CLIP::STAND2))
	//	m_bBrrow = true;
	//else
	//{
	//	if (model->AnimationIsFinishEx())
	//		m_bBrrow = true;
	//}

	_vector vPosition = transform->Get_State(CTransform::STATE_POSITION);
	_vector vUp = transform->Get_State(CTransform::STATE_UP);

	//아래 방향으로
	m_fBurrowAcc += TimeDelta;
	vPosition = vPosition - vUp * powf(m_fBurrowAcc, 2.f);
	transform->Set_State(CTransform::STATE_POSITION, vPosition);

	if (m_fBurrowAcc >= 0.3f)
	{
		m_bBurrowStart = false;
		m_fBurrowAcc = 0.f;

		if (!m_bEvolutionStart)
		{
			//플레이어 전방으로
			if (m_bTooFar)
			{
				m_bTooFar = false;
				m_bBurrowStart = false;
				m_fBurrowAcc = 0.f;
				m_bCloseAttackStart = true;

				_vector vPlayerPos = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);
				_vector vDir = XMVector3Normalize(XMVectorSetY(vPosition - vPlayerPos, 0.f));
				_vector vPos = vPlayerPos + vDir * 3.f;
				transform->Set_State(CTransform::STATE_POSITION, vPos);

				SetupState(BOSS_CLIP::BORN, CAnimation::TYPE::ONE, false);
			}
			else
			{
				m_bBurrowStart = false;
				m_fBurrowAcc = 0.f;

				transform->Set_State(CTransform::STATE_POSITION, XMVectorSet(1.3f, 0.f, 21.f, 1.f));
				SetupState(BOSS_CLIP::BORN, CAnimation::TYPE::ONE, false);
			}
		}
		else
		{
			m_bBurrowStart = false;
			m_fBurrowAcc = 0.f;

			transform->Set_State(CTransform::STATE_POSITION, XMVectorSet(1.3f, 0.f, 21.f, 1.f));
			SetupState(BOSS_CLIP::BORN, CAnimation::TYPE::ONE, false);
		}

	}
}

void CBoss::Evolution(_double TimeDelta)
{
	if (!m_bAttackable && !m_bBurrowable)
		return;

	if (m_bEvolutionStart)
	{
		if (!model->AnimationCompare(BOSS_CLIP::BORN))
		{
			if (m_bBurrowable)
				m_bBurrowStart = true;
		}
		else
		{
			if (model->AnimationIsFinishEx())
			{
				m_bBurrowable = true;
				m_bEvolutionStart = false;
				m_bEvolution = true;
				SetupState(BOSS_CLIP::ATK5, CAnimation::ONE, false);
			}
		}
	}

	if (model->AnimationCompare(BOSS_CLIP::ATK5))
	{
		if (model->AnimationIsFinishEx())
		{
			m_bAttackable = true;
			m_fAttackAcc = 0.f;
			m_iAttackCount = 0;
			m_bBrrow = false;
			m_bEvolutionFinish = true;
		}
	}
}

void CBoss::ColseAttack2(_double TimeDelta)
{
	m_bAttackable = false;

	LookTarget(TimeDelta, 1.5f);

	if (!model->AnimationCompare(BOSS_CLIP::ATK13))
	{
		if (model->AnimationCompare(STANDEX))
		{
			SetupState(BOSS_CLIP::ATK13, CAnimation::TYPE::ONE, false);
			m_bCloseAttackExBegin = true;
		}
		else if (!model->AnimationCompare(STANDEX))
		{
			if (model->AnimationIsFinishEx())
			{
				SetupState(BOSS_CLIP::ATK13, CAnimation::TYPE::ONE, false);
				m_bCloseLockTarget = true;
			}
		}
	}

	if (m_bCloseLockTarget)
	{
		if (model->AnimationCompare(ATK13))
		{
			if (model->AnimationIsPreFinishCustom(0.5))
			{
				_vector vBossPos = transform->Get_State(CTransform::STATE_POSITION);
				_vector vRight = XMVector3Normalize(transform->Get_State(CTransform::STATE_RIGHT));
				_vector vPlayerPos = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);
				_vector vDir = XMVector3Normalize(vPlayerPos - vBossPos);
				
				m_bCloseAttackExBegin = true;
				XMStoreFloat3(&vPrevPlayerPos, vPlayerPos);
			}
		}
	}
	
	if (m_bCloseAttackExBegin)
	{
		if (!m_bCloseAttack)
		{
			m_bCloseAttack = true;

			int iDirRandom = rand() % 7;
			_vector vPlayerPos = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);
			_vector vDir = XMVector3Normalize(m_pPlayerTransform->Get_State(CTransform::STATE_LOOK));

			_vector vThornPos = vPlayerPos + vDir * (iDirRandom);
			m_pColliderCloseTransform[ONE]->Set_State(CTransform::STATE_POSITION, vThornPos);
			m_pCloseAttack[ONE]->SetActive(true);
			m_bCloseAttackCollActive = true;

			for (int i = 0; i < 9; ++i)
			{
				int iRandomPos = rand() % 20 - 10;
				int iRandom = rand() % 12;
				int iRandomAngleX = rand() % 90 - 45;
				int iRandomAngleZ = rand() % 90 - 45;

				vThornPos = XMVectorSetX(vThornPos, XMVectorGetX(vThornPos) + iRandomPos * 0.1f);
				vThornPos = XMVectorSetZ(vThornPos, XMVectorGetZ(vThornPos) + iRandomPos * 0.1f);

				m_pThornCloseFront[i]->SetPosition(vThornPos);
				m_pThornCloseFront[i]->SetRotationXYZ(_float3(iRandomAngleX, 0.f, iRandomAngleZ));
				m_pThornCloseFront[i]->SetupScaleUpStart(iRandom);
			}
		}

		if (m_bCloseAttack)
		{
			if (m_iColseAttackIndex < 2)
			{
				m_fNextIndexAcc += TimeDelta;
				if (m_fNextIndexAcc >= m_fNextIndexTime)
				{
					m_fNextIndexAcc = 0.f;
					m_bColseAttackExStart[m_iColseAttackIndex] = true;
					m_iColseAttackIndex++;
				}
			}
		}

		//두번째 양 사이드 시작
		if (m_bColseAttackExStart[0])
		{
			if (!m_bColseAttackEx[0])
			{
				int iDirRandom = rand() % 7;
				m_bColseAttackEx[0] = true;
				_vector vPlayerPos = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);
				_vector vDir = XMVector3Normalize(m_pPlayerTransform->Get_State(CTransform::STATE_LOOK));

				_vector vThornPos = vPlayerPos + vDir * (iDirRandom);
				m_pColliderCloseTransform[TWO]->Set_State(CTransform::STATE_POSITION, vThornPos);
				m_pCloseAttack[TWO]->SetActive(true);
				//왼쪽 전방
				for (int i = 0; i < 9; ++i)
				{
					int iRandomPos = rand() % 20 - 10;
					int iRandom = rand() % 10;
					int iRandomAngleX = rand() %  90 - 45;
					int iRandomAngleZ = rand() %  90 - 45;

					//vDir = XMVector3Normalize(vDir + -vRight);
					vThornPos = XMVectorSetX(vThornPos, XMVectorGetX(vThornPos) + iRandomPos * 0.1f);
					vThornPos = XMVectorSetZ(vThornPos, XMVectorGetZ(vThornPos) + iRandomPos * 0.1f);

					m_pThornCloseLeftFront[i]->SetPosition(vThornPos);
					m_pThornCloseLeftFront[i]->SetRotationXYZ(_float3(iRandomAngleX, 0.f, iRandomAngleZ));
					m_pThornCloseLeftFront[i]->SetupScaleUpStart(iRandom);
				}

				//오른쪽 전방
				for (int i = 0; i < 9; ++i)
				{
					int iRandomPos = rand() % 20 - 10;
					int iRandom = rand() % 10;
					int iRandomAngleX = rand() %  90 - 45;
					int iRandomAngleZ = rand() %  90 - 45;

					//vDir = XMVector3Normalize(vDir + vRight);
					_vector vThornPos = vPlayerPos + vDir * (iDirRandom);
					vThornPos = XMVectorSetX(vThornPos, XMVectorGetX(vThornPos) + iRandomPos * 0.1f);
					vThornPos = XMVectorSetZ(vThornPos, XMVectorGetZ(vThornPos) + iRandomPos * 0.1f);

					m_pThornCloseRightFront[i]->SetPosition(vThornPos);
					m_pThornCloseRightFront[i]->SetRotationXYZ(_float3(iRandomAngleX, 0.f, iRandomAngleZ));
					m_pThornCloseRightFront[i]->SetupScaleUpStart(iRandom);
				}
			}
		}

		//두번째 양 사이드 시작
		if (m_bColseAttackExStart[1])
		{
			if (!m_bColseAttackEx[1])
			{
				int iDirRandom = rand() % 7;
				_vector vPlayerPos = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);
				_vector vDir = XMVector3Normalize(m_pPlayerTransform->Get_State(CTransform::STATE_LOOK));

				m_bColseAttackEx[1] = true;

				m_bColseAttackEx[0] = true;

				_vector vThornPos = vPlayerPos + vDir * (iDirRandom);
				m_pColliderCloseTransform[TRE]->Set_State(CTransform::STATE_POSITION, vThornPos);
				m_pCloseAttack[TRE]->SetActive(true);
				//왼쪽
				for (int i = 0; i < 9; ++i)
				{
					int iRandomPos = rand() % 20 - 10;
					int iRandom = rand() % 9;
					int iRandomAngleX = rand() %  90 - 45;
					int iRandomAngleZ = rand() %  90 - 45;

					vThornPos = XMVectorSetX(vThornPos, XMVectorGetX(vThornPos) + iRandomPos * 0.1f);
					vThornPos = XMVectorSetZ(vThornPos, XMVectorGetZ(vThornPos) + iRandomPos * 0.1f);
					
					m_pThornCloseLeft[i]->SetPosition(vThornPos);
					m_pThornCloseLeft[i]->SetRotationXYZ(_float3(iRandomAngleX, 0.f, iRandomAngleZ));
					m_pThornCloseLeft[i]->SetupScaleUpStart(iRandom);
				}

				//오른쪽
				for (int i = 0; i < 9; ++i)
				{
					int iRandomPos = rand() % 20 - 10;
					int iRandom = rand() % 9;
					int iRandomAngleX = rand() %  90 - 45;
					int iRandomAngleZ = rand() %  90 - 45;

					_vector vThornPos = vPlayerPos + vDir * (iDirRandom);
					vThornPos = XMVectorSetX(vThornPos, XMVectorGetX(vThornPos) + iRandomPos * 0.1f);
					vThornPos = XMVectorSetZ(vThornPos, XMVectorGetZ(vThornPos) + iRandomPos * 0.1f);

					m_pThornCloseRight[i]->SetPosition(vThornPos);
					m_pThornCloseRight[i]->SetRotationXYZ(_float3(iRandomAngleX, 0.f, iRandomAngleZ));
					m_pThornCloseRight[i]->SetupScaleUpStart(iRandom);
				}
			}
		}

		m_fCloseAttackActiveAcc += TimeDelta;
		if (m_fCloseAttackActiveAcc >= m_fCloseAttackActiveTime)
		{
			m_fNextIndexAcc = 0.f;
			m_iColseAttackIndex = 0;
			m_bCloseLockTarget = false;

			m_bCloseAttackCollActive = false;

			m_bAttackable = true;
			m_bCloseAttack = false;
			m_bCloseAttackExStart = false;
			m_bCloseAttackStart = false;
			m_fCloseAttackActiveAcc = 0.f;

			m_bColseAttackEx[0] = false;
			m_bColseAttackEx[1] = false;
			m_bColseAttackExStart[0] = false;
			m_bColseAttackExStart[1] = false;

			for (int i = 0; i < 3; ++i)
			{
				m_pCloseAttack[m_iColseAttackIndex]->SetActive(false);
				m_pColliderCloseTransform[i]->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 10.f, 0.f, 1.f));
			}

			for (int i = 0; i < 9; ++i)
			{
				m_pThornCloseFront[i]->SetupScaleSmoothDownStart();
				m_pThornCloseRightFront[i]->SetupScaleSmoothDownStart();
				m_pThornCloseLeftFront[i]->SetupScaleSmoothDownStart();
				m_pThornCloseRight[i]->SetupScaleSmoothDownStart();
				m_pThornCloseLeft[i]->SetupScaleSmoothDownStart();
				m_bCloseAttackExBegin = false;
			}
		}
	}

}

void CBoss::LineSkill2(_double TimeDelta)
{
	enum { LEFT, FRONT, RIGHT, LINE_END };

	if (!m_bLineExAnimStart)
	{
		m_bLineExAnimStart = true;
		SetupState(BOSS_CLIP::ATK13, CAnimation::TYPE::ONE, false);
	}

	_vector vBossPos = transform->Get_State(CTransform::STATE_POSITION);
	_vector vPlayerPos;// = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	_vector vBossRight;// = transform->Get_State(CTransform::STATE_RIGHT);
	_vector vDir;// = XMVector3Normalize(vPlayerPos - vBossPos);
	_vector vLeftDir;// = XMVector3Normalize(vDir * 2.f - vBossRight);
	_vector vRightDir;// = XMVector3Normalize(vDir * 2.f + vBossRight);
	
	//모두 다 생성하기전까지 회전
	_bool bRotationFinish = LookTarget(TimeDelta, 2.f);

	if (model->AnimationCompare(BOSS_CLIP::ATK13))
	{
		if (!m_bLineExAnimSetup)
		{
			if (model->AnimationIsPreFinishCustom(0.4))
			{
				//회전이 종료됬을떄 라인공격 실행
				if (!m_bLineAttack)
				{
					if (bRotationFinish)
					{
						m_bLineAttackCollActive = true;

						m_bAttackable = false;
						m_bLineAttack = true;

						vPlayerPos = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);
						vBossRight = transform->Get_State(CTransform::STATE_RIGHT);
						vDir = XMVector3Normalize(vPlayerPos - vBossPos);
						vLeftDir = XMVector3Normalize(vDir * 2.f - vBossRight);
						vRightDir = XMVector3Normalize(vDir * 2.f + vBossRight);

						XMStoreFloat3(&m_vFrontDir, vDir);
						XMStoreFloat3(&m_vLeftDir, vLeftDir);
						XMStoreFloat3(&m_vRightDir, vRightDir);

						m_bLineSkillStart[LEFT] = true;
						m_bLineSkillStart[FRONT] = true;
						m_bLineSkillStart[RIGHT] = true;

						//콜라이더 및 콜라이더 트렌스폼
						for (int i = 0; i < 3; ++i)
						{
							m_pColliderTransform[i]->Set_State(CTransform::STATE_POSITION, transform->Get_State(CTransform::STATE_POSITION));
							m_pColliderLine[i]->SetActive(true);
						}

					}
					else
						return;
				}
			}
		}

	}
	

	//왼쪽
	if (m_bLineSkillStart[LEFT])
	{
		//좌측 Coll
		{
			_vector vLeftCollPos = m_pColliderTransform[LEFT]->Get_State(CTransform::STATE_POSITION) + XMLoadFloat3(&m_vLeftDir) * m_fLineCollSpeed * TimeDelta;
			m_pColliderTransform[LEFT]->Set_State(CTransform::STATE_POSITION, vLeftCollPos);
		}

		if (!m_pThorn1[m_iCurrentOneIndex[LEFT]][LEFT]->IsRender())
		{
			int iRandomPos = rand() % 3 - 1;

			int iRandom = rand() % 3 + 1;
			int iRandomAngleX = rand() % 90 - 45;
			int iRandomAngleZ = rand() % 90 - 45;

			_vector vThornPos;
			vThornPos = vBossPos + XMLoadFloat3(&m_vLeftDir) * m_iCurrentOneIndex[FRONT] * 0.5f;

			m_pThorn1[m_iCurrentOneIndex[LEFT]][LEFT]->SetPosition(vThornPos);
			m_pThorn1[m_iCurrentOneIndex[LEFT]][LEFT]->SetRotationXYZ(_float3(iRandomAngleX, 0.f, iRandomAngleZ));
			m_pThorn1[m_iCurrentOneIndex[LEFT]][LEFT]->SetupScaleUpStart(max((iRandom + m_iCurrentOneIndex[LEFT]) * 0.4f, 4.f));
		}

		if (!m_pThorn2[m_iCurrentTwoIndex[LEFT]][LEFT]->IsRender())
		{
			int iRandomPos = rand() % 3 - 1;

			int iRandom = rand() % 3 + 1;
			int iRandomAngleX = rand() % 90 - 45;
			int iRandomAngleZ = rand() % 90 - 45;

			_vector vThornPos;
			vThornPos = vBossPos + XMLoadFloat3(&m_vLeftDir) * m_iCurrentTwoIndex[FRONT] * 0.5f;

			m_pThorn2[m_iCurrentTwoIndex[LEFT]][LEFT]->SetPosition(vThornPos);
			m_pThorn2[m_iCurrentTwoIndex[LEFT]][LEFT]->SetRotationXYZ(_float3(iRandomAngleX, 0.f, iRandomAngleZ));
			m_pThorn2[m_iCurrentTwoIndex[LEFT]][LEFT]->SetupScaleUpStart(max((iRandom + m_iCurrentOneIndex[LEFT]) * 0.4f, 4.f));
		}

		if (m_iCurrentOneIndex[LEFT] < 24 && m_pThorn1[m_iCurrentOneIndex[LEFT]][LEFT]->IsScaleFinish())
			m_iCurrentOneIndex[LEFT]++;

		if (m_iCurrentTwoIndex[LEFT] < 24 && m_pThorn2[m_iCurrentTwoIndex[LEFT]][LEFT]->IsScaleFinish())
			m_iCurrentTwoIndex[LEFT]++;

		if (m_iCurrentOneIndex[LEFT] == 24 && m_iCurrentTwoIndex[LEFT] == 24)
		{
			m_iCurrentOneIndex[LEFT] = 0;
			m_iCurrentTwoIndex[LEFT] = 0;
			m_bLineSkillStart[LEFT] = false;
			m_bLineSkillErase[LEFT] = true;
			m_bCheckDir[LEFT] = false;

			//콜라이더 및 콜라이더 트렌스폼
			m_pColliderTransform[LEFT]->Set_State(CTransform::STATE_POSITION, transform->Get_State(CTransform::STATE_POSITION));
			m_pColliderLine[LEFT]->SetActive(true);

		}
	}

	//정면
	if (m_bLineSkillStart[FRONT])
	{
		//정면 Coll
		{
			_vector vFrontCollPos = m_pColliderTransform[FRONT]->Get_State(CTransform::STATE_POSITION) + XMLoadFloat3(&m_vFrontDir) * m_fLineCollSpeed * TimeDelta;
			m_pColliderTransform[FRONT]->Set_State(CTransform::STATE_POSITION, vFrontCollPos);
		}

		if (!m_pThorn1[m_iCurrentOneIndex[FRONT]][FRONT]->IsRender())
		{
			int iRandomPos = rand() % 3 - 1;

			int iRandom = rand() % 3 + 1;
			int iRandomAngleX = rand() % 90 - 45;
			int iRandomAngleZ = rand() % 90 - 45;

			_vector vThornPos;
			vThornPos = vBossPos + XMLoadFloat3(&m_vFrontDir) * m_iCurrentOneIndex[FRONT] * 0.5f;

			m_pThorn1[m_iCurrentOneIndex[FRONT]][FRONT]->SetPosition(vThornPos);
			m_pThorn1[m_iCurrentOneIndex[FRONT]][FRONT]->SetRotationXYZ(_float3(iRandomAngleX, 0.f, iRandomAngleZ));
			m_pThorn1[m_iCurrentOneIndex[FRONT]][FRONT]->SetupScaleUpStart(max((iRandom + m_iCurrentOneIndex[FRONT]) * 0.4f, 4.f));
		}

		if (!m_pThorn2[m_iCurrentTwoIndex[FRONT]][FRONT]->IsRender())
		{
			int iRandomPos = rand() % 3 - 1;

			int iRandom = rand() % 3 + 1;
			int iRandomAngleX = rand() % 90 - 45;
			int iRandomAngleZ = rand() % 90 - 45;

			_vector vThornPos;
			vThornPos = vBossPos + XMLoadFloat3(&m_vFrontDir) * m_iCurrentTwoIndex[FRONT] * 0.5f;

			m_pThorn2[m_iCurrentTwoIndex[FRONT]][FRONT]->SetPosition(vThornPos);
			m_pThorn2[m_iCurrentTwoIndex[FRONT]][FRONT]->SetRotationXYZ(_float3(iRandomAngleX, 0.f, iRandomAngleZ));
			m_pThorn2[m_iCurrentTwoIndex[FRONT]][FRONT]->SetupScaleUpStart(max((iRandom + m_iCurrentOneIndex[FRONT]) * 0.4f, 4.f));
		}

		if (m_iCurrentOneIndex[FRONT] < 24 && m_pThorn1[m_iCurrentOneIndex[FRONT]][FRONT]->IsScaleFinish())
			m_iCurrentOneIndex[FRONT]++;

		if (m_iCurrentTwoIndex[FRONT] < 24 && m_pThorn2[m_iCurrentTwoIndex[FRONT]][FRONT]->IsScaleFinish())
			m_iCurrentTwoIndex[FRONT]++;

		if (m_iCurrentOneIndex[FRONT] == 24 && m_iCurrentTwoIndex[FRONT] == 24)
		{
			m_iCurrentOneIndex[FRONT] = 0;
			m_iCurrentTwoIndex[FRONT] = 0;
			m_bLineSkillStart[FRONT] = false;
			m_bLineSkillErase[FRONT] = true;
			m_bCheckDir[FRONT] = false;

			//콜라이더 및 콜라이더 트렌스폼
			m_pColliderTransform[FRONT]->Set_State(CTransform::STATE_POSITION, transform->Get_State(CTransform::STATE_POSITION));
			m_pColliderLine[FRONT]->SetActive(false);
		}
	}

	//오른쪽
	if (m_bLineSkillStart[RIGHT])
	{
		//우측 Coll
		{
			_vector vRightCollPos = m_pColliderTransform[RIGHT]->Get_State(CTransform::STATE_POSITION) + XMLoadFloat3(&m_vRightDir) * m_fLineCollSpeed * TimeDelta;
			m_pColliderTransform[RIGHT]->Set_State(CTransform::STATE_POSITION, vRightCollPos);
		}

		if (!m_pThorn1[m_iCurrentOneIndex[RIGHT]][RIGHT]->IsRender())
		{
			int iRandomPos = rand() % 3 - 1;

			int iRandom = rand() % 3 + 1;
			int iRandomAngleX = rand() % 90 - 45;
			int iRandomAngleZ = rand() % 90 - 45;

			_vector vThornPos;
			vThornPos = vBossPos + XMLoadFloat3(&m_vRightDir) * m_iCurrentOneIndex[RIGHT] * 0.5f;

			m_pThorn1[m_iCurrentOneIndex[RIGHT]][RIGHT]->SetPosition(vThornPos);
			m_pThorn1[m_iCurrentOneIndex[RIGHT]][RIGHT]->SetRotationXYZ(_float3(iRandomAngleX, 0.f, iRandomAngleZ));
			m_pThorn1[m_iCurrentOneIndex[RIGHT]][RIGHT]->SetupScaleUpStart(max((iRandom + m_iCurrentOneIndex[RIGHT]) * 0.4f, 4.f));
		}

		if (!m_pThorn2[m_iCurrentTwoIndex[RIGHT]][RIGHT]->IsRender())
		{
			int iRandomPos = rand() % 3 - 1;

			int iRandom = rand() % 3 + 1;
			int iRandomAngleX = rand() % 90 - 45;
			int iRandomAngleZ = rand() % 90 - 45;

			_vector vThornPos;
			vThornPos = vBossPos + XMLoadFloat3(&m_vRightDir) * m_iCurrentTwoIndex[RIGHT] * 0.5f;

			m_pThorn2[m_iCurrentTwoIndex[RIGHT]][RIGHT]->SetPosition(vThornPos);
			m_pThorn2[m_iCurrentTwoIndex[RIGHT]][RIGHT]->SetRotationXYZ(_float3(iRandomAngleX, 0.f, iRandomAngleZ));
			m_pThorn2[m_iCurrentTwoIndex[RIGHT]][RIGHT]->SetupScaleUpStart(max((iRandom + m_iCurrentOneIndex[RIGHT]) * 0.4f, 4.f));
		}

		if (m_iCurrentOneIndex[RIGHT] < 24 && m_pThorn1[m_iCurrentOneIndex[RIGHT]][RIGHT]->IsScaleFinish())
			m_iCurrentOneIndex[RIGHT]++;

		if (m_iCurrentTwoIndex[RIGHT] < 24 && m_pThorn2[m_iCurrentTwoIndex[RIGHT]][RIGHT]->IsScaleFinish())
			m_iCurrentTwoIndex[RIGHT]++;

		if (m_iCurrentOneIndex[RIGHT] == 24 && m_iCurrentTwoIndex[RIGHT] == 24)
		{
			m_iCurrentOneIndex[RIGHT] = 0;
			m_iCurrentTwoIndex[RIGHT] = 0;
			m_bLineSkillStart[RIGHT] = false;
			m_bLineSkillErase[RIGHT] = true;
			m_bCheckDir[RIGHT] = false;

			//콜라이더 및 콜라이더 트렌스폼
			m_pColliderTransform[RIGHT]->Set_State(CTransform::STATE_POSITION, transform->Get_State(CTransform::STATE_POSITION));
			m_pColliderLine[RIGHT]->SetActive(false);
		}
	}

	//모두 다 지우기
	if (m_bLineSkillErase[LEFT] && m_bLineSkillErase[FRONT] && m_bLineSkillErase[RIGHT])
	{
		m_bLineAttackCollActive = false;
		m_bLineAttackEraseEx = true;
	}

	if (m_bLineAttackEraseEx)
	{
		m_fEraseExAcc += TimeDelta;
		if (m_fEraseExAcc >= m_fEraseExDelay)
		{
			m_fEraseExAcc = 0.f;

			m_pThorn1[m_iEraseIndexEx][LEFT]->SetupScaleSmoothDownStart();
			m_pThorn2[m_iEraseIndexEx][LEFT]->SetupScaleSmoothDownStart();

			m_pThorn1[m_iEraseIndexEx][FRONT]->SetupScaleSmoothDownStart();
			m_pThorn2[m_iEraseIndexEx][FRONT]->SetupScaleSmoothDownStart();

			m_pThorn1[m_iEraseIndexEx][RIGHT]->SetupScaleSmoothDownStart();
			m_pThorn2[m_iEraseIndexEx][RIGHT]->SetupScaleSmoothDownStart();

			m_iEraseIndexEx++;
			if (m_iEraseIndexEx == 25)
			{
				m_bAttackable = true;

				m_iEraseIndexEx = 0;
				m_bBurrowable = true;
				m_bLineAttack = false;
				m_bUseLineSkill = false;
				m_bLineAttackEraseEx = false;

				m_bLineSkillErase[LEFT] = false;
				m_bLineSkillErase[FRONT] = false;
				m_bLineSkillErase[RIGHT] = false;
			}
		}
	}

}

void CBoss::LastAttack(_double TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	m_bAttackable = false;

	enum { LEFT, FRONT, RIGHT, BACK, LEFT_BACK, RIGHT_BACK, LINE_END };

	_vector vBossPos = transform->Get_State(CTransform::STATE_POSITION);

	//모두 다 생성하기전까지 회전
	_bool bRotationFinish = LookTarget(TimeDelta, 2.f);

	//회전이 종료됬을떄 라인공격 실행
	if (!m_bLastAttackBegin)
	{
		if (bRotationFinish)
		{
			SetupState(BOSS_CLIP::ATK11, CAnimation::TYPE::ONE, false);

			if (model->AnimationCompare(BOSS_CLIP::ATK11))
			{
				if (model->AnimationIsPreFinishCustom(0.3))
				{
				
					m_bLastAttackBegin = true;
					m_bLineSkillStart[LEFT] = true;
					m_bLineSkillStart[FRONT] = true;
					m_bLineSkillStart[RIGHT] = true;

					m_bLineSkillStart[BACK] = true;
					m_bLineSkillStart[LEFT_BACK] = true;
					m_bLineSkillStart[RIGHT_BACK] = true;

					m_bLineAttackExCollActive = true;

					_vector vPlayerPos = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);
					_vector vBossRight = transform->Get_State(CTransform::STATE_RIGHT);
					_vector vDir = XMVector3Normalize(vPlayerPos - vBossPos);
					_vector vLeftDir = XMVector3Normalize(vDir * 2.f - vBossRight);
					_vector vRightDir = XMVector3Normalize(vDir * 2.f + vBossRight);

					_vector vBackDir = XMVector3Normalize(vBossPos - vPlayerPos);
					_vector vLeftBackDir = XMVector3Normalize(vBackDir * 2.f - vBossRight);
					_vector vRightBackDir = XMVector3Normalize(vBackDir * 2.f + vBossRight);

					XMStoreFloat3(&m_vFrontDir, vDir);
					XMStoreFloat3(&m_vLeftDir, vLeftDir);
					XMStoreFloat3(&m_vRightDir, vRightDir);

					XMStoreFloat3(&m_vBackDir, vBackDir);
					XMStoreFloat3(&m_vLeftBackDir, vLeftBackDir);
					XMStoreFloat3(&m_vRightBackDir, vRightBackDir);

					//Coll Init
					for (int i = 0; i < 6; ++i)
					{
						m_pColliderTransform[i]->Set_State(CTransform::STATE_POSITION, vBossPos);
					}
				}
			}
		}
		else
			return;
	}

	//왼쪽
	if (m_bLineSkillStart[LEFT])
	{
		_vector vLeftCollPos = m_pColliderTransform[LEFT]->Get_State(CTransform::STATE_POSITION) + XMLoadFloat3(&m_vLeftDir) * m_fLineCollSpeed * TimeDelta;
		m_pColliderTransform[LEFT]->Set_State(CTransform::STATE_POSITION, vLeftCollPos);
		m_pColliderLine[LEFT]->SetActive(true);

		if (!m_pThorn1[m_iCurrentOneIndex[LEFT]][LEFT]->IsRender())
		{
			int iRandomPos = rand() % 3 - 1;

			int iRandom = rand() % 3 + 1;
			int iRandomAngleX = rand() % 90 - 45;
			int iRandomAngleZ = rand() % 90 - 45;

			_vector vThornPos;
			vThornPos = vBossPos + XMLoadFloat3(&m_vLeftDir) * m_iCurrentOneIndex[LEFT] * 0.5f;

			m_pThorn1[m_iCurrentOneIndex[LEFT]][LEFT]->SetPosition(vThornPos);
			m_pThorn1[m_iCurrentOneIndex[LEFT]][LEFT]->SetRotationXYZ(_float3(iRandomAngleX, 0.f, iRandomAngleZ));
			m_pThorn1[m_iCurrentOneIndex[LEFT]][LEFT]->SetupScaleUpStart(max((iRandom + m_iCurrentOneIndex[LEFT]) * 0.4f, 4.f));
		}

		if (!m_pThorn2[m_iCurrentTwoIndex[LEFT]][LEFT]->IsRender())
		{
			int iRandomPos = rand() % 3 - 1;

			int iRandom = rand() % 3 + 1;
			int iRandomAngleX = rand() % 90 - 45;
			int iRandomAngleZ = rand() % 90 - 45;

			_vector vThornPos;
			vThornPos = vBossPos + XMLoadFloat3(&m_vLeftDir) * m_iCurrentTwoIndex[LEFT] * 0.5f;

			m_pThorn2[m_iCurrentTwoIndex[LEFT]][LEFT]->SetPosition(vThornPos);
			m_pThorn2[m_iCurrentTwoIndex[LEFT]][LEFT]->SetRotationXYZ(_float3(iRandomAngleX, 0.f, iRandomAngleZ));
			m_pThorn2[m_iCurrentTwoIndex[LEFT]][LEFT]->SetupScaleUpStart(max((iRandom + m_iCurrentTwoIndex[LEFT]) * 0.4f, 4.f));
		}

		if (m_iCurrentOneIndex[LEFT] < 24 && m_pThorn1[m_iCurrentOneIndex[LEFT]][LEFT]->IsScaleFinish())
			m_iCurrentOneIndex[LEFT]++;

		if (m_iCurrentTwoIndex[LEFT] < 24 && m_pThorn2[m_iCurrentTwoIndex[LEFT]][LEFT]->IsScaleFinish())
			m_iCurrentTwoIndex[LEFT]++;

		if (m_iCurrentOneIndex[LEFT] == 24 && m_iCurrentTwoIndex[LEFT] == 24)
		{
			m_iCurrentOneIndex[LEFT] = 0;
			m_iCurrentTwoIndex[LEFT] = 0;
			m_bLineSkillStart[LEFT] = false;
			m_bLineSkillErase[LEFT] = true;
			m_bCheckDir[LEFT] = false;
			m_pColliderLine[LEFT]->SetActive(false);
		}
	}

	//정면
	if (m_bLineSkillStart[FRONT])
	{
		_vector vFrontCollPos = m_pColliderTransform[FRONT]->Get_State(CTransform::STATE_POSITION) + XMLoadFloat3(&m_vFrontDir) * m_fLineCollSpeed * TimeDelta;
		m_pColliderTransform[FRONT]->Set_State(CTransform::STATE_POSITION, vFrontCollPos);
		m_pColliderLine[FRONT]->SetActive(true);

		if (!m_pThorn1[m_iCurrentOneIndex[FRONT]][FRONT]->IsRender())
		{
			int iRandomPos = rand() % 3 - 1;

			int iRandom = rand() % 3 + 1;
			int iRandomAngleX = rand() % 90 - 45;
			int iRandomAngleZ = rand() % 90 - 45;

			_vector vThornPos;
			vThornPos = vBossPos + XMLoadFloat3(&m_vFrontDir) * m_iCurrentOneIndex[FRONT] * 0.5f;

			m_pThorn1[m_iCurrentOneIndex[FRONT]][FRONT]->SetPosition(vThornPos);
			m_pThorn1[m_iCurrentOneIndex[FRONT]][FRONT]->SetRotationXYZ(_float3(iRandomAngleX, 0.f, iRandomAngleZ));
			m_pThorn1[m_iCurrentOneIndex[FRONT]][FRONT]->SetupScaleUpStart(max((iRandom + m_iCurrentOneIndex[FRONT]) * 0.4f, 4.f));
		}

		if (!m_pThorn2[m_iCurrentTwoIndex[FRONT]][FRONT]->IsRender())
		{
			int iRandomPos = rand() % 3 - 1;

			int iRandom = rand() % 3 + 1;
			int iRandomAngleX = rand() % 90 - 45;
			int iRandomAngleZ = rand() % 90 - 45;

			_vector vThornPos;
			vThornPos = vBossPos + XMLoadFloat3(&m_vFrontDir) * m_iCurrentTwoIndex[FRONT] * 0.5f;

			m_pThorn2[m_iCurrentTwoIndex[FRONT]][FRONT]->SetPosition(vThornPos);
			m_pThorn2[m_iCurrentTwoIndex[FRONT]][FRONT]->SetRotationXYZ(_float3(iRandomAngleX, 0.f, iRandomAngleZ));
			m_pThorn2[m_iCurrentTwoIndex[FRONT]][FRONT]->SetupScaleUpStart(max((iRandom + m_iCurrentTwoIndex[FRONT]) * 0.4f, 4.f));
		}

		if (m_iCurrentOneIndex[FRONT] < 24 && m_pThorn1[m_iCurrentOneIndex[FRONT]][FRONT]->IsScaleFinish())
			m_iCurrentOneIndex[FRONT]++;

		if (m_iCurrentTwoIndex[FRONT] < 24 && m_pThorn2[m_iCurrentTwoIndex[FRONT]][FRONT]->IsScaleFinish())
			m_iCurrentTwoIndex[FRONT]++;

		if (m_iCurrentOneIndex[FRONT] == 24 && m_iCurrentTwoIndex[FRONT] == 24)
		{
			m_iCurrentOneIndex[FRONT] = 0;
			m_iCurrentTwoIndex[FRONT] = 0;
			m_bLineSkillStart[FRONT] = false;
			m_bLineSkillErase[FRONT] = true;
			m_bCheckDir[FRONT] = false;
			m_pColliderLine[FRONT]->SetActive(false);
		}
	}

	//오른쪽
	if (m_bLineSkillStart[RIGHT])
	{
		_vector vRightCollPos = m_pColliderTransform[RIGHT]->Get_State(CTransform::STATE_POSITION) + XMLoadFloat3(&m_vRightDir) * m_fLineCollSpeed * TimeDelta;
		m_pColliderTransform[RIGHT]->Set_State(CTransform::STATE_POSITION, vRightCollPos);
		m_pColliderLine[RIGHT]->SetActive(true);

		if (!m_pThorn1[m_iCurrentOneIndex[RIGHT]][RIGHT]->IsRender())
		{
			int iRandomPos = rand() % 3 - 1;

			int iRandom = rand() % 3 + 1;
			int iRandomAngleX = rand() % 90 - 45;
			int iRandomAngleZ = rand() % 90 - 45;

			_vector vThornPos;
			vThornPos = vBossPos + XMLoadFloat3(&m_vRightDir) * m_iCurrentOneIndex[RIGHT] * 0.5f;

			m_pThorn1[m_iCurrentOneIndex[RIGHT]][RIGHT]->SetPosition(vThornPos);
			m_pThorn1[m_iCurrentOneIndex[RIGHT]][RIGHT]->SetRotationXYZ(_float3(iRandomAngleX, 0.f, iRandomAngleZ));
			m_pThorn1[m_iCurrentOneIndex[RIGHT]][RIGHT]->SetupScaleUpStart(max((iRandom + m_iCurrentOneIndex[RIGHT]) * 0.4f, 4.f));
		}

		if (!m_pThorn2[m_iCurrentTwoIndex[RIGHT]][RIGHT]->IsRender())
		{
			int iRandomPos = rand() % 3 - 1;

			int iRandom = rand() % 3 + 1;
			int iRandomAngleX = rand() % 90 - 45;
			int iRandomAngleZ = rand() % 90 - 45;

			_vector vThornPos;
			vThornPos = vBossPos + XMLoadFloat3(&m_vRightDir) * m_iCurrentTwoIndex[RIGHT] * 0.5f;

			m_pThorn2[m_iCurrentTwoIndex[RIGHT]][RIGHT]->SetPosition(vThornPos);
			m_pThorn2[m_iCurrentTwoIndex[RIGHT]][RIGHT]->SetRotationXYZ(_float3(iRandomAngleX, 0.f, iRandomAngleZ));
			m_pThorn2[m_iCurrentTwoIndex[RIGHT]][RIGHT]->SetupScaleUpStart(max((iRandom + m_iCurrentTwoIndex[RIGHT]) * 0.4f, 4.f));
		}

		if (m_iCurrentOneIndex[RIGHT] < 24 && m_pThorn1[m_iCurrentOneIndex[RIGHT]][RIGHT]->IsScaleFinish())
			m_iCurrentOneIndex[RIGHT]++;

		if (m_iCurrentTwoIndex[RIGHT] < 24 && m_pThorn2[m_iCurrentTwoIndex[RIGHT]][RIGHT]->IsScaleFinish())
			m_iCurrentTwoIndex[RIGHT]++;

		if (m_iCurrentOneIndex[RIGHT] == 24 && m_iCurrentTwoIndex[RIGHT] == 24)
		{
			m_iCurrentOneIndex[RIGHT] = 0;
			m_iCurrentTwoIndex[RIGHT] = 0;
			m_bLineSkillStart[RIGHT] = false;
			m_bLineSkillErase[RIGHT] = true;
			m_bCheckDir[RIGHT] = false;
			m_pColliderLine[RIGHT]->SetActive(false);
		}
	}

	//왼쪽 뒤
	if (m_bLineSkillStart[LEFT_BACK])
	{
		_vector vBackCollPos = m_pColliderTransform[LEFT_BACK]->Get_State(CTransform::STATE_POSITION) + XMLoadFloat3(&m_vLeftBackDir) * m_fLineCollSpeed * TimeDelta;
		m_pColliderTransform[LEFT_BACK]->Set_State(CTransform::STATE_POSITION, vBackCollPos);
		m_pColliderLine[LEFT_BACK]->SetActive(true);

		if (!m_pThorn1[m_iCurrentOneIndex[LEFT_BACK]][LEFT_BACK]->IsRender())
		{
			int iRandomPos = rand() % 3 - 1;

			int iRandom = rand() % 3 + 1;
			int iRandomAngleX = rand() % 90 - 45;
			int iRandomAngleZ = rand() % 90 - 45;

			_vector vThornPos;
			vThornPos = vBossPos + XMLoadFloat3(&m_vLeftBackDir) * m_iCurrentOneIndex[LEFT_BACK] * 0.5f;

			m_pThorn1[m_iCurrentOneIndex[LEFT_BACK]][LEFT_BACK]->SetPosition(vThornPos);
			m_pThorn1[m_iCurrentOneIndex[LEFT_BACK]][LEFT_BACK]->SetRotationXYZ(_float3(iRandomAngleX, 0.f, iRandomAngleZ));
			m_pThorn1[m_iCurrentOneIndex[LEFT_BACK]][LEFT_BACK]->SetupScaleUpStart(max((iRandom + m_iCurrentOneIndex[LEFT_BACK]) * 0.4f, 4.f));
		}

		if (!m_pThorn2[m_iCurrentTwoIndex[LEFT_BACK]][LEFT_BACK]->IsRender())
		{
			int iRandomPos = rand() % 3 - 1;

			int iRandom = rand() % 3 + 1;
			int iRandomAngleX = rand() % 90 - 45;
			int iRandomAngleZ = rand() % 90 - 45;

			_vector vThornPos;
			vThornPos = vBossPos + XMLoadFloat3(&m_vLeftBackDir) * m_iCurrentTwoIndex[FRONT] * 0.5f;

			m_pThorn2[m_iCurrentTwoIndex[LEFT_BACK]][LEFT_BACK]->SetPosition(vThornPos);
			m_pThorn2[m_iCurrentTwoIndex[LEFT_BACK]][LEFT_BACK]->SetRotationXYZ(_float3(iRandomAngleX, 0.f, iRandomAngleZ));
			m_pThorn2[m_iCurrentTwoIndex[LEFT_BACK]][LEFT_BACK]->SetupScaleUpStart(max((iRandom + m_iCurrentTwoIndex[LEFT_BACK]) * 0.4f, 4.f));
		}

		if (m_iCurrentOneIndex[LEFT_BACK] < 24 && m_pThorn1[m_iCurrentOneIndex[LEFT_BACK]][LEFT_BACK]->IsScaleFinish())
			m_iCurrentOneIndex[LEFT_BACK]++;

		if (m_iCurrentTwoIndex[LEFT_BACK] < 24 && m_pThorn2[m_iCurrentTwoIndex[LEFT_BACK]][LEFT_BACK]->IsScaleFinish())
			m_iCurrentTwoIndex[LEFT_BACK]++;

		if (m_iCurrentOneIndex[LEFT_BACK] == 24 && m_iCurrentTwoIndex[LEFT_BACK] == 24)
		{
			m_iCurrentOneIndex[LEFT_BACK] = 0;
			m_iCurrentTwoIndex[LEFT_BACK] = 0;
			m_bLineSkillStart[LEFT_BACK] = false;
			m_bLineSkillErase[LEFT_BACK] = true;
			m_bCheckDir[LEFT_BACK] = false;
			m_pColliderLine[LEFT_BACK]->SetActive(false);
		}
	}

	//뒤
	if (m_bLineSkillStart[BACK])
	{
		_vector vBackCollPos = m_pColliderTransform[BACK]->Get_State(CTransform::STATE_POSITION) + XMLoadFloat3(&m_vBackDir) * m_fLineCollSpeed * TimeDelta;
		m_pColliderTransform[BACK]->Set_State(CTransform::STATE_POSITION, vBackCollPos);
		m_pColliderLine[BACK]->SetActive(true);

		if (!m_pThorn1[m_iCurrentOneIndex[BACK]][BACK]->IsRender())
		{
			int iRandomPos = rand() % 3 - 1;

			int iRandom = rand() % 3 + 1;
			int iRandomAngleX = rand() % 90 - 45;
			int iRandomAngleZ = rand() % 90 - 45;

			_vector vThornPos;
			vThornPos = vBossPos + XMLoadFloat3(&m_vBackDir) * m_iCurrentOneIndex[BACK] * 0.5f;

			m_pThorn1[m_iCurrentOneIndex[BACK]][BACK]->SetPosition(vThornPos);
			m_pThorn1[m_iCurrentOneIndex[BACK]][BACK]->SetRotationXYZ(_float3(iRandomAngleX, 0.f, iRandomAngleZ));
			m_pThorn1[m_iCurrentOneIndex[BACK]][BACK]->SetupScaleUpStart(max((iRandom + m_iCurrentOneIndex[BACK]) * 0.4f, 4.f));
		}

		if (!m_pThorn2[m_iCurrentTwoIndex[BACK]][BACK]->IsRender())
		{
			int iRandomPos = rand() % 3 - 1;

			int iRandom = rand() % 3 + 1;
			int iRandomAngleX = rand() % 90 - 45;
			int iRandomAngleZ = rand() % 90 - 45;

			_vector vThornPos;
			vThornPos = vBossPos + XMLoadFloat3(&m_vBackDir)* m_iCurrentTwoIndex[BACK] * 0.5f;

			m_pThorn2[m_iCurrentTwoIndex[BACK]][BACK]->SetPosition(vThornPos);
			m_pThorn2[m_iCurrentTwoIndex[BACK]][BACK]->SetRotationXYZ(_float3(iRandomAngleX, 0.f, iRandomAngleZ));
			m_pThorn2[m_iCurrentTwoIndex[BACK]][BACK]->SetupScaleUpStart(max((iRandom + m_iCurrentTwoIndex[BACK]) * 0.4f, 4.f));
		}

		if (m_iCurrentOneIndex[BACK] < 24 && m_pThorn1[m_iCurrentOneIndex[BACK]][BACK]->IsScaleFinish())
			m_iCurrentOneIndex[BACK]++;

		if (m_iCurrentTwoIndex[BACK] < 24 && m_pThorn2[m_iCurrentTwoIndex[BACK]][BACK]->IsScaleFinish())
			m_iCurrentTwoIndex[BACK]++;

		if (m_iCurrentOneIndex[BACK] == 24 && m_iCurrentTwoIndex[BACK] == 24)
		{
			m_iCurrentOneIndex[BACK] = 0;
			m_iCurrentTwoIndex[BACK] = 0;
			m_bLineSkillStart[BACK] = false;
			m_bLineSkillErase[BACK] = true;
			m_bCheckDir[BACK] = false;

			m_pColliderLine[BACK]->SetActive(false);
		}
	}

	//오른쪽 뒤
	if (m_bLineSkillStart[RIGHT_BACK])
	{
		_vector vBackCollPos = m_pColliderTransform[RIGHT_BACK]->Get_State(CTransform::STATE_POSITION) + XMLoadFloat3(&m_vRightBackDir) * m_fLineCollSpeed * TimeDelta;
		m_pColliderTransform[RIGHT_BACK]->Set_State(CTransform::STATE_POSITION, vBackCollPos);
		m_pColliderLine[RIGHT_BACK]->SetActive(true);

		if (!m_pThorn1[m_iCurrentOneIndex[RIGHT_BACK]][RIGHT_BACK]->IsRender())
		{
			int iRandomPos = rand() % 3 - 1;

			int iRandom = rand() % 3 + 1;
			int iRandomAngleX = rand() % 90 - 45;
			int iRandomAngleZ = rand() % 90 - 45;

			_vector vThornPos;
			vThornPos = vBossPos + XMLoadFloat3(&m_vRightBackDir) * m_iCurrentOneIndex[RIGHT_BACK] * 0.5f;

			m_pThorn1[m_iCurrentOneIndex[RIGHT_BACK]][RIGHT_BACK]->SetPosition(vThornPos);
			m_pThorn1[m_iCurrentOneIndex[RIGHT_BACK]][RIGHT_BACK]->SetRotationXYZ(_float3(iRandomAngleX, 0.f, iRandomAngleZ));
			m_pThorn1[m_iCurrentOneIndex[RIGHT_BACK]][RIGHT_BACK]->SetupScaleUpStart(max((iRandom + m_iCurrentOneIndex[RIGHT_BACK]) * 0.4f, 4.f));
		}

		if (!m_pThorn2[m_iCurrentTwoIndex[RIGHT_BACK]][RIGHT_BACK]->IsRender())
		{
			int iRandomPos = rand() % 3 - 1;

			int iRandom = rand() % 3 + 1;
			int iRandomAngleX = rand() % 90 - 45;
			int iRandomAngleZ = rand() % 90 - 45;

			_vector vThornPos;
			vThornPos = vBossPos + XMLoadFloat3(&m_vRightBackDir)* m_iCurrentTwoIndex[RIGHT_BACK] * 0.5f;

			m_pThorn2[m_iCurrentTwoIndex[RIGHT_BACK]][RIGHT_BACK]->SetPosition(vThornPos);
			m_pThorn2[m_iCurrentTwoIndex[RIGHT_BACK]][RIGHT_BACK]->SetRotationXYZ(_float3(iRandomAngleX, 0.f, iRandomAngleZ));
			m_pThorn2[m_iCurrentTwoIndex[RIGHT_BACK]][RIGHT_BACK]->SetupScaleUpStart(max((iRandom + m_iCurrentTwoIndex[RIGHT_BACK]) * 0.4f, 4.f));
		}

		if (m_iCurrentOneIndex[RIGHT_BACK] < 24 && m_pThorn1[m_iCurrentOneIndex[RIGHT_BACK]][RIGHT_BACK]->IsScaleFinish())
			m_iCurrentOneIndex[RIGHT_BACK]++;

		if (m_iCurrentTwoIndex[RIGHT_BACK] < 24 && m_pThorn2[m_iCurrentTwoIndex[RIGHT_BACK]][RIGHT_BACK]->IsScaleFinish())
			m_iCurrentTwoIndex[RIGHT_BACK]++;

		if (m_iCurrentOneIndex[RIGHT_BACK] == 24 && m_iCurrentTwoIndex[RIGHT_BACK] == 24)
		{
			m_iCurrentOneIndex[RIGHT_BACK] = 0;
			m_iCurrentTwoIndex[RIGHT_BACK] = 0;
			m_bLineSkillStart[RIGHT_BACK] = false;
			m_bLineSkillErase[RIGHT_BACK] = true;
			m_bCheckDir[RIGHT_BACK] = false;

			m_pColliderLine[BACK_RIGHT]->SetActive(false);
		}
	}

	if (m_bLineSkillErase[LEFT] && m_bLineSkillErase[FRONT] && m_bLineSkillErase[RIGHT] &&
		m_bLineSkillErase[LEFT_BACK] && m_bLineSkillErase[BACK] && m_bLineSkillErase[RIGHT_BACK])
	{
		m_bLineAttackExCollActive = false;

		if (m_iEraseIndexEx < 25)
		{
			m_fLastAttackEraseAcc += TimeDelta;
			if (m_fLastAttackEraseAcc >= m_fLastAttackEraseDelay)
			{
				m_fLastAttackEraseAcc = 0.f;

				m_pThorn1[m_iEraseIndexEx][LEFT]->SetupScaleSmoothDownStart();
				m_pThorn2[m_iEraseIndexEx][LEFT]->SetupScaleSmoothDownStart();

				m_pThorn1[m_iEraseIndexEx][FRONT]->SetupScaleSmoothDownStart();
				m_pThorn2[m_iEraseIndexEx][FRONT]->SetupScaleSmoothDownStart();

				m_pThorn1[m_iEraseIndexEx][RIGHT]->SetupScaleSmoothDownStart();
				m_pThorn2[m_iEraseIndexEx][RIGHT]->SetupScaleSmoothDownStart();

				m_pThorn1[m_iEraseIndexEx][LEFT_BACK]->SetupScaleSmoothDownStart();
				m_pThorn2[m_iEraseIndexEx][LEFT_BACK]->SetupScaleSmoothDownStart();

				m_pThorn1[m_iEraseIndexEx][RIGHT_BACK]->SetupScaleSmoothDownStart();
				m_pThorn2[m_iEraseIndexEx][RIGHT_BACK]->SetupScaleSmoothDownStart();

				m_pThorn1[m_iEraseIndexEx][BACK]->SetupScaleSmoothDownStart();
				m_pThorn2[m_iEraseIndexEx][BACK]->SetupScaleSmoothDownStart();

				m_iEraseIndexEx++;
				if (m_iEraseIndexEx == 25)
				{
					m_bLineExAnimStart = false;
					m_bLineExAnimSetup = false;
					m_bLastAttack = false;
					m_bAttackable = true;
					m_iEraseIndexEx = 0;
					m_bBurrowable = true;
					m_bUseLineSkill = false;
					m_bLineAttackEraseEx = false;

					m_bLineSkillErase[LEFT] = false;
					m_bLineSkillErase[FRONT] = false;
					m_bLineSkillErase[RIGHT] = false;

					m_bLineSkillErase[RIGHT_BACK] = false;
					m_bLineSkillErase[LEFT_BACK] = false;
					m_bLineSkillErase[BACK] = false;
				}
			}
		}
	}
	
}

_float CBoss::GetLengthFromPlayer() const
{
	_float fLength = XMVectorGetX(XMVector3Length(m_pPlayerTransform->Get_State(CTransform::STATE_POSITION) - transform->Get_State(CTransform::STATE_POSITION)));
	return fLength;
}

_bool CBoss::LookTarget(_double TimeDelta, _float fRotationSpeed)
{
	_vector vRight = XMVector3Normalize(transform->Get_State(CTransform::STATE_RIGHT));
	_vector vDir = XMVector3Normalize(m_pPlayerTransform->Get_State(CTransform::STATE_POSITION) - transform->Get_State(CTransform::STATE_POSITION));

	_float fRadianAngle = XMVectorGetX(XMVector3Dot(vRight, vDir));
	if (isnan(fRadianAngle))
		fRadianAngle = 0.f;

	_float fDirCheck = XMVectorGetY((vRight, vDir));

	if(fDirCheck >= 0.f)
		transform->Rotate(VECTOR_UP, fRadianAngle * (TimeDelta * fRotationSpeed));
	else
		transform->Rotate(VECTOR_UP, -fRadianAngle * (TimeDelta * fRotationSpeed));
	
	if (fRadianAngle < 0.05f)
		return true;
	else
		return false;
}

void CBoss::DefaultAnimation(_double TimeDelta)
{
	if (m_bEvolution)
		return;

	Spawn();
	if (m_bSpawn)
	{
		if (model->AnimationCompare(BOSS_CLIP::BORN))
		{
			if (model->AnimationIsFinishEx())
				m_bBrrow = false;
		}
	}

	//플레이어와 가깝고 다른용무가 없다
	if (m_bSpawn && !m_bBrrow &&
		CloseToPlayer())
	{
		m_bTooFar = false;
		if (model->AnimationCompare(BOSS_CLIP::STAND2))
		{
			m_fAttackAcc += TimeDelta;
			if (m_fAttackAcc >= m_fAttackAccTime)
			{
				m_fAttackAcc = 0.f;
				m_bAttackable = true;

				switch (m_iAttackCount)
				{
				case 0:
					m_bUseLineSkill = true;
					break;
				case 1:
					m_bUseMissile1 = true;
					m_bMissileStart = true;
					break;
				case 2:
					if (GetLengthFromPlayer() >= 6.f)
						m_bMoveForward = true;
					else
						m_bMoveBackward = true;
					break;
				case 3:
					m_bUseMissile1 = true;
					m_bMissileStart = true;
					break;
				case 4:
					m_bUseLineSkill = true;
					break;
				case 5:
					m_bUseMissile1 = true;
					m_bMissileStart = true;
					break;
				case 6:
					m_bBurrowStart = true;
					break;
				case 7:
					m_bUseMissile1 = true;
					m_bMissileStart = true;
					break;
				default:
					break;
				}

				if (m_iAttackCount > 7)
				{
					m_iAttackCount = 0;
				}
				else
					m_iAttackCount++;
			}
		}
	}

	if (m_bSpawn && !m_bBrrow &&
		!CloseToPlayer())
	{
		m_fFarAcc += TimeDelta;
		if (m_fFarAcc >= m_fFarTime)
		{
			m_fFarAcc = 0.f;
			m_bTooFar = true;

			if (GetLengthFromPlayer() >= 18.f)
			{
				m_bBurrowStart = true;
			}

			//어택 초기화
			m_iAttackCount = 0;
			m_fAttackAcc = m_fAttackAccTime;
		}
	}

	if (m_bTooFar && !m_bBurrowStart)
	{
		if (!model->AnimationCompare(BOSS_CLIP::ATK_FOWARD))
		{
			if (model->AnimationIsFinishEx())
				m_bMoveForward = true;
		}
		else if (model->AnimationCompare(BOSS_CLIP::STAND2))
			m_bMoveForward = true;
	}

	if (m_bMoveBackward)
		MoveBackword(TimeDelta);

	if (m_bMoveForward)
		MoveForward(TimeDelta);

	if (m_bUseMissile1)
		Missile1(TimeDelta);

}

void CBoss::EvolutionAnimation(_double TimeDelta)
{
	if (m_bAttackable)
	{
		m_fAttackAcc += TimeDelta;
		if (m_fAttackAcc >= 3.0f)
		{
			m_bAttack = true;
			m_fAttackAcc = 0.f;

			m_iAttackCountEx++;
			if (m_iAttackCountEx > 3)
				m_iAttackCountEx = 0;
		}
	}

	if (m_bAttack)
	{
		m_bAttack = false;
		switch (m_iAttackCountEx)
		{
		case 0:
			m_bUseLineSkill = true;
			break;
		case 1:
			m_bCloseAttackExStart = true;
			break;
		case 2:
			m_bUseMissile1 = true;
			m_bMissileStart = true;
			break;
		case 3:
			m_bLastAttackBegin = false;
			m_bLastAttack = true;
			break;
		}
	}
}

void CBoss::AnimationController(_double TimeDelta)
{
	if (m_bCloseAttackExStart)
		ColseAttack2(TimeDelta);

	if (m_bUseLineSkill)
		LineSkill2(TimeDelta);

	if (m_bUseMissile1)
		Missile1(TimeDelta);

	if (m_bLastAttack)
		LastAttack(TimeDelta);


	if (m_bDie)
	{
		if (model->AnimationCompare(BOSS_CLIP::DEATH))
		{
			m_pCamera->StartShake(5.f, 80.f 0.8f);
		}

		if (model->AnimationIsFinishEx())
		{
			SetupState(BOSS_CLIP::DEATH, CAnimation::TYPE::ONE, false);
		}

	}
	else
	{
		if (!m_bEvolution)
			DefaultAnimation(TimeDelta);
		else
			EvolutionAnimation(TimeDelta);

		//모션이 끝났다면 아이들 모션
		if (!m_bBrrow)
		{
			if (m_bEvolution)
			{
				if (!model->AnimationCompare(BOSS_CLIP::STANDEX))
				{
					if (model->AnimationIsFinishEx())
						SetupState(BOSS_CLIP::STANDEX, CAnimation::TYPE::LOOP, false);
				}
			}
			else
			{
				if (!model->AnimationCompare(BOSS_CLIP::STAND2))
				{
					if (model->AnimationIsFinishEx())
						SetupState(BOSS_CLIP::STAND2, CAnimation::TYPE::LOOP, false);
				}
			}
		}

	}
	
	//바디 올리기
	_float4x4 vBossWorld = transform->Get_WorldMatrix();
	vBossWorld._42 = vBossWorld._42 + 0.43f;
	m_pBodyTransform->Set_WorldMatrix(vBossWorld);

	model->Setup_Animation(m_eAnimState.eCurAnimationClip, m_eAnimState.eAnimType, m_eAnimState.bLerp);
	model->Play_Animation(TimeDelta, transform, 0.2, false, "BoneHip");
}

void CBoss::SetupColliders()
{
	collider->Update(XMLoadFloat4x4(&transform->Get_WorldMatrix()));
	_float4x4 CollMatrix = transform->Get_WorldMatrix();
	CollMatrix._42 = CollMatrix._42 - 0.5f;
	m_pOverlapCollider->Update(XMLoadFloat4x4(&CollMatrix));

	if (m_pAppManager->IsFreeze())
	{
		for (int i = 0; i < 6; ++i)
			m_pColliderLine[i]->SetActive(false);
	}

	if (m_bLineAttackExCollActive)
	{
		//LineAttack 2 Coll
		for (int i = 0; i < 6; ++i)
			m_pColliderLine[i]->Update(XMLoadFloat4x4(&m_pColliderTransform[i]->Get_WorldMatrix()));
	}
	else if (m_bLineAttackCollActive)
	{
		//LineAttack 1 Coll
		for (int i = 0; i < 3; ++i)
			m_pColliderLine[i]->Update(XMLoadFloat4x4(&m_pColliderTransform[i]->Get_WorldMatrix()));
	}

	//CloseAttack 1 Coll
	if (m_bCloseAttackCollActive)
	{
		for (int i = 0; i < 3; ++i)
			m_pCloseAttack[i]->Update(XMLoadFloat4x4(&m_pColliderCloseTransform[i]->Get_WorldMatrix()));
	}
}

_float4 CBoss::GetPosition()
{
	if (transform == nullptr)
		return _float4();

	if (IsDestroy())
		return _float4();

	_float4 vPos;
	XMStoreFloat4(&vPos, transform->Get_State(CTransform::STATE_POSITION));

	return vPos;
}

_float CBoss::GetLengthFromCamera()
{
	CPipeLine* pPipeLine = CPipeLine::GetInstance();

	_vector vPos = transform->Get_State(CTransform::STATE_POSITION);
	_vector vCamPos = XMLoadFloat4(&pPipeLine->Get_CamPosition());

	_float fLength = XMVectorGetX(XMVector3Length(vPos - vCamPos));

	return fLength;
}

//void CBoss::OnCollisionEnter(CCollider * src, CCollider * dest)
//{
//}
//
//void CBoss::OnCollisionStay(CCollider * src, CCollider * dest)
//{
//	CPlayerCamera* pCamera = dynamic_cast<CPlayerCamera*>(dest->GetOwner());
//	if (src->Compare(m_pOverlapCollider) && pCamera && dest->Compare(m_pCamera->GetCollider()))
//	{
//		m_bAlpha = true;
//	}
//}
//
//void CBoss::OnCollisionExit(CCollider * src, CCollider * dest)
//{
//
//}

CBoss * CBoss::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBoss*	pInstance = new CBoss(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CBoss");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CBoss::Clone(void * pArg)
{
	CBoss* pInstance = new CBoss(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CBoss");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBoss::Free()
{
	__super::Free();

	for (int j = 0; j < 6; j++)
	{
		for (int i = 0; i < 25; ++i)
		{
			Safe_Release(m_pThorn1[i][j]);
			Safe_Release(m_pThorn2[i][j]);
		}
	}

	for (int i = 0; i < 3; ++i)
	{
		Safe_Release(m_pThornMissileLeft[i]);
		Safe_Release(m_pThornMissileMiddle[i]);
		Safe_Release(m_pThornMissileRight[i]);
	}

	for (int i = 0; i < 9; ++i)
	{
		Safe_Release(m_pThornClose[i]);

		Safe_Release(m_pThornCloseRightFront[i]);
		Safe_Release(m_pThornCloseLeftFront[i]);
		Safe_Release(m_pThornCloseFront[i]);
		Safe_Release(m_pThornCloseRight[i]);
		Safe_Release(m_pThornCloseLeft[i]);
	}

	for (int i = 0; i < 3; ++i)
	{
		Safe_Release(m_pCloseAttack[i]);
	}

	for (int i = 0; i < 6; ++i)
	{
		Safe_Release(m_pColliderLine[i]);
	}

	Safe_Release(m_pBodyTransform);
}
