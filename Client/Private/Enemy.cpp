#include "pch.h"
#include "..\Public\Enemy.h"

#include "GameInstance.h"
#include "SkillBallSystem.h"
#include "ApplicationManager.h"
#include "PlayerCamera.h"
#include "Character.h"
#include "Bone.h"
#include "Boss.h"

_uint CEnemy::s_iCount = 0;

CEnemy::CEnemy(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CEnemy::CEnemy(const CEnemy & rhs)
	: CGameObject(rhs)
	, m_State(rhs.m_State)
	, m_bMovable(rhs.m_bMovable)
	, m_bOverlapped(rhs.m_bOverlapped)
	, m_OverlapAcc(rhs.m_OverlapAcc)
	, m_OverlappedWait(rhs.m_OverlappedWait)
	, m_eType(rhs.m_eType)
{
	s_iCount++;
}

HRESULT CEnemy::Initialize_Prototype(TYPE eType)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	ZeroMemory(&m_State, sizeof(ENEMY_STATE));
	m_State.fMaxHp = 4000.f;
	m_State.fCurHp = 4000.f;

	m_eType = eType;

	return S_OK;
}

HRESULT CEnemy::Initialize(void * pArg)
{
	m_pAppManager = CApplicationManager::GetInstance();

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	AddComponents();

	if (nullptr != pArg)
	{
		m_pPlayer = (CCharacter*)pArg;
		m_pPlayerTransform = static_cast<CTransform*>(m_pPlayer->Find_Component(L"com_transform"));
	}

	int iRandomX = rand() % 20 + 15;
	int iRandomZ = rand() % 25 + 35;
 	SetPosition(_float3(iRandomX , 0.f, iRandomZ));

	bone = model->GetBonePtr("Bip001Pelvis");
	XMStoreFloat4x4(&m_RootBoneMatrix, XMLoadFloat4x4(&bone->GetOffSetMatrix()) * XMLoadFloat4x4(&bone->GetCombinedMatrix()) * XMLoadFloat4x4(&model->GetLocalMatrix()) * XMLoadFloat4x4(&transform->Get_WorldMatrix()));

	ZeroMemory(&m_State, sizeof(ENEMY_STATE));
	switch (m_eType)
	{
	case TYPE::HUMANOID:
		m_State.fMaxHp = 1000.f;
		m_State.fCurHp = 1000.f;
		m_pWeaponBone = model->GetBonePtr("BulletCase");
		break;
	case TYPE::ANIMAL:
		m_State.fMaxHp = 1000.f;
		m_State.fCurHp = 1000.f;
		m_pWeaponBone = model->GetBonePtr("Bip001");
		break;
	}

	m_eState = CGameObject::STATE::DISABLE;

	int iRandomWaitTime = rand() % 3 + 1;
	int iRandomWaitTimeDot = rand() % 10;
	m_fSpawnWaitTimeOut = (_float)iRandomWaitTime + ((_float)iRandomWaitTimeDot * 0.1f);

	return S_OK;
}

void CEnemy::Tick(_double TimeDelta)
{
	TimeDelta = Freeze(TimeDelta);

	m_bAlpha = false;

	__super::Tick(TimeDelta);
	
	if (m_bAttackCollision)
	{
		m_fAttackCollisionLocal += (_float)TimeDelta;
		if (m_fAttackCollisionLocal >= m_fAttackCollisionTimeOut)
		{
			m_bAttackCollision = false;
			//m_pWeaponCollider->SetActive(false);
			m_fAttackCollisionLocal = 0.0f;
		}
	}

	if(m_bDeadWait)
		Die(TimeDelta);
	else
	{
		AnimationState(TimeDelta);
		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		pGameInstance->AddCollider(collider);
		pGameInstance->AddCollider(m_pOverlapCollider);
		pGameInstance->AddCollider(m_pWeaponCollider);
	}
}

void CEnemy::LateTick(_double TimeDelta)
{
	TimeDelta = Freeze(TimeDelta);

	__super::LateTick(TimeDelta);
	
	model->Play_Animation(TimeDelta, transform, 0.1);

	_matrix tranMatrix = XMLoadFloat4x4(&bone->GetOffSetMatrix()) * XMLoadFloat4x4(&bone->GetCombinedMatrix()) * XMLoadFloat4x4(&model->GetLocalMatrix()) * XMLoadFloat4x4(&transform->Get_WorldMatrix());
	
	_float4x4 overlapMatrix = transform->Get_WorldMatrix();
	//collider->Update(XMLoadFloat4x4(&transform->Get_WorldMatrix()));
	collider->Update(XMLoadFloat4x4(&transform->Get_WorldMatrix()));
	m_pOverlapCollider->Update(XMLoadFloat4x4(&overlapMatrix));
	m_pWeaponCollider->Update(XMLoadFloat4x4(&transform->Get_WorldMatrix()));	

	if (nullptr != renderer)
		renderer->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CEnemy::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetupShaderResources()))
		return E_FAIL;

	_uint MeshCount = model->Get_MeshCount();
	for (_uint i = 0; i < MeshCount; ++i)
	{
		if (!strcmp("Cheek", model->GetMeshName(i)) || !strcmp("Cheek01", model->GetMeshName(i)))
			continue;

		model->Setup_ShaderMaterialResource(shader, "g_DiffuseTexture", i, aiTextureType::aiTextureType_DIFFUSE);

		model->Setup_BoneMatrices(shader, "g_BoneMatrix", i);

		if(m_bAlpha)
			shader->Begin(1);
		else
			shader->Begin(0);

		model->Render(i);
	}

	return S_OK;
}

