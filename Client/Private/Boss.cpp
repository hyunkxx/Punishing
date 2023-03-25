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

	for (int j = 0; j < 3; j++)
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

	return S_OK;
}

void CBoss::Tick(_double TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	m_bAlpha = false;
	TimeDelta = Freeze(TimeDelta);
	
	//LookTarget(TimeDelta, 0.2f);
	if(pGameInstance->Input_KeyState_Custom(DIK_T) == KEY_STATE::TAP)
		Missile1(TimeDelta);

	LineSkill(TimeDelta, m_iCurrentLine);

	pGameInstance->AddCollider(collider);
	pGameInstance->AddCollider(m_pOverlapCollider);
}

void CBoss::LateTick(_double TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	TimeDelta = Freeze(TimeDelta);

	//각종 애니미에션 설정 및 제어
	AnimationController(TimeDelta);

	//콜라이더 피직스 업데이트에 전달
	SetupColliders();

	if (nullptr != renderer && m_bSpawn)
		renderer->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CBoss::Render()
{
	if (FAILED(SetupShaderResources()))
		return E_FAIL;

	_uint MeshCount = model->Get_MeshCount();
	for (_uint i = 0; i < MeshCount; ++i)
	{
		if (i == 0)
		{
			if (FAILED(m_pBodyTransform->Setup_ShaderResource(shader, "g_WorldMatrix")))
				return E_FAIL;
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
	if (!m_bSpawn && GetLengthFromPlayer() <= m_fNearCheckRange)
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
				}
			}
		}
	}
	else
	{
		m_bLineAttack = false;
	}

	if (m_bLineSkillStart[0])
	{
		_vector vBossPos = transform->Get_State(CTransform::STATE_POSITION);
		if (!m_bCheckDir[0])
		{
			_vector vDir = XMVector3Normalize(m_pPlayerTransform->Get_State(CTransform::STATE_POSITION) - transform->Get_State(CTransform::STATE_POSITION));
			XMStoreFloat3(&m_vLineOneDir[0], vDir);
			m_bCheckDir[0] = true;
		}

		if (!m_pThorn1[m_iCurrentOneIndex[0]][LINE_ONE]->IsRender())
		{
			int iRandomPos = rand() % 3 - 1;

			int iRandom = rand() % 3 + 1;
			int iRandomAngleX = rand() % 120 - 60;
			int iRandomAngleZ = rand() % 120 - 60;

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
			int iRandomAngleX = rand() % 120 - 60;
			int iRandomAngleZ = rand() % 120 - 60;

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
		}
	}
	else if (m_bLineSkillStart[1])
	{
		_vector vBossPos = transform->Get_State(CTransform::STATE_POSITION);
		if (!m_bCheckDir[1])
		{
			_vector vDir = XMVector3Normalize(m_pPlayerTransform->Get_State(CTransform::STATE_POSITION) - transform->Get_State(CTransform::STATE_POSITION));
			XMStoreFloat3(&m_vLineOneDir[1], vDir);
			m_bCheckDir[1] = true;
		}

		if (!m_pThorn1[m_iCurrentOneIndex[1]][LINE_TWO]->IsRender())
		{
			int iRandomPos = rand() % 3 - 1;

			int iRandom = rand() % 3 + 1;
			int iRandomAngleX = rand() % 120 - 60;
			int iRandomAngleZ = rand() % 120 - 60;

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
			int iRandomAngleX = rand() % 120 - 60;
			int iRandomAngleZ = rand() % 120 - 60;

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
			m_bCheckDir[2] = true;
		}

		if (!m_pThorn1[m_iCurrentOneIndex[2]][LINE_TRE]->IsRender())
		{
			int iRandomPos = rand() % 3 - 1;

			int iRandom = rand() % 3 + 1;
			int iRandomAngleX = rand() % 120 - 60;
			int iRandomAngleZ = rand() % 120 - 60;

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
			int iRandomAngleX = rand() % 120 - 60;
			int iRandomAngleZ = rand() % 120 - 60;

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

		}
	}


	if (m_bLineSkillErase[0])
	{
		m_fEraseAcc[0] += TimeDelta;
		if (m_fEraseAcc[0] >= m_fEraseTime)
		{
			m_fEraseAcc[0] = 0.f;
			m_pThorn1[m_iEraseIndex[0]][0]->SetRender(false);
			m_pThorn2[m_iEraseIndex[0]][0]->SetRender(false);

			m_pThorn1[m_iEraseIndex[0]][0]->Reset();
			m_pThorn2[m_iEraseIndex[0]][0]->Reset();

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
			m_pThorn1[m_iEraseIndex[1]][1]->SetRender(false);
			m_pThorn2[m_iEraseIndex[1]][1]->SetRender(false);

			m_pThorn1[m_iEraseIndex[1]][1]->Reset();
			m_pThorn2[m_iEraseIndex[1]][1]->Reset();

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
			m_pThorn1[m_iEraseIndex[2]][2]->SetRender(false);
			m_pThorn2[m_iEraseIndex[2]][2]->SetRender(false);

			m_pThorn1[m_iEraseIndex[2]][2]->Reset();
			m_pThorn2[m_iEraseIndex[2]][2]->Reset();

			m_iEraseIndex[2]++;
		}

		if (m_iEraseIndex[2] == 25)
		{
			m_iEraseIndex[2] = 0;
			m_bLineSkillErase[2] = false;
		}
	}
}

