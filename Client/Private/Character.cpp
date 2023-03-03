#include "pch.h"
#include "..\Public\Character.h"

#include "GameInstance.h"
#include "Weapon.h"
#include "Bone.h"
#include "Enemy.h"

//Bip001?(리얼 루트본) Bip001Pelvis (척추) R3KalieninaMd010031 (000)
CCharacter::CCharacter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CCharacter::CCharacter(const CCharacter & rhs)
	: CGameObject(rhs)
	, ANIM_DESC(rhs.ANIM_DESC)
{
}

HRESULT CCharacter::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	CGameInstance::GetInstance()->CreateTimer(L"Combo");

	ZeroMemory(&ANIM_DESC, sizeof(CAnimation::ANIMATION_DESC));
	SetAnimation(CLIP::STAND2, CAnimation::TYPE::LOOP);

	return S_OK;
}

HRESULT CCharacter::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(AddComponents()))
		return E_FAIL;
	
	if (FAILED(AddWeapon()))
		return E_FAIL;

	bone = mModel->GetBonePtr("Bip001");

	return S_OK;
}

void CCharacter::Tick(_double TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	pGameInstance->AddCollider(mCollider);
	pGameInstance->AddCollider(mEnemyCheckCollider);

	__super::Tick(TimeDelta);

	KeyInput(TimeDelta);
	Dash(TimeDelta);
	Attack();

	AnimationControl(TimeDelta);

	CameraSocketUpdate();
}

void CCharacter::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);
	
	NullTargetErase();
	FindNearTarget();

	_matrix transMatrix = XMLoadFloat4x4(&bone->GetCombinedMatrix()) * XMLoadFloat4x4(&mTransform->Get_WorldMatrix());
	mCollider->Update(transMatrix);
	mEnemyCheckCollider->Update(XMLoadFloat4x4(&mTransform->Get_WorldMatrix()));

	if (nullptr != mRenderer)
		mRenderer->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);
}

HRESULT CCharacter::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetupShaderResources()))
		return E_FAIL;

	_uint MeshCount = mModel->Get_MeshCount();
	for (_uint i = 0; i < MeshCount; ++i)
	{
		if (!strcmp("Cheek", mModel->GetMeshName(i)) || !strcmp("Cheek01", mModel->GetMeshName(i)))
			continue;

		mModel->Setup_ShaderMaterialResource(mShader, "g_DiffuseTexture", i, aiTextureType::aiTextureType_DIFFUSE);
		//m_pModelCom->SetUp_ShaderMaterialResource(m_pShaderCom, "g_AmbientTexture", i, aiTextureType_AMBIENT);

		mModel->Setup_BoneMatrices(mShader, "g_BoneMatrix", i);
		mShader->Begin(0);
		mModel->Render(i);
	}
	
	return S_OK;
}

void CCharacter::RenderGUI()
{
	ImGui::Begin("Transform");
	_float3 vPos;
	XMStoreFloat3(&vPos, mTransform->Get_State(CTransform::STATE_POSITION));

	ImGui::InputFloat3("Position", (_float*)&vPos);
	ImGui::InputInt("Attack ", (int*)&m_iCurAttackCount);

	if (nullptr != m_pNearEnemy)
	{
		_float4 vTargetPos = m_pNearEnemy->GetPosition();
		ImGui::InputFloat4("Target Position", (float*)&vTargetPos);
	}
	else
	{
		_float4 vTargetPos = { 0.f, 0.f, 0.f, 0.f };
		ImGui::InputFloat4("Target Position", (float*)&vTargetPos);
	}

	ImGui::End();
}

const CBone * CCharacter::GetBone(const char * szBoneName) const
{
	return mModel->GetBonePtr(szBoneName);
}