void CEnemy::RenderGUI()
{
}

_fmatrix CEnemy::GetWorldMatrix()
{
	return XMLoadFloat4x4(&transform->Get_WorldMatrix());
}

void CEnemy::LookPlayer()
{
	transform->LookAt(m_pPlayerTransform->Get_State(CTransform::STATE_POSITION));
}

_float2 CEnemy::Reset(_float3 vPos, _float fRadius)
{
	m_eState = STATE::ACTIVE;

	m_bDead = false;
	m_bDeadWait = false;
	
	_float2 iRandomPos = {0.f, 0.f};

	iRandomPos.x = (rand() % (int)fRadius) - ((int)fRadius >> 1) + (int)vPos.x;
	iRandomPos.y = (rand() % (int)fRadius) - ((int)fRadius >> 1) + (int)vPos.z;
	SetPosition(_float3(iRandomPos.x, 0.f, iRandomPos.y));

	m_State.fCurHp = m_State.fMaxHp;

	m_fNuckBackTimer = 0.0f;
	m_fDeadWaitTimer = 0.0f;

	m_bTraceFinish = false;
	m_bRotationFinish = false;
	m_bAttack = false;
	m_bOverlapped = false;
	m_bMovable = false;
	m_bHit = false;
	m_bNuckback = false;
	m_bNuckBackFinish = false;
	m_bAirHit = false;
	m_bAir = false;
	m_bStandupStart = false;
	m_bHolding = false;

	m_bSpawnWait = true;
	m_fSpawnWaitAcc = 0.f;

	int iRandomWaitTime = rand() % 3 + 1;
	int iRandomWaitTimeDot = rand() % 10;
	m_fSpawnWaitTimeOut = (_float)iRandomWaitTime + ((_float)iRandomWaitTimeDot * 0.1f);

	return iRandomPos;
}

_float4 CEnemy::GetPosition()
{
 	if (transform == nullptr)
		return _float4();

	if(IsDestroy())
		return _float4();

	_float4 vPos;
	XMStoreFloat4(&vPos, transform->Get_State(CTransform::STATE_POSITION));

	return vPos;
}

void CEnemy::SetPosition(_float3 vPosition)
{
	if (transform == nullptr)
		return;

	transform->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&vPosition));
}

HRESULT CEnemy::AddComponents()
{
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("proto_com_renderer"), TEXT("com_renderer"), (CComponent**)&renderer)))
		return E_FAIL;

	CTransform::TRANSFORM_DESC TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORM_DESC));

	TransformDesc.fMoveSpeed = 1.5f;
	TransformDesc.fRotationSpeed = 5.f;

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("proto_com_transform"), TEXT("com_transform"), (CComponent**)&transform, &TransformDesc)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("proto_com_shader_vtxanimmodel"), TEXT("com_shader"), (CComponent**)&shader)))
		return E_FAIL;

	if (m_eType == TYPE::HUMANOID)
	{
		if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("proto_com_model_enemy01"), TEXT("com_model"), (CComponent**)&model)))
			return E_FAIL;
	}
	else
	{
		if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("proto_com_model_enemy02"), TEXT("com_model"), (CComponent**)&model)))
			return E_FAIL;
	}

	CCollider::COLLIDER_DESC collDesc;
	collDesc.owner = this;
	collDesc.vCenter = _float3(0.f, 1.f, 0.f);
	collDesc.vExtents = _float3(1.5f, 2.f, 1.5f);
	collDesc.vRotation = _float3(0.f, 0.f, 0.f);

	(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("proto_com_obb_collider"), TEXT("com_collider"), (CComponent**)&collider, &collDesc));

	ZeroMemory(&collDesc, sizeof collDesc);
	collDesc.owner = this;
	collDesc.vCenter = _float3(0.f, 1.3f, 0.f);
	collDesc.vExtents = _float3(1.1f, 1.1f, 1.1f);
	collDesc.vRotation = _float3(0.f, 0.f, 0.f);

	(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("proto_com_sphere_collider"), TEXT("com_collder_enemy"), (CComponent**)&m_pOverlapCollider, &collDesc));

	ZeroMemory(&collDesc, sizeof collDesc);
	collDesc.owner = this;
	collDesc.vExtents = _float3(1.7f, 1.7f, 1.7f);
	collDesc.vCenter = _float3(0.f, 1.f, 1.7f);
	collDesc.vRotation = _float3(0.f, 0.f, 0.f);

	(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("proto_com_sphere_collider"), TEXT("com_collder_weapon"), (CComponent**)&m_pWeaponCollider, &collDesc));
	m_pWeaponCollider->SetActive(false);

	return S_OK;
}

