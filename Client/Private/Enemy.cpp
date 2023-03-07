#include "pch.h"
#include "..\Public\Enemy.h"

#include "GameInstance.h"
#include "Character.h"
#include "Bone.h"

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
	m_State.fMaxHp = 700.f;
	m_State.fCurHp = 700.f;

	m_eType = eType;

	return S_OK;
}

HRESULT CEnemy::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	AddComponents();

	if (nullptr != pArg)
	{
		m_pPlayer = (CCharacter*)pArg;
		m_pPlayerTransform = static_cast<CTransform*>(m_pPlayer->Find_Component(L"com_transform"));
	}

 	SetPosition(_float3(20.f + (s_iCount * 5), 0.f, 40.f));

	bone = model->GetBonePtr("Bip001");

	return S_OK;
}

void CEnemy::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	pGameInstance->AddCollider(collider);

	AnimationState(TimeDelta);

}

void CEnemy::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	model->Play_Animation(TimeDelta, transform, 0.3);

	_matrix tranMatrix = XMLoadFloat4x4(&bone->GetCombinedMatrix()) * XMLoadFloat4x4(&transform->Get_WorldMatrix());
	collider->Update(tranMatrix);

	if (nullptr != renderer)
		renderer->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);
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
		shader->Begin(0);
		model->Render(i);
	}

	return S_OK;
}

void CEnemy::RenderGUI()
{
}

_float4 CEnemy::GetPosition()
{
 	if (transform == nullptr)
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
	collDesc.vCenter = _float3(0.f, 0.f, 0.f);
	collDesc.vExtants = _float3(1.f, 1.f, 1.f);
	collDesc.vRotaion = _float3(0.f, 0.f, 0.f);

	(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("proto_com_sphere_collider"), TEXT("com_collider"), (CComponent**)&collider, &collDesc));

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
		m_OverlapAcc = 0.0;
	}

	//현재 위치에서 반대방향으로 이동
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
	if (m_eType == TYPE::HUMANOID)
		model->Setup_Animation((_uint)CLIP::STAND, CAnimation::TYPE::LOOP, true);
	else
		model->Setup_Animation((_uint)CLIP_TWO::STAND2, CAnimation::TYPE::LOOP, true);
}

_bool CEnemy::Hit(_double TimeDelta)
{
	srand((unsigned int)time(nullptr));
	int iRandom = rand() % 2;

	if(m_eType == TYPE::HUMANOID)
		model->Setup_Animation((_uint)CLIP::HIT1, CAnimation::TYPE::ONE, true);
	else
		model->Setup_Animation((_uint)CLIP_TWO::HIT1, CAnimation::TYPE::ONE, true);

	if (model->AnimationIsFinish())
	{
		m_bHit = false;
		m_bNuckBackFinish = false;
		m_fNuckBackTimer = 0.0f;
		return true;
	}
	else
		return false;
}

void CEnemy::NuckBack(_double TimeDelta)
{
	m_fNuckBackTimer += TimeDelta;
	if (m_fNuckBackTimer >= m_fNuckBackTimeOut)
	{
		m_bNuckBackFinish = true;
		m_fNuckBackTimer = 0.0f;
	}

	if (!m_bNuckBackFinish)
	{
		_vector vPlayerPos = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);
		_vector vCurrentPos = transform->Get_State(CTransform::STATE_POSITION);
		_vector vDir = m_pPlayerTransform->Get_State(CTransform::STATE_LOOK);
		transform->LookAt(vPlayerPos);

		//_vector vDir = vCurrentPos - vPlayerPos;
		vDir = XMVector3Normalize(vDir);

		_vector vPos;
		vPos = vCurrentPos + (vDir) * 23.f;
		vCurrentPos = XMVectorLerp(vCurrentPos, vPos, TimeDelta * 0.6f);
		transform->Set_State(CTransform::STATE_POSITION, vCurrentPos);
	}

}

void CEnemy::AnimationState(_double TimeDelta)
{
	if (m_bHit)
	{
		Hit(TimeDelta);
		//NuckBack(TimeDelta);
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

			if (vDistance < 40.f)
			{
				if (m_bRotationFinish)
				{
					if (vDistance < m_fAttackRange)
						m_bAttack = true;
					else
						Trace(TimeDelta);
				}
			}

			if (m_bAttack)
				Attack(TimeDelta);

			if (model->AnimationIsFinish())
				Idle(TimeDelta);
		}
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

	Safe_Release(renderer);
	Safe_Release(transform);
	Safe_Release(model);
	Safe_Release(shader);
	Safe_Release(collider);
}

void CEnemy::OnCollisionEnter(CCollider * src, CCollider * dest)
{
	//몬스터 끼리 밀어내기 처리
	CEnemy* pEnemy = dynamic_cast<CEnemy*>(src->GetOwner());
	CEnemy* pAotherEnemy = dynamic_cast<CEnemy*>(dest->GetOwner());
	//둘다 에너미 타입이고 기본 충돌체일때
	if (pEnemy && pAotherEnemy)
	{
		if (model->AnimationCompare((_uint)CLIP::ATTACK1) ||
			model->AnimationCompare((_uint)CLIP::ATTACK2) ||
			model->AnimationCompare((_uint)CLIP::ATTACK3))
			return;

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


	//플레이어 웨폰과 충돌처리
	CCharacter* pPlayer = dynamic_cast<CCharacter*>(dest->GetOwner());
	if (pPlayer)
	{
		CCollider* pWeaponCollider = pPlayer->GetWeaponCollider();
		if (dest->Compare(pWeaponCollider))
		{
			m_bHit = true;
		}
	}

}

void CEnemy::OnCollisionStay(CCollider * src, CCollider * dest)
{

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