HRESULT CCharacter::AddWeapon()
{
	//레이어 삭제시 삭제됨
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	CWeapon::OWNER_DESC descOwner;
	descOwner.pModel = mModel;
	descOwner.pWeaponCase = mModel->GetBonePtr("WeaponCase1");
	descOwner.pTransform = mTransform;

	if (nullptr == (pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("proto_obj_kamui_weapon"), L"kamui_weapon", L"weapon", &descOwner)))
		return E_FAIL;

	return S_OK;
}

HRESULT CCharacter::AddComponents()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("proto_com_renderer"), TEXT("com_renderer"), (CComponent**)&mRenderer)))
		return E_FAIL;

	CTransform::TRANSFORM_DESC TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORM_DESC));

	TransformDesc.fMoveSpeed = m_fMoveSpeed;
	TransformDesc.fRotationSpeed = XMConvertToRadians(m_fRotationSpeed);

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("proto_com_transform"), TEXT("com_transform"), (CComponent**)&mTransform, &TransformDesc)))
		return E_FAIL;

	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORM_DESC));

	TransformDesc.fMoveSpeed = m_fMoveSpeed;
	TransformDesc.fRotationSpeed = XMConvertToRadians(230.f);

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("proto_com_transform"), TEXT("com_camera_socket_transform"), (CComponent**)&mCameraSocketTransform, &TransformDesc)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("proto_com_shader_vtxanimmodel"), TEXT("com_shader"), (CComponent**)&mShader)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("proto_com_model_kamui"), TEXT("com_model"), (CComponent**)&mModel)))
		return E_FAIL;

	mTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(31.5f, 0.f, 19.5f, 1.f));

	CCollider::COLLIDER_DESC collDesc;
	collDesc.owner = this;
	collDesc.vCenter = _float3(0.f, 0.f, 0.f);
	collDesc.vExtants = _float3(2.f, 2.f, 2.f);
	collDesc.vRotaion = _float3(0.f, 0.f, 0.f);

	if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("proto_com_sphere_collider"), TEXT("com_collider"), (CComponent**)&mCollider, &collDesc)))
		return E_FAIL;

	ZeroMemory(&collDesc, sizeof(collDesc));
	collDesc.owner = this;
	collDesc.vCenter = _float3(0.f, 0.f, 0.f);
	collDesc.vExtants = _float3(20.f, 20.f, 20.f);
	collDesc.vRotaion = _float3(0.f, 0.f, 0.f);

	if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("proto_com_sphere_collider"), TEXT("com_collider_check"), (CComponent**)&mEnemyCheckCollider, &collDesc)))
		return E_FAIL;

	mEnemyCheckCollider->SetColor(_float4(1.f, 1.f, 0.f, 1.f));

	return S_OK;
}