HRESULT CEnemy::SetupShaderResources()
{
	if (nullptr == shader)
		return E_FAIL;

	CGameInstance* pInstance = CGameInstance::GetInstance();

	if (pInstance->Input_KeyState_Custom(DIK_PGUP) == KEY_STATE::TAP)
		animation++;

	if (pInstance->Input_KeyState_Custom(DIK_PGDN) == KEY_STATE::TAP)
		animation--;

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

void CEnemy::OverlapProcess(_double TimeDelta)
{
	if (m_OverlapAcc >= m_OverlappedWait)
	{
		m_bRotationFinish = false;
		m_bOverlapped = false;

		m_bTraceFinish = false;
		m_bAttackCollision = false;

		m_OverlapAcc = 0.0;
	}
	
	//플레이어를 향하는 방향벡터  ( Enemy -> Player)
	//_vector vPlayerDir = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION) - transform->Get_State(CTransform::STATE_POSITION);

	//현재 위치에서 반대방향으로 이동 + 플레이어 방향벡터(반대)
	_vector vPos = transform->Get_State(CTransform::STATE_POSITION) + XMLoadFloat3(&m_vNagative) * (_float)TimeDelta;
	_vector vEnemyLook = XMVector3Normalize(transform->Get_State(CTransform::STATE_LOOK));
	_vector vDir = XMVector3Normalize(vPos - transform->Get_State(CTransform::STATE_POSITION));

	//EnemyLook , 이동해야할 방향 
	_float vAxisY = XMVectorGetY(XMVector3Cross(vEnemyLook, vDir));
	_float fRadian = XMVectorGetX(XMVector3Dot(vDir, vEnemyLook));
	if (isnan(fRadian))
		fRadian = 0.0f;

	//공격중엔 애니메이션이 끝난뒤 Overlap진행
	if (model->AnimationCompare((_uint)CLIP::ATTACK1) ||
		model->AnimationCompare((_uint)CLIP::ATTACK2) ||
		model->AnimationCompare((_uint)CLIP::ATTACK3))
	{
		m_OverlapAcc = 0.0;
		if (model->AnimationIsFinish())
		{
			if (fRadian > 0.8f)
			{
				transform->Set_State(CTransform::STATE_POSITION, vPos);

				if(m_eType == TYPE::HUMANOID)
					model->Setup_Animation((_uint)CLIP::RUN, CAnimation::TYPE::LOOP, true);
				if (m_eType == TYPE::ANIMAL)
					model->Setup_Animation((_uint)CLIP_TWO::RUN, CAnimation::TYPE::LOOP, true);
			}
			else if (fRadian < -0.8f)
			{
				transform->Set_State(CTransform::STATE_POSITION, vPos);

				if (m_eType == TYPE::HUMANOID)
					model->Setup_Animation((_uint)CLIP::RUNB, CAnimation::TYPE::LOOP, true);
				if (m_eType == TYPE::ANIMAL)
					model->Setup_Animation((_uint)CLIP_TWO::WALKB, CAnimation::TYPE::LOOP, true);
			}
			else
			{
				if (vAxisY >= 0.0)
				{
					transform->Set_State(CTransform::STATE_POSITION, vPos);

					if (m_eType == TYPE::HUMANOID)
						model->Setup_Animation((_uint)CLIP::RUNR, CAnimation::TYPE::LOOP, true);
					if (m_eType == TYPE::ANIMAL)
						model->Setup_Animation((_uint)CLIP_TWO::WALKR, CAnimation::TYPE::LOOP, true);
				}
				else
				{
					transform->Set_State(CTransform::STATE_POSITION, vPos);

					if (m_eType == TYPE::HUMANOID)
						model->Setup_Animation((_uint)CLIP::RUNL, CAnimation::TYPE::LOOP, true);
					if (m_eType == TYPE::ANIMAL)
						model->Setup_Animation((_uint)CLIP_TWO::WALKL, CAnimation::TYPE::LOOP, true);
				}
			}
		}
	}
	else
	{
		if (fRadian > 0.8f)
		{
				transform->Set_State(CTransform::STATE_POSITION, vPos);

				if (m_eType == TYPE::HUMANOID)
					model->Setup_Animation((_uint)CLIP::RUN, CAnimation::TYPE::LOOP, true);
				if (m_eType == TYPE::ANIMAL)
					model->Setup_Animation((_uint)CLIP_TWO::RUN, CAnimation::TYPE::LOOP, true);
		}
		else if (fRadian < -0.8f)
		{
			transform->Set_State(CTransform::STATE_POSITION, vPos);

			if (m_eType == TYPE::HUMANOID)
				model->Setup_Animation((_uint)CLIP::RUNB, CAnimation::TYPE::LOOP, true);
			if (m_eType == TYPE::ANIMAL)
				model->Setup_Animation((_uint)CLIP_TWO::WALKB, CAnimation::TYPE::LOOP, true);
		}
		else
		{
			if (vAxisY >= 0.0)
			{
				transform->Set_State(CTransform::STATE_POSITION, vPos);

				if (m_eType == TYPE::HUMANOID)
					model->Setup_Animation((_uint)CLIP::RUNR, CAnimation::TYPE::LOOP, true);
				if (m_eType == TYPE::ANIMAL)
					model->Setup_Animation((_uint)CLIP_TWO::WALKR, CAnimation::TYPE::LOOP, true);
			}
			else
			{
				transform->Set_State(CTransform::STATE_POSITION, vPos);

				if (m_eType == TYPE::HUMANOID)
					model->Setup_Animation((_uint)CLIP::RUNL, CAnimation::TYPE::LOOP, true);
				if (m_eType == TYPE::ANIMAL)
					model->Setup_Animation((_uint)CLIP_TWO::WALKL, CAnimation::TYPE::LOOP, true);
			}
		}

		m_OverlapAcc += TimeDelta;
	}
}

void CEnemy::LookPlayer(_double TimeDelta)
{
	if (m_pAppManager->IsFreeze())
		return;

	_vector vPlayerPos = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	_vector vPosition = transform->Get_State(CTransform::STATE_POSITION);
	_vector vDirection = XMVector3Normalize(vPlayerPos - vPosition);

	_vector vCurrentLook = XMVector3Normalize(transform->Get_State(CTransform::STATE_LOOK));
	_vector vCurrentRight = XMVector3Normalize(transform->Get_State(CTransform::STATE_RIGHT));
	_float fRadianAngle = acos(XMVectorGetX(XMVector3Dot(vCurrentLook, vDirection)));

	if (isnan(fRadianAngle))
	{
		m_bRotationFinish = false;
		m_bMovable = false;
		fRadianAngle = 0.0f;
	}

	if (fRadianAngle < 0.1)
	{
		m_bRotationFinish = true;
		m_bMovable = true;
	}
	else
	{
		m_bMovable = false;
		m_bRotationFinish = false;
	}

	_vector vRight= XMVector3Normalize(XMVector3Cross(VECTOR_UP, vDirection));
	_float fAxisY = XMVectorGetY(XMVector3Cross(vCurrentLook, vDirection));


	if(fAxisY >= 0.f)
		transform->Rotate(VECTOR_UP, TimeDelta * fRadianAngle);
	else
		transform->Rotate(VECTOR_UP, TimeDelta * -fRadianAngle);

}

void CEnemy::Trace(_double TimeDelta)
{
	if (!m_bMovable)
		return;

	m_bAttack = false;
	m_bAttackCollision = false;
	m_fAttackCoolTimer = 0.0f;
	
	_vector vPlayerPos = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	transform->Chase(vPlayerPos, TimeDelta, m_fAttackRange);

	if (m_eType == TYPE::HUMANOID)
		model->Setup_Animation((_uint)CLIP::RUN, CAnimation::TYPE::LOOP, true);
	else
		model->Setup_Animation((_uint)CLIP_TWO::RUN, CAnimation::TYPE::LOOP, true);
}

void CEnemy::Attack(_double TimeDelta)
{
	m_bMovable = false;

	if (m_eType == TYPE::HUMANOID)
		model->Setup_Animation((_uint)CLIP::ATTACK4, CAnimation::TYPE::ONE, true);
	else
		model->Setup_Animation((_uint)CLIP_TWO::ATTACK1, CAnimation::TYPE::ONE, true);

}

void CEnemy::Idle(_double TimeDelta)
{
	m_bAttack = false;
	m_bMovable = true;
	m_bRotationFinish = false;
	m_bAttackCollision = false;

	if (m_eType == TYPE::HUMANOID)
		model->Setup_Animation((_uint)CLIP::STAND, CAnimation::TYPE::LOOP, true);
	else
		model->Setup_Animation((_uint)CLIP_TWO::STAND2, CAnimation::TYPE::LOOP, true);
}

_bool CEnemy::Hit(_double TimeDelta)
{
	int iRandom = rand() % 2;
	
	m_bAttackCollision = false;

	m_bRotationFinish = false;

	if (m_State.fCurHp <= 0.f)
		m_bDeadWait = true;
	else
		m_bDeadWait = false;

	if (m_bAirHit)
	{
		m_bHitStart = false;
		m_bHit = false;
		m_bNuckBackFinish = false;
		m_fNuckBackTimer = 0.0f;
		return false;
	}

	//if (m_pAppManager->IsFreeze())
	//	return false;
	if (m_eType == TYPE::HUMANOID)
	{
		if(m_iRandomHitAnim == 0)
			model->Setup_Animation((_uint)CLIP::HIT1, CAnimation::TYPE::ONE, true);
		else if(m_iRandomHitAnim == 1)
			model->Setup_Animation((_uint)CLIP::HIT2, CAnimation::TYPE::ONE, true);
	}
	else
	{
		if (m_iRandomHitAnim == 0)
			model->Setup_Animation((_uint)CLIP_TWO::HIT2, CAnimation::TYPE::ONE, true);
		else if (m_iRandomHitAnim == 1)
			model->Setup_Animation((_uint)CLIP_TWO::HIT2, CAnimation::TYPE::ONE, true);
	}

	if (model->AnimationIsFinishEx())
	{
		m_bHitStart = false;
		m_bHit = false;
		m_bNuckBackFinish = false;
		m_fNuckBackTimer = 0.0f;
		return true;
	}
	else
		return false;
}

void CEnemy::RecvDamage(_float fDamage)
{
	CSkillBallSystem* pSkillSystem = CSkillBallSystem::GetInstance();
	CApplicationManager::GetInstance()->SetHitFreeze(true);

	int iRandom = rand() % 100;
	if (iRandom < 30)
	{
		int iRandom = rand() % 3;
		pSkillSystem->PushSkill(m_pDevice, m_pContext, (CSkillBase::TYPE)iRandom);
	}

	//if (IsAirbone())
	//	AirboneReset();

	//m_pPlayer->AddCombo();
	//m_pPlayer->ResetComboTime();

	m_State.fCurHp -= fDamage;

	DieCheck();
}

void CEnemy::SetNuckback(_float fPower)
{
	m_bNuckback = true;
	m_bNuckBackFinish = false;
	m_fNuckbackPower = fPower;
}

void CEnemy::SetAirborne(_float fDamage)
{
	m_bAir = true;
	m_bAirHit = true;
	m_bStandupStart = false;
	m_fStandupTimer = 0.0f;
	RecvDamage(fDamage);
}

void CEnemy::NuckBack(_double TimeDelta)
{
	m_fNuckBackTimer += (_float)TimeDelta;
	if (m_fNuckBackTimer >= m_fNuckBackTimeOut)
	{
		m_fNuckbackPower = 0.4f;
		m_bNuckback = false;
		m_bNuckBackFinish = true;
		m_fNuckBackTimer = 0.0f;
	}

	_vector vPlayerPos = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	_vector vCurrentPos = transform->Get_State(CTransform::STATE_POSITION);
	//_vector vDir = m_pPlayerTransform->Get_State(CTransform::STATE_LOOK);

	LookPlayer(TimeDelta);

	_vector vDir = vCurrentPos - vPlayerPos;
	vDir = XMVector3Normalize(vDir);

	_vector vPos;
	vPos = vCurrentPos + (vDir) * m_fNuckbackPower;
	vCurrentPos = XMVectorLerp(vCurrentPos, vPos, TimeDelta * 0.8f);
	transform->Set_State(CTransform::STATE_POSITION, vCurrentPos);

}

void CEnemy::Airborne(_double TimeDelta)
{
	m_bRotationFinish = false;

	if (m_bAir)
	{
		if(m_eType == TYPE::HUMANOID)
			model->Setup_Animation((_uint)CLIP::HITDOWN, CAnimation::TYPE::ONE, false);
		else
			model->Setup_Animation((_uint)CLIP_TWO::HITDOWN, CAnimation::TYPE::ONE, false);
	}
	else
	{
		if (m_eType == TYPE::HUMANOID)
			model->Setup_Animation((_uint)CLIP::STANDUP, CAnimation::TYPE::ONE, false);
		else
			model->Setup_Animation((_uint)CLIP_TWO::STANDUP, CAnimation::TYPE::ONE, false);
	}

	if (m_eType == TYPE::HUMANOID)
	{
		if (model->AnimationCompare((_uint)CLIP::HITDOWN))
		{
			if (model->AnimationIsFinishEx())
				m_bStandupStart = true;
		}
		else if (model->AnimationCompare((_uint)CLIP::STANDUP))
		{
			if (model->AnimationIsFinishEx())
			{
				m_bAir = false;
				m_bAirHit = false;
			}
		}
	}
	else
	{
		if (model->AnimationCompare((_uint)CLIP_TWO::HITDOWN))
		{
			if (model->AnimationIsFinishEx())
				m_bStandupStart = true;
		}
		else if (model->AnimationCompare((_uint)CLIP_TWO::STANDUP))
		{
			if (model->AnimationIsFinishEx())
			{
				m_bAir = false;
				m_bAirHit = false;
			}
		}
	}

	if (m_bStandupStart)
	{
		m_fStandupTimer += (_float)TimeDelta;
		if (m_fStandupTimer >= m_fStandupTimeOut)
		{
			m_bAir = false;
			m_bStandupStart = false;
			m_fStandupTimer = 0.0f;
		}
	}

}

void CEnemy::AirboneReset()
{
	if (m_eType == TYPE::HUMANOID)
	{
		if (model->AnimationCompare((_uint)CLIP::HITDOWN))
			model->AnimationReset();
	}
	else
	{
		if (model->AnimationCompare((_uint)CLIP_TWO::HITDOWN))
			model->AnimationReset();
	}
}

void CEnemy::Holding(_double TimeDleta)
{
	
}

_bool CEnemy::DieCheck()
{
	if (m_State.fCurHp <= 0.f)
	{
		m_bDeadWait = true;
		return true;
	}

	return false;
}

void CEnemy::Die(_double TimeDelta)
{
	if (!m_bDead)
	{
		static_cast<CCharacter*>(m_pPlayer)->DeleteTargetFromList(this);

		if (m_eType == TYPE::HUMANOID)
			model->Setup_Animation((_uint)CLIP::DEATH, CAnimation::TYPE::ONE, true);
		else
			model->Setup_Animation((_uint)CLIP_TWO::DEATH, CAnimation::TYPE::ONE, true);
	}

	if (m_bDeadWait && model->AnimationIsFinishEx())
	{
		CCollider* pColl = m_pPlayer->GetEnemyCheckCollider();
		pColl->EraseHitCollider(m_pOverlapCollider);
		m_pOverlapCollider->EraseHitCollider(pColl);

		m_bDead = true;
	}

	m_fDeadWaitTimer += (_float)TimeDelta;
	if (m_fDeadWaitTimer >= m_fDeadWaitTimeOut)
	{
		SetState(STATE::DISABLE);
	}
}

_double CEnemy::Freeze(_double TimeDelta)
{
	if (m_pAppManager->IsFreeze())
	{
		m_bAttackCollision = false;

		if(!dynamic_cast<CBoss*>(this))
			m_pWeaponCollider->SetActive(false);

		_vector CurTimeDelta = XMVectorSet(m_fCurTimeScale, m_fCurTimeScale, m_fCurTimeScale, m_fCurTimeScale);
		m_fCurTimeScale = XMVectorGetX(XMVectorLerp(CurTimeDelta, XMVectorSet(0.1f, 0.1f, 0.1f, 0.1f), TimeDelta * 0.8));
	}
	else
		m_fCurTimeScale = 1.0;

	return TimeDelta = TimeDelta * m_fCurTimeScale;
}

_vector CEnemy::GetRootBonePosition()
{
	_matrix BoneMatrix = XMLoadFloat4x4(&m_RootBoneMatrix);
	return BoneMatrix.r[3];
}

void CEnemy::AnimationState(_double TimeDelta)
{
	//스폰됬을떄 대기
	if (m_bSpawnWait)
	{
		if (m_bHit || m_bAirHit || m_bHolding)
		{
			m_fSpawnWaitAcc = 0.f;
			m_bSpawnWait = false;
		}

		if (m_eType == TYPE::HUMANOID)
			model->Setup_Animation((_uint)CLIP::STAND, CAnimation::TYPE::LOOP, true);
		else
			model->Setup_Animation((_uint)CLIP_TWO::STAND2, CAnimation::TYPE::LOOP, true);

		m_fSpawnWaitAcc += TimeDelta;

		if (m_fSpawnWaitAcc >= m_fSpawnWaitTimeOut)
		{
			m_bSpawnWait = false;
			m_fSpawnWaitAcc = 0.f;
		}

		return;
	}



	if (m_bAirHit)
	{
		Airborne(TimeDelta);
	}
	else if (m_bHit)
	{
		Hit(TimeDelta);
		NuckBack(TimeDelta);
	}
	else if (m_bHolding)
	{
		transform->LookAt(m_pPlayerTransform->Get_State(CTransform::STATE_POSITION));

		switch (m_eType)
		{
		case Client::CEnemy::TYPE::HUMANOID:
			model->Setup_Animation((_uint)CLIP::HIT1, CAnimation::TYPE::LOOP, true);
			break;
		case Client::CEnemy::TYPE::ANIMAL:
			model->Setup_Animation((_uint)CLIP_TWO::HIT2, CAnimation::TYPE::LOOP, true);
			break;
		default:
			break;
		}

	}
	else
	{
		_vector vPlayerPos = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);
		_vector vPosition = transform->Get_State(CTransform::STATE_POSITION);

		_vector vTargetDir = vPlayerPos - vPosition;
		_float vDistance = XMVectorGetX(XMVector3Length(vTargetDir));

		if (m_bOverlapped)
		{
				OverlapProcess(TimeDelta);
		}
		else
		{
			m_OverlapAcc = 0.0;

			if (!m_bRotationFinish)
				LookPlayer(TimeDelta);

			if (m_bRotationFinish)
			{
				if (vDistance > m_fAttackRange)
				{
					if (m_bAttack)
					{
						if (m_eType == TYPE::HUMANOID)
						{
							if (model->AnimationCompare(((_uint)CLIP::ATTACK4)))
							{
								if (model->AnimationIsFinishEx())
								{
									Trace(TimeDelta);
									m_fAttackCoolTimer = 0.f;
								}
							}
						}
						else
						{
							if (model->AnimationCompare(((_uint)CLIP::ATTACK1)))
							{
								if (model->AnimationIsFinishEx())
								{
									Trace(TimeDelta);
									m_fAttackCoolTimer = 0.f;
								}
							}
						}
					}
					else
					{
						if (m_bTraceFinish)
						{
							m_fTraceLocal += TimeDelta;
							if (m_fTraceLocal >= m_fTraceTimeOut)
							{
								m_fTraceLocal = 0.0;
								m_bRotationFinish = false;
								m_bTraceFinish = false;
							}
						}
						else
						{
							Trace(TimeDelta);
							m_fAttackCoolTimer = 0.f;
						}
					}
				}
				else
				{
					m_bTraceFinish = true;

					if (!m_bAttack)
					{
						if (m_eType == TYPE::HUMANOID)
							model->Setup_Animation((_uint)CLIP::STAND, CAnimation::TYPE::LOOP, true);
						else
							model->Setup_Animation((_uint)CLIP_TWO::STAND2, CAnimation::TYPE::LOOP, true);
					}

					m_fAttackCoolTimer += TimeDelta;
					if (m_fAttackCoolTimer >= m_fAttackCoolTimeOut)
					{
						m_bAttack = true;
						m_fAttackCoolTimer = 0.f;
					}
				}
			}

			if (m_bAttack)
			{
				Attack(TimeDelta);

				if (!m_bAttackCollision || !m_bAttackOneCall)
				{
					m_bAttackOneCall = true;
					if (model->AnimationIsPreFinishEx())
					{
						m_bAttackCollision = true;
						m_pWeaponCollider->SetActive(true);
					}
				}

				if (model->AnimationIsFinishEx())
				{
					m_bAttack = false;
					m_bAttackOneCall = false;
					m_bAttackCollision = false;
					m_pWeaponCollider->SetActive(false);
				}
			}

			if (model->AnimationIsFinishEx())
				Idle(TimeDelta);
		}
	}
 
	//공격중이 아닌데 켜져있으면 끄자 && 초산 공간일떄도 
	if(m_pAppManager->IsFreeze() || (!m_bAttack && m_pWeaponCollider->IsActive()))
		m_pWeaponCollider->SetActive(false);

	if (m_bNuckback)
	{
		NuckBack(TimeDelta);
	}
}