void CBoss::Missile1(_double TimeDelta)
{
	_vector vBossPos = transform->Get_State(CTransform::STATE_POSITION);
	_vector vPlayerDir = XMVector3Normalize(m_pPlayerTransform->Get_State(CTransform::STATE_POSITION) - vBossPos);
	_vector vRight = XMVector3Cross(VECTOR_UP, vPlayerDir);

	LookTarget(TimeDelta, 0.5f);

	if(model->AnimationCompare(BOSS_CLIP::ATK3))
	{
		if(model->AnimationIsFinishEx())
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

	SetupState(BOSS_CLIP::ATK3, CAnimation::TYPE::ONE, false);

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

void CBoss::AnimationController(_double TimeDelta)
{
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
		if (model->AnimationCompare(BOSS_CLIP::STAND1) || model->AnimationCompare(BOSS_CLIP::STAND2))
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
					m_bMoveBackward = true;
					break;
				case 3:
					m_bUseMissile1 = true;
					m_bMissileStart = true;
					break;
				case 4:
					m_bMoveForward = true;
					break;
				default:
					break;
				}

				if (m_iAttackCount > 4)
					m_iAttackCount = 0;
				else
					m_iAttackCount++;
			}
		}
	}

	if (m_bMoveBackward)
		MoveBackword(TimeDelta);

	if (m_bMoveForward)
		MoveForward(TimeDelta);

	if (m_bUseMissile1)
		Missile1(TimeDelta);

	//모션이 끝났다면 아이들 모션
	if (!m_bBrrow)
	{
		if (!m_bEvolution)
		{
			if (!model->AnimationCompare(BOSS_CLIP::STAND2))
			{
				if (model->AnimationIsFinishEx())
					SetupState(BOSS_CLIP::STAND2, CAnimation::TYPE::LOOP, false);
			}
		}
		else
		{
			if (!model->AnimationCompare(BOSS_CLIP::STAND1))
			{
				if (model->AnimationIsFinishEx())
					SetupState(BOSS_CLIP::STAND1, CAnimation::TYPE::LOOP, false);
			}
		}
	}

	//바디 올리기
	_float4x4 vBossWorld = transform->Get_WorldMatrix();
	//vBossWorld._42 = vBossWorld._42 + 0.3f;
	m_pBodyTransform->Set_WorldMatrix(vBossWorld);

	model->Setup_Animation(m_eAnimState.eCurAnimationClip, m_eAnimState.eAnimType, m_eAnimState.bLerp);
	model->Play_Animation(TimeDelta, transform, 0.2, false, "BoneHip");
}

void CBoss::SetupColliders()
{
	collider->Update(XMLoadFloat4x4(&transform->Get_WorldMatrix()));
	m_pOverlapCollider->Update(XMLoadFloat4x4(&transform->Get_WorldMatrix()));
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

void CBoss::OnCollisionEnter(CCollider * src, CCollider * dest)
{
}

void CBoss::OnCollisionStay(CCollider * src, CCollider * dest)
{
	CPlayerCamera* pCamera = dynamic_cast<CPlayerCamera*>(dest->GetOwner());
	if (src->Compare(m_pOverlapCollider) && pCamera && dest->Compare(m_pCamera->GetCollider()))
	{
		m_bAlpha = true;
	}
}

void CBoss::OnCollisionExit(CCollider * src, CCollider * dest)
{

}

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

	for (int j = 0; j < 3; j++)
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

	Safe_Release(m_pBodyTransform);
}