HRESULT CCharacter::SetupShaderResources()
{
	if (nullptr == mShader)
		return E_FAIL;

	CGameInstance* pInstance = CGameInstance::GetInstance();

	if (FAILED(mTransform->Setup_ShaderResource(mShader, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(mShader->SetMatrix("g_ViewMatrix", &pInstance->Get_Transform_float4x4(CPipeLine::TS_VIEW))))
		return E_FAIL;

	if (FAILED(mShader->SetMatrix("g_ProjMatrix", &pInstance->Get_Transform_float4x4(CPipeLine::TS_PROJ))))
		return E_FAIL;

	if (FAILED(mShader->SetRawValue("g_vCamPosition", &pInstance->Get_CamPosition(), sizeof(_float3))))
		return E_FAIL;

	const LIGHT_DESC* LightDesc = pInstance->GetLightDesc(0);
	if (nullptr == LightDesc)
		return E_FAIL;

	if (FAILED(mShader->SetRawValue("g_vLightDir", &LightDesc->vDirection, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(mShader->SetRawValue("g_vLightDiffuse", &LightDesc->vDiffuse, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(mShader->SetRawValue("g_vLightSpecular", &LightDesc->vSpecular, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(mShader->SetRawValue("g_vLightAmbient", &LightDesc->vAmbient, sizeof(_float4))))
		return E_FAIL;

	return S_OK;
}

void CCharacter::KeyInput(_double TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	InputWASD(TimeDelta);

	if (mModel->AnimationIsFinish())
		Idle();
}

void CCharacter::Dash(_double TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (m_bAttacking)
	{
		m_bDashable = false;
		if (mModel->AnimationIsPreFinish())
			m_bDashable = true;
	}
	else
		m_bDashable = true;

	if (AnimationCompare(CLIP::MOVE1) || AnimationCompare(CLIP::MOVE2))
	{
		if (mModel->AnimationIsPreFinish())
		{
			m_bDashable = true;

			m_bFrontDashReady = false;
			m_bRightDashReady = false;
			m_bLeftDashReady = false;

			m_bAttackable = true;

			m_fDashFrontTimer = 0.0;
			m_fDashLeftTimer = 0.0;
			m_fDashRightTimer = 0.0;
		}
	}

	//정면 대쉬
	if (m_bDashable)
	{
		if (!m_bFrontDashReady)
		{
			if (pGameInstance->Input_KeyState_Custom(DIK_W) == KEY_STATE::AWAY)
			{
				m_bFrontDashReady = true;
				m_bLeftDashReady = false;
				m_bRightDashReady = false;
			}
		}
		else
		{
			if (pGameInstance->Input_KeyState_Custom(DIK_W) == KEY_STATE::TAP)
			{
				SetAnimation(CLIP::MOVE1, CAnimation::ONE);
				m_bAttackable = false;
				m_bMoveable = false;
				m_bDashable = false;
			}
		}
	}

	//왼쪽 대쉬
	if (m_bDashable)
	{
		if (!m_bLeftDashReady)
		{
			if (pGameInstance->Input_KeyState_Custom(DIK_A) == KEY_STATE::AWAY)
			{
				m_bFrontDashReady = false;
				m_bLeftDashReady = true;
				m_bRightDashReady = false;
			}
		}
		else
		{
			if (pGameInstance->Input_KeyState_Custom(DIK_A) == KEY_STATE::TAP)
			{
				SetAnimation(CLIP::MOVE1, CAnimation::ONE);
				m_bAttackable = false;
				m_bMoveable = false;
				m_bDashable = false;
			}
		}
	}

	//오른쪽 대쉬
	if (m_bDashable)
	{
		if (!m_bRightDashReady)
		{
			if (pGameInstance->Input_KeyState_Custom(DIK_D) == KEY_STATE::AWAY)
			{
				m_bFrontDashReady = false;
				m_bLeftDashReady = false;
				m_bRightDashReady = true;
			}
		}
		else
		{
			if (pGameInstance->Input_KeyState_Custom(DIK_D) == KEY_STATE::TAP)
			{
				SetAnimation(CLIP::MOVE1, CAnimation::ONE);
				m_bAttackable = false;
				m_bMoveable = false;
				m_bDashable = false;
			}
		}
	}

	//백 대쉬
	if (m_bDashable)
	{
		if (pGameInstance->Input_KeyState_Custom(DIK_Z) == KEY_STATE::TAP)
		{
			SetAnimation(CLIP::MOVE2, CAnimation::ONE);
			m_bAttackable = false;
			m_bMoveable = false;
			m_bDashable = false;
		}
	}

	if (m_bFrontDashReady)
	{
		m_fDashFrontTimer += TimeDelta;
		if (m_fDashFrontTimer >= m_fDashTimeOut)
		{
			m_bFrontDashReady = false;
			m_fDashFrontTimer = 0.0;
		}
	}
	else
		m_fDashFrontTimer = 0.0;

	if (m_bLeftDashReady)
	{
		m_fDashLeftTimer += TimeDelta;
		if (m_fDashLeftTimer >= m_fDashTimeOut)
		{
			m_bLeftDashReady = false;
			m_fDashLeftTimer = 0.0;
		}
	}
	else
		m_fDashLeftTimer = 0.0;

	if (m_bRightDashReady)
	{
		m_fDashRightTimer += TimeDelta;
		if (m_fDashRightTimer >= m_fDashTimeOut)
		{
			m_bRightDashReady = false;
			m_fDashRightTimer = 0.0;
		}
	}
	else
		m_fDashRightTimer = 0.0;

}

void CCharacter::Idle()
{
	SetAnimation(CLIP::STAND2, CAnimation::TYPE::LOOP);
}

void CCharacter::MoveStop(_double TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	m_bMove = false;
	m_bRun = false;
	m_bMoveable = true;

	if (AnimationCompare(CLIP::RUN) || AnimationCompare(CLIP::RUN_START))
	{
		SetAnimation(CLIP::STOP, CAnimation::TYPE::ONE);
	}

}

void CCharacter::Attack()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (!m_bAttackable)
		return;

	if (pGameInstance->Input_MouseState_Custom(DIMK_LB) == KEY_STATE::TAP)
	{
		if (m_bAttacking)
		{
			m_bMoveable = false;
			m_bDashable = false;
			m_bAttacking = true;

			if (m_iCurAttackCount == CLIP::ATTACK5)
			{
				if (mModel->AnimationIsFinish())
					SetAnimation((CCharacter::CLIP)m_iCurAttackCount, CAnimation::TYPE::ONE);
			}
			else
			{
				if (mModel->AnimationIsPreFinish())
				{
					m_iCurAttackCount++;
					if (m_iCurAttackCount >= m_iAttackCount)
						m_iCurAttackCount = 0;

					SetAnimation((CCharacter::CLIP)m_iCurAttackCount, CAnimation::TYPE::ONE);
				}
			}
		}
		else
		{
			m_iCurAttackCount = 0;
			m_bMoveable = false;
			m_bAttacking = true;
			SetAnimation((CCharacter::CLIP)m_iCurAttackCount, CAnimation::TYPE::ONE);
		}
	}

	if (!AnimationCompare(CLIP::ATTACK1) &&
		!AnimationCompare(CLIP::ATTACK2) &&
		!AnimationCompare(CLIP::ATTACK3) &&
		!AnimationCompare(CLIP::ATTACK4) &&
		!AnimationCompare(CLIP::ATTACK5))
	{
		m_iCurAttackCount = 0;
		m_bAttacking = false;
		m_bMoveable = true;
	}
}

void CCharacter::NullTargetErase()
{
	for (auto& iter = m_Enemys.begin(); iter != m_Enemys.end(); )
	{
		if (static_cast<CGameObject*>(*iter)->IsDestroy() || 
			(*iter) == nullptr)
		{
			iter = m_Enemys.erase(iter);
		}
		else
			++iter;
	}
}

_bool CCharacter::FindTargetFromList(CGameObject * pObject)
{
	_bool ret = false;
	for (auto& enemy : m_Enemys)
	{
		if (enemy == (CEnemy*)pObject)
			return true;
	}

	return false;
}

void CCharacter::DeleteTargetFromList(CGameObject * pObject)
{
	for (auto iter = m_Enemys.begin(); iter != m_Enemys.end();)
	{
		if ((*iter)->Compare(pObject))
		{
			if (m_pNearEnemy == (CEnemy*)pObject)
				m_pNearEnemy = nullptr;

			iter = m_Enemys.erase(iter);
		}
		else
			++iter;
	}
}

void CCharacter::FindNearTarget()
{
	m_pNearEnemy = nullptr;

	_float fNear = FLT_MAX;
	_vector vPos = mTransform->Get_State(CTransform::STATE_POSITION);
	for (auto& enemy : m_Enemys)
	{
		CTransform* pTransform = (CTransform*)enemy->Find_Component(L"com_transform");
		_vector vTargetPos = pTransform->Get_State(CTransform::STATE_POSITION);

		float fDist = XMVectorGetX(XMVector3Length(vTargetPos - vPos));
		if (fDist < fNear)
		{
			fNear = fDist;
			m_pNearEnemy = enemy;
		}
	}
}

_float3 CCharacter::LockOnCameraPosition()
{
	if(nullptr == m_pNearEnemy)
		return _float3();

	_float3 vLockPosition;
	_vector vDir = (XMLoadFloat4(&m_pNearEnemy->GetPosition()) - mTransform->Get_State(CTransform::STATE_POSITION)) * 0.7f;
	_float fHeight = XMVectorGetY(mCameraSocketTransform->Get_State(CTransform::STATE_POSITION));
	vDir = XMVectorSetY(vDir, fHeight);

	XMStoreFloat3(&vLockPosition, vDir);

	return vLockPosition;
}

void CCharacter::SetAnimation(CLIP eClip, CAnimation::TYPE eAnimationType)
{
	ANIM_DESC.Clip = eClip;
	ANIM_DESC.Type = eAnimationType;
}

void CCharacter::AnimationControl(_double TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	mModel->Setup_Animation(ANIM_DESC.Clip, true);

	if (AnimationCompare(CLIP::RUN))
	{
		mModel->Play_Animation(TimeDelta, mTransform, ANIM_DESC.Type, 0.01f);
	}
	else
		mModel->Play_Animation(TimeDelta, mTransform, ANIM_DESC.Type);

}

void CCharacter::CameraSocketUpdate()
{
	_vector vLook = XMVector3Normalize(mTransform->Get_State(CTransform::STATE_LOOK));
	_vector cameraPos = mTransform->Get_State(CTransform::STATE_POSITION);
	cameraPos = XMVectorSetY(cameraPos, XMVectorGetY(cameraPos) + 2.f);
	
	//플레이어 뒤로 좀 미는거 수정
	cameraPos = cameraPos - (vLook * 4.f);

	mCameraSocketTransform->Set_State(CTransform::STATE_POSITION, cameraPos);
}

void CCharacter::ForwardRotaion(_double TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	_matrix cameraMatrix = pGameInstance->Get_Transform_Matrix_Inverse(CPipeLine::TRANSFORM_STATE::TS_VIEW);
	_vector vCameraRight = cameraMatrix.r[0];
	_vector vCameraLook = XMVector3Normalize(XMVector3Cross(vCameraRight, VECTOR_UP));
	_vector vPlayerLook = XMVector3Normalize(mTransform->Get_State(CTransform::STATE_LOOK));

	_float fRadianAngle = acos(XMVectorGetX(XMVector3Dot(vCameraLook, vPlayerLook)));
	_float AxisY = XMVectorGetY(XMVector3Cross(vPlayerLook, vCameraLook));

	if (isnan(fRadianAngle))
		fRadianAngle = 0.0f;

	if (AxisY >= 0.f)
		mTransform->Rotate(VECTOR_UP, TimeDelta * fRadianAngle);
	else
		mTransform->Rotate(VECTOR_UP, TimeDelta * -fRadianAngle);

}

void CCharacter::BackwardRotaion(_double TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	_matrix cameraMatrix = pGameInstance->Get_Transform_Matrix_Inverse(CPipeLine::TRANSFORM_STATE::TS_VIEW);
	_vector vCameraRight = cameraMatrix.r[0];
	_vector vCameraLook = -XMVector3Normalize(XMVector3Cross(vCameraRight, VECTOR_UP));
	_vector vPlayerLook = XMVector3Normalize(mTransform->Get_State(CTransform::STATE_LOOK));

	_float fRadianAngle = acos(XMVectorGetX(XMVector3Dot(vCameraLook, vPlayerLook)));
	_float AxisY = XMVectorGetY(XMVector3Cross(vPlayerLook, vCameraLook));

	if (isnan(fRadianAngle))
		fRadianAngle = 0.0f;

	if (AxisY >= 0.f)
		mTransform->Rotate(VECTOR_UP, TimeDelta * fRadianAngle);
	else
		mTransform->Rotate(VECTOR_UP, TimeDelta * -fRadianAngle);
}

void CCharacter::RightRotation(_double TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	_matrix cameraMatrix = pGameInstance->Get_Transform_Matrix_Inverse(CPipeLine::TRANSFORM_STATE::TS_VIEW);
	_vector vCameraRight = cameraMatrix.r[0];
	_vector vCameraLook = -XMVector3Normalize(XMVector3Cross(vCameraRight, VECTOR_UP));
	_vector vPlayerRight = XMVector3Normalize(mTransform->Get_State(CTransform::STATE_RIGHT));

	_float fRadianAngle = acos(XMVectorGetX(XMVector3Dot(vCameraLook, vPlayerRight)));
	_float AxisY = XMVectorGetY(XMVector3Cross(vPlayerRight, vCameraLook));

	if (isnan(fRadianAngle))
		fRadianAngle = 0.0f;

	if (AxisY >= 0.f)
		mTransform->Rotate(VECTOR_UP, TimeDelta * fRadianAngle);
	else
		mTransform->Rotate(VECTOR_UP, TimeDelta * -fRadianAngle);

}

void CCharacter::LeftRotation(_double TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	_matrix cameraMatrix = pGameInstance->Get_Transform_Matrix_Inverse(CPipeLine::TRANSFORM_STATE::TS_VIEW);
	_vector vCameraRight = cameraMatrix.r[0];
	_vector vCameraLook = XMVector3Normalize(XMVector3Cross(vCameraRight, VECTOR_UP));
	_vector vPlayerRight = XMVector3Normalize(mTransform->Get_State(CTransform::STATE_RIGHT));

	_float fRadianAngle = acos(XMVectorGetX(XMVector3Dot(vCameraLook, vPlayerRight)));
	_float AxisY = XMVectorGetY(XMVector3Cross(vPlayerRight, vCameraLook));
	
	if (isnan(fRadianAngle))
		fRadianAngle = 0.0f;

	if (AxisY >= 0.f)
		mTransform->Rotate(VECTOR_UP, TimeDelta * fRadianAngle);
	else
		mTransform->Rotate(VECTOR_UP, TimeDelta * -fRadianAngle);
}

void CCharacter::Movement(_double TimeDelta)
{
	m_bMove = true;

	if (AnimationCompare(CLIP::STAND1) || AnimationCompare(CLIP::STAND2) || AnimationCompare(CLIP::RUN_START_END) || AnimationCompare(CLIP::STOP))
	{
		SetAnimation(CLIP::RUN_START, CAnimation::TYPE::ONE);
	}
	else if (AnimationCompare(CLIP::RUN_START))
	{
		if (FinishCheckPlay(CLIP::RUN, CAnimation::TYPE::LOOP))
			m_bRun = true;
	}
	else if (AnimationCompare(CLIP::RUN))
	{
		mTransform->MoveForward(TimeDelta);
	}
	else
	{
		FinishCheckPlay(CLIP::RUN_START, CAnimation::TYPE::ONE);
	}
}

void CCharacter::InputWASD(_double TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (pGameInstance->Input_KeyState_Custom(DIK_W) == KEY_STATE::TAP)
		m_iWASDCount++;
	else if(pGameInstance->Input_KeyState_Custom(DIK_W) == KEY_STATE::AWAY)
		m_iWASDCount--;

	if (pGameInstance->Input_KeyState_Custom(DIK_A) == KEY_STATE::TAP)
		m_iWASDCount++;
	else if (pGameInstance->Input_KeyState_Custom(DIK_A) == KEY_STATE::AWAY)
		m_iWASDCount--;

	if (pGameInstance->Input_KeyState_Custom(DIK_S) == KEY_STATE::TAP)
		m_iWASDCount++;
	else if (pGameInstance->Input_KeyState_Custom(DIK_S) == KEY_STATE::AWAY)
		m_iWASDCount--;

	if (pGameInstance->Input_KeyState_Custom(DIK_D) == KEY_STATE::TAP)
		m_iWASDCount++;
	else if (pGameInstance->Input_KeyState_Custom(DIK_D) == KEY_STATE::AWAY)
		m_iWASDCount--;

	if (m_bMove &&
		pGameInstance->Input_KeyState_Custom(DIK_W) != KEY_STATE::HOLD &&
		pGameInstance->Input_KeyState_Custom(DIK_A) != KEY_STATE::HOLD &&
		pGameInstance->Input_KeyState_Custom(DIK_S) != KEY_STATE::HOLD &&
		pGameInstance->Input_KeyState_Custom(DIK_D) != KEY_STATE::HOLD)
	{
		MoveStop(TimeDelta);
	}

	TimeDelta = TimeDelta / m_iWASDCount;

	if (pGameInstance->Input_KeyState_Custom(DIK_W) == KEY_STATE::HOLD)
	{
		if (!m_bMoveable)
			return;

		ForwardRotaion(TimeDelta);
		Movement(TimeDelta);
	}

	if (pGameInstance->Input_KeyState_Custom(DIK_S) == KEY_STATE::HOLD)
	{
		if (!m_bMoveable)
			return;

		BackwardRotaion(TimeDelta);
		Movement(TimeDelta);
	}

	if (pGameInstance->Input_KeyState_Custom(DIK_A) == KEY_STATE::HOLD)
	{
		if (!m_bMoveable)
			return;

		LeftRotation(TimeDelta);
		Movement(TimeDelta);
	}

	if (pGameInstance->Input_KeyState_Custom(DIK_D) == KEY_STATE::HOLD)
	{
		if (!m_bMoveable)
			return;

		RightRotation(TimeDelta);
		Movement(TimeDelta);
	}
}

_bool CCharacter::AnimationCompare(CLIP eClip)
{
	return ANIM_DESC.Clip == eClip;
}

_bool CCharacter::FinishCheckPlay(CLIP eClip, CAnimation::TYPE eAnimationType)
{
	if (mModel->AnimationIsFinish())
	{
		SetAnimation(eClip, eAnimationType);
		return true;
	}

	return false;
}

CCharacter* CCharacter::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCharacter*	pInstance = new CCharacter(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CCharacter");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCharacter::Clone(void* pArg)
{
 	CCharacter* pInstance = new CCharacter(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CCharacter");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCharacter::Free()
{ 
	__super::Free();

	Safe_Release(mEnemyCheckCollider);
	Safe_Release(mCollider);
	Safe_Release(mRenderer);
	Safe_Release(mTransform);
	Safe_Release(mCameraSocketTransform);
	Safe_Release(mModel);
	Safe_Release(mShader);
}

void CCharacter::OnCollisionEnter(CCollider * src, CCollider * dest)
{
	if (src->Compare(mEnemyCheckCollider))
	{
		CEnemy* pEnemy = dynamic_cast<CEnemy*>(dest->GetOwner());
		if (pEnemy)
		{
			if (!FindTargetFromList(dest->GetOwner()))
				m_Enemys.push_back(pEnemy);
		}
	}
}

void CCharacter::OnCollisionStay(CCollider * src, CCollider * dest)
{
	if (src->Compare(mCollider))
	{
		CEnemy* pEnemy = dynamic_cast<CEnemy*>(dest->GetOwner());
		pEnemy->Destroy();

		//_vector pos = XMVectorSet(31.5f, 0.f, 19.5f, 1.f);
		//mTransform->Set_State(CTransform::STATE_POSITION, pos);
	}
}

void CCharacter::OnCollisionExit(CCollider * src, CCollider * dest)
{
	if (src->Compare(mEnemyCheckCollider))
	{
		CEnemy* pEnemy = dynamic_cast<CEnemy*>(dest->GetOwner());
		if (pEnemy)
			DeleteTargetFromList(dest->GetOwner());
	}
}