CEnemy * CEnemy::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, TYPE eType)
{
	CEnemy*	pInstance = new CEnemy(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype(eType)))
	{
		MSG_BOX("Failed to Create : CEnemy");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CEnemy::Clone(void * pArg)
{
	CEnemy* pInstance = new CEnemy(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CEnemy");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEnemy::Free()
{
	__super::Free();

	s_iCount--;

	Safe_Release(renderer);
	Safe_Release(transform);
	Safe_Release(model);
	Safe_Release(shader);
	Safe_Release(collider);
	Safe_Release(m_pOverlapCollider);
}

void CEnemy::OnCollisionEnter(CCollider * src, CCollider * dest)
{
	//몬스터 끼리 밀어내기 처리
	CEnemy* pEnemy = dynamic_cast<CEnemy*>(src->GetOwner());
	CEnemy* pAotherEnemy = dynamic_cast<CEnemy*>(dest->GetOwner());
	//둘다 에너미 타입이고 기본 충돌체일때

	if (pEnemy && pAotherEnemy)
	{
		if (src->Compare(m_pOverlapCollider) && dest->Compare(pAotherEnemy->GetOverlapCollider()))
		{
			if (m_eType == TYPE::HUMANOID)
			{
				if (model->AnimationCompare((_uint)CLIP::ATTACK4))
					return;
			}
			else if (m_eType == TYPE::ANIMAL)
			{
				if (model->AnimationCompare((_uint)CLIP_TWO::ATTACK1))
					return;
			}

			_float3 vSrcDir;

			//현재 밀리고있을때
			if (pEnemy->IsOverlap())
			{
				//이전에 밀리고있는방향
				_vector vPrevDir = XMLoadFloat3(&m_vNagative);
				_vector vDir = XMVector3Normalize(vPrevDir + XMLoadFloat4(&pAotherEnemy->GetPosition()) - XMLoadFloat4(&pEnemy->GetPosition()));
				XMStoreFloat3(&vSrcDir, -vDir);

				m_OverlapAcc = 0.0f;
				pEnemy->SetOverlap(true, vSrcDir);
			}
			else
			{
				_vector vDir = XMVector3Normalize(XMLoadFloat4(&pAotherEnemy->GetPosition()) - XMLoadFloat4(&pEnemy->GetPosition()));
				XMStoreFloat3(&vSrcDir, -vDir);

				m_OverlapAcc = 0.0f;
				pEnemy->SetOverlap(true, vSrcDir);
			}
		}
	}

	//플레이어 웨폰과 충돌처리
	CCharacter* pPlayer = dynamic_cast<CCharacter*>(dest->GetOwner());
	
	if (pPlayer)
	{
		m_bOverlapped = false;

		//플레이어 공격에 의해 데미지 받기
		CCollider* pWeaponCollider = pPlayer->GetWeaponCollider();
		if (src->Compare(m_pOverlapCollider) && dest->Compare(pWeaponCollider))
		{
			CBoss* pBoss = dynamic_cast<CBoss*>(this);
			if (!pBoss)
			{
				m_iRandomHitAnim = rand() % 2;
				if (m_bHit)
				{
					if (model->AnimationCompare((_uint)CLIP::HIT1) ||
						model->AnimationCompare((_uint)CLIP::HIT2) ||
						model->AnimationCompare((_uint)CLIP_TWO::HIT1) ||
						model->AnimationCompare((_uint)CLIP_TWO::HIT2))
					{
						//탑승한애는 애니메이션 리셋시 너무 떨려서 이렇게 처리해놓음
						if (m_eType == TYPE::ANIMAL)
							model->Setup_Animation((_uint)CLIP_TWO::HIT2, CAnimation::TYPE::ONE, true);
						else
							model->AnimationReset();
					}
				}
			}


			m_bHit = true;
			RecvDamage(pPlayer->GetDamage());
		}

		//CCollider* pSkillCollider = pPlayer->GetSkillCollider();
		//if (src->Compare(collider))
		//{
		//	if (dest->Compare(pSkillCollider))
		//	{
		//		CGameInstance* pInstance = CGameInstance::GetInstance();
		//		_float DeltaTime = pInstance->GetTimer(TEXT("144FPS"));

		//		Airborne(DeltaTime);
		//		pPlayer->RecvDamage(pPlayer->GetDamage());
		//	}
		//}

		//플레이어에게 데미지 주기
		CCollider* pBodyCollider = pPlayer->GetBodyCollider();
		if (src->Compare(m_pWeaponCollider))
		{
			if (dest->Compare(pBodyCollider))
			{
				pPlayer->LookPos(transform->Get_State(CTransform::STATE_POSITION));
				pPlayer->Hit();
				pPlayer->RecvDamage(25.f);
			}
		}
	}


}

void CEnemy::OnCollisionStay(CCollider * src, CCollider * dest)
{
	CPlayerCamera* pCamera = dynamic_cast<CPlayerCamera*>(dest->GetOwner());
	if (src->Compare(m_pOverlapCollider) && pCamera && dest->Compare(m_pCamera->GetCollider()))
	{
		m_bAlpha = true;
	}
}

void CEnemy::OnCollisionExit(CCollider * src, CCollider * dest)
{
	//CEnemy* pEnemy = dynamic_cast<CEnemy*>(src->GetOwner());
	//CEnemy* pAotherEnemy = dynamic_cast<CEnemy*>(dest->GetOwner());

	////둘다 에너미 타입이고 기본 충돌체일때
	//if (pEnemy && pAotherEnemy)
	//{
	//	pEnemy->SetOverlap(false, _float3());
	//	pAotherEnemy->SetOverlap(false, _float3());
	//}
}

_float CEnemy::GetLengthFromCamera()
{
	CPipeLine* pPipeLine = CPipeLine::GetInstance();

	_vector vPos = transform->Get_State(CTransform::STATE_POSITION);
	_vector vCamPos = XMLoadFloat4(&pPipeLine->Get_CamPosition());

	_float fLength = XMVectorGetX(XMVector3Length(vPos - vCamPos));

	return fLength;
}
