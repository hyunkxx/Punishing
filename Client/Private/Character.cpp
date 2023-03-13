#include "pch.h"
#include "..\Public\Character.h"

#include "GameInstance.h"
#include "CollisionManager.h"
#include "ApplicationManager.h"
#include "Weapon.h"
#include "Bone.h"
#include "Enemy.h"
#include "Wall.h"

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
	m_pAppManager = CApplicationManager::GetInstance();
	ZeroMemory(&m_matrixPrevPos, sizeof(_float4x4));

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(AddComponents()))
		return E_FAIL;
	
	if (FAILED(AddWeapon()))
		return E_FAIL;

	ZeroMemory(&ANIM_DESC, sizeof(CAnimation::ANIMATION_DESC));
	SetAnimation(CLIP::STAND2, CAnimation::TYPE::LOOP);

	bone = mModel->GetBonePtr("Bip001");
	m_pWeaponBone = mModel->GetBonePtr("WeaponCase1");


	return S_OK;
}

void CCharacter::Tick(_double TimeDelta)
{
	TimeDelta = Freeze(TimeDelta);

	__super::Tick(TimeDelta);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	pGameInstance->AddCollider(mCollider);
	pGameInstance->AddCollider(mWallCheckCollider, 1);
	pGameInstance->AddCollider(mEnemyCheckCollider);
	pGameInstance->AddCollider(mDashCheckCollider);
	pGameInstance->AddCollider(mWeaponCollider);

	if (m_bHit)
	{
		if (mModel->AnimationIsPreFinishCustom(0.15))
		{
			m_bHit = false;
			m_bMoveable = true;
			m_bDashable = true;
			m_bAttackable = true;
		}
	}

	KeyInput(TimeDelta);
	Dash(TimeDelta);
	Attack(TimeDelta);

	if(!m_WallHit)
		XMStoreFloat3(&vPrevPosition, mTransform->Get_State(CTransform::STATE_POSITION));

	AnimationControl(TimeDelta);
	SkillColliderControl(TimeDelta);
	//PositionHold(TimeDelta);

	CameraSocketUpdate();
	TargetListDeastroyCehck();
}

void CCharacter::LateTick(_double TimeDelta)
{
	TimeDelta = Freeze(TimeDelta);
	__super::LateTick(TimeDelta);

	FindNearTarget();

	_matrix transMatrix = XMLoadFloat4x4(&bone->GetCombinedMatrix()) * XMLoadFloat4x4(&mTransform->Get_WorldMatrix());
	mCollider->Update(transMatrix);
	mWallCheckCollider->Update(transMatrix);
	mEnemyCheckCollider->Update(XMLoadFloat4x4(&mTransform->Get_WorldMatrix()));
	//mWeaponCollider->Update(XMLoadFloat4x4(&mTransform->Get_WorldMatrix()));
	mWeaponCollider->Update(XMLoadFloat4x4(&m_pWeaponBone->GetOffSetMatrix()) * XMLoadFloat4x4(&m_pWeaponBone->GetCombinedMatrix()) * XMLoadFloat4x4(&mModel->GetLocalMatrix()) *  XMLoadFloat4x4(&mTransform->Get_WorldMatrix()));
	mDashCheckCollider->Update(XMLoadFloat4x4(&m_matrixPrevPos));

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
	//_float3 vPos;
	//XMStoreFloat3(&vPos, mTransform->Get_State(CTransform::STATE_POSITION));

	//ImGui::InputFloat3("Position", (_float*)&vPos);
	//ImGui::InputInt("Attack ", (int*)&m_iCurAttackCount);

	//if (nullptr != m_pNearEnemy && !m_pNearEnemy->IsDestroy())
	//{
	//	CTransform* pTransform = static_cast<CTransform*>(static_cast<CGameObject*>(m_pNearEnemy)->Find_Component(L"com_transform"));
	//	if (pTransform)
	//	{
	//		_float4 vTargetPos = m_pNearEnemy->GetPosition();
	//		ImGui::InputFloat4("Target Position", (float*)&vTargetPos);
	//	}
	//}
	//else
	//{
 //		_float4 vTargetPos = { 0.f, 0.f, 0.f, 0.f };
	//	ImGui::InputFloat4("Target Position", (float*)&vTargetPos);
	//}

	//_int size = (_uint)m_Enemys.size();
	//ImGui::InputInt("NearEnemy List ", (int*)&size);
	//ImGui::InputInt("Look Enemy index", (int*)&m_iEnemyIndex);
	CCollisionManager* pCollManager = CCollisionManager::GetInstance();
	int Count = pCollManager->GetHasCollisionCount();
	ImGui::InputInt("Look Enemy index", &Count);

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
	collDesc.vExtents = _float3(1.2f, 1.2f, 1.2f);
	collDesc.vRotation = _float3(0.f, 0.f, 0.f);

	if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("proto_com_sphere_collider"), TEXT("com_collider"), (CComponent**)&mCollider, &collDesc)))
		return E_FAIL;

	ZeroMemory(&collDesc, sizeof(collDesc));
	collDesc.owner = this;
	collDesc.vCenter = _float3(0.f, 0.f, 0.f);
	collDesc.vExtents = _float3(2.f, 2.f, 2.f);
	collDesc.vRotation = _float3(0.f, 0.f, 0.f);

	if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("proto_com_sphere_collider"), TEXT("com_collider_dash_check"), (CComponent**)&mDashCheckCollider, &collDesc)))
		return E_FAIL;

	mDashCheckCollider->SetCollision(false);
	mDashCheckCollider->SetColor(_float4(1.f, 1.f, 0.f, 1.f));

	ZeroMemory(&collDesc, sizeof(collDesc));
	collDesc.owner = this;
	collDesc.vCenter = _float3(0.f, 0.f, 0.f);
	collDesc.vExtents = _float3(1.f, 1.f, 1.f);
	collDesc.vRotation = _float3(0.f, 0.f, 0.f);

	if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("proto_com_sphere_collider"), TEXT("com_collider_wallcheck"), (CComponent**)&mWallCheckCollider, &collDesc)))
		return E_FAIL;

	mWallCheckCollider->SetColor(_float4(1.f, 0.f, 1.f, 1.f));

	ZeroMemory(&collDesc, sizeof(collDesc));
	collDesc.owner = this;
	collDesc.vCenter = _float3(0.f, 0.f, 0.f);
	collDesc.vExtents = _float3(20.f, 20.f, 20.f);
	collDesc.vRotation = _float3(0.f, 0.f, 0.f);

	if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("proto_com_sphere_collider"), TEXT("com_collider_check"), (CComponent**)&mEnemyCheckCollider, &collDesc)))
		return E_FAIL;

	mEnemyCheckCollider->SetColor(_float4(1.f, 1.f, 0.f, 1.f));

	ZeroMemory(&collDesc, sizeof(collDesc));
	collDesc.owner = this;
	//collDesc.vCenter = _float3(0.f, 1.f, 1.f);
	//collDesc.vExtents = _float3(2.5f, 1.f, 1.f);
	//collDesc.vRotation = _float3(0.f, 0.f, 0.f);

	collDesc.vCenter = _float3(0.8f, 0.f, 0.f);
	collDesc.vExtents = _float3(2.5f, 0.3f, 0.3f);
	collDesc.vRotation = _float3(0.f, 0.f, 0.f);

	if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("proto_com_obb_collider"), TEXT("com_collider_weapon"), (CComponent**)&mWeaponCollider, &collDesc)))
		return E_FAIL;

	mWeaponCollider->SetActive(false);
	mWeaponCollider->SetColor(_float4(0.f, 1.f, 0.f, 1.f));

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

	InputMove(TimeDelta);

	if (pGameInstance->Input_KeyState_Custom(DIK_TAB) == KEY_STATE::TAP)
	{
		NearTargetChange();
	}

	//테스트 코드
	if (pGameInstance->Input_KeyState_Custom(DIK_P) == KEY_STATE::TAP)
	{
		m_bEvolution = !m_bEvolution;
		SetAnimation(CLIP::ATTACK51, CAnimation::TYPE::ONE);
		m_bAttackable = false;
		m_bMoveable = false;
		m_bDashable = false;
	}

	//넉백
	if (mModel->AnimationCompare(CLIP::ATTACK51))
	{
		if (!m_pAppManager->IsFreeze())
		{
			if (mModel->AnimationIsPreFinishCustom(0.55))
			{
				for (auto& pEnemy : m_Enemys)
				{
					_vector vEnemyPos = XMLoadFloat4(&pEnemy->GetPosition());
					_vector vPos = mTransform->Get_State(CTransform::STATE_POSITION);

					_float fLength = XMVectorGetX(XMVector3Length(vEnemyPos - vPos));
					if (fLength < 5.f)
					{
						pEnemy->SetNuckback(40.f);
					}
				}
			}
		}

	}
	
	if (pGameInstance->Input_KeyState_Custom(DIK_A) == KEY_STATE::TAP)
	{
		if(!m_bUseSkill)
			SkillA(TimeDelta);
	}

	if (pGameInstance->Input_KeyState_Custom(DIK_B) == KEY_STATE::TAP)
	{
		if (!m_bUseSkill)
			SkillB(TimeDelta);
	}

	if (pGameInstance->Input_KeyState_Custom(DIK_C) == KEY_STATE::TAP)
	{
		if (!m_bUseSkill)
			SkillC(TimeDelta);
	}

	if (mModel->AnimationCompare(CLIP::ATTACK51))
	{
		if (mModel->AnimationIsFinishEx())
		{
			m_bAttackable = true;
			m_bMoveable = true;
			m_bDashable = true;
		}
	}

	if (mModel->AnimationIsFinish())
		Idle();
}

void CCharacter::Dash(_double TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	mWeaponCollider->SetActive(false);

	if (m_bAttacking)
	{
		m_bDashable = false;
		if (mModel->AnimationIsPreFinish())
			m_bDashable = true;
	}
	else
		m_bDashable = true;

	if (m_bDashPrevPosSet)
	{
		m_fActiveDuration += TimeDelta;
		if (m_fActiveDuration >= m_fActiveTimeOut)
		{
			m_fActiveDuration = 0.0f;
			m_bDashPrevPosSet = false;
			mDashCheckCollider->SetActive(false);
		}
	}

	if (AnimationCompare(CLIP::MOVE1) || AnimationCompare(CLIP::MOVE2))
	{
		mCollider->SetActive(false);

		if (mModel->AnimationIsPreFinish())
		{
			mCollider->SetActive(true);
			m_bDashable = true;

			m_bRootMotion = true;
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
			if (pGameInstance->Input_KeyState_Custom(DIK_UPARROW) == KEY_STATE::AWAY)
			{
				m_bFrontDashReady = true;
				m_bLeftDashReady = false;
				m_bRightDashReady = false;
			}
		}
		else
		{
			if (pGameInstance->Input_KeyState_Custom(DIK_UPARROW) == KEY_STATE::TAP)
			{
				SetAnimation(CLIP::MOVE1, CAnimation::ONE);
				m_bAttackable = false;
				m_bMoveable = false;
				m_bDashable = false;

				SavePrevPos();
			}
		}
	}

	//왼쪽 대쉬
	if (m_bDashable)
	{
		if (!m_bLeftDashReady)
		{
			if (pGameInstance->Input_KeyState_Custom(DIK_LEFTARROW) == KEY_STATE::AWAY)
			{
				m_bFrontDashReady = false;
				m_bLeftDashReady = true;
				m_bRightDashReady = false;
			}
		}
		else
		{
			if (pGameInstance->Input_KeyState_Custom(DIK_LEFTARROW) == KEY_STATE::TAP)
			{
				SetAnimation(CLIP::MOVE1, CAnimation::ONE);
				m_bAttackable = false;
				m_bMoveable = false;
				m_bDashable = false;

				SavePrevPos();
			}
		}
	}

	//오른쪽 대쉬
	if (m_bDashable)
	{
		if (!m_bRightDashReady)
		{
			if (pGameInstance->Input_KeyState_Custom(DIK_RIGHTARROW) == KEY_STATE::AWAY)
			{
				m_bFrontDashReady = false;
				m_bLeftDashReady = false;
				m_bRightDashReady = true;
			}
		}
		else
		{
			if (pGameInstance->Input_KeyState_Custom(DIK_RIGHTARROW) == KEY_STATE::TAP)
			{
				SetAnimation(CLIP::MOVE1, CAnimation::ONE);
				m_bAttackable = false;
				m_bMoveable = false;
				m_bDashable = false;

				SavePrevPos();
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

			SavePrevPos();
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

void CCharacter::Attack(_double TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (!m_bAttackable)
		return;

	if (m_bAttacking)
	{
		mWeaponCollider->SetActive(true);
	}

	if ((m_bAttacking) && mModel->AnimationIsPreFinish())
	{
		mWeaponCollider->SetActive(false);
	}

	if (pGameInstance->Input_KeyState_Custom(DIK_LCONTROL) == KEY_STATE::TAP)
	{
		if (m_pNearEnemy)
			mTransform->LookAt(XMLoadFloat4(&m_pNearEnemy->GetPosition()));

		if (!m_bEvolution)
		{

			if (m_bAttacking)
			{
				m_bMoveable = false;
				m_bDashable = false;
				m_bAttacking = true;

				if (m_iCurAttackCount == CLIP::ATTACK5)
				{
					if (mModel->AnimationIsFinish())
					{
						m_bWeaponTimerOn = true;
						SetAnimation((CCharacter::CLIP)m_iCurAttackCount, CAnimation::TYPE::ONE);
					}
				}
				else
				{
					if (mModel->AnimationIsPreFinish())
					{
						m_iCurAttackCount++;
						if (m_iCurAttackCount >= m_iAttackCount)
						{
							m_iCurAttackCount = 0;
						}

						m_bWeaponTimerOn = true;
						SetAnimation((CCharacter::CLIP)m_iCurAttackCount, CAnimation::TYPE::ONE);
					}
				}
			}
			else
			{
				m_iCurAttackCount = 0;
				m_bMoveable = false;
				m_bAttacking = true;
				m_bWeaponTimerOn = true;
				SetAnimation((CCharacter::CLIP)m_iCurAttackCount, CAnimation::TYPE::ONE);
			}
		}
		else
		{
			if (m_bAttacking)
			{
				m_bMoveable = false;
				m_bDashable = false;
				m_bAttacking = true;

				if (m_iCurAttackCount == CLIP::ATTACK45)
				{
					if (mModel->AnimationIsPreFinish())
					{
						m_bWeaponTimerOn = true;
						SetAnimation(CCharacter::CLIP::ATTACK45, CAnimation::TYPE::ONE);
					}
				}
				else
				{
					if (mModel->AnimationIsPreFinish())
					{
						m_iCurAttackCount++;
						if (m_iCurAttackCount >= m_iAttackCount)
						{
							m_iCurAttackCount = 0;
						}

						m_bWeaponTimerOn = true;

						switch (m_iCurAttackCount)
						{
						case 1:
							SetAnimation(CCharacter::CLIP::ATTACK42, CAnimation::TYPE::ONE);
							break;
						case 2:
							SetAnimation(CCharacter::CLIP::ATTACK43, CAnimation::TYPE::ONE);
							break;
						case 3:
							SetAnimation(CCharacter::CLIP::ATTACK44, CAnimation::TYPE::ONE);
							break;
						case 4:
							SetAnimation(CCharacter::CLIP::ATTACK45, CAnimation::TYPE::ONE);
							break;
						}
						
					}
				}
			}
			else
			{
				m_iCurAttackCount = 0;
				m_bMoveable = false;
				m_bAttacking = true;
				m_bWeaponTimerOn = true;
				SetAnimation(CCharacter::CLIP::ATTACK41, CAnimation::TYPE::ONE);
			}
		}
	}

	if (!AnimationCompare(CLIP::ATTACK1) &&
		!AnimationCompare(CLIP::ATTACK2) &&
		!AnimationCompare(CLIP::ATTACK3) &&
		!AnimationCompare(CLIP::ATTACK4) &&
		!AnimationCompare(CLIP::ATTACK5) &&
		!AnimationCompare(CLIP::ATTACK11) &&
		!AnimationCompare(CLIP::ATTACK41) &&
		!AnimationCompare(CLIP::ATTACK42) &&
		!AnimationCompare(CLIP::ATTACK43) &&
		!AnimationCompare(CLIP::ATTACK44) &&
		!AnimationCompare(CLIP::ATTACK45))
	{
		mWeaponCollider->SetActive(false);
		m_iCurAttackCount = 0;
		m_bAttacking = false;
		m_bMoveable = true;
	}
}

void CCharacter::PositionHold(_double TimeDelta)
{
	//if (m_OverlappedInfo.empty())
	//	return;

	//_vector vNormal = XMVectorSet(0.f, 0.f, 0.f, 0.f);
	//_vector vPosition = XMVectorSet(0.f, 0.f, 0.f, 0.f);

	//for (auto& overlapInfo : m_OverlappedInfo)
	//{
	//	vNormal += XMLoadFloat3(&overlapInfo.vDir);
	//	vPosition += XMLoadFloat3(&overlapInfo.vPos);
	//}
	//vNormal = XMVector3Normalize(vNormal);
	//vNormal = XMVectorSetY(vNormal, 0.f);

	//vPosition = vPosition / m_OverlappedInfo.size();

	//_vector vCurrentPos = mTransform->Get_State(CTransform::STATE_POSITION);
	////_vector vDistance = vPosition - vCurrentPos;
	//vCurrentPos = vCurrentPos + vNormal;
	//mTransform->Set_State(CTransform::STATE_POSITION, vCurrentPos);

	//m_OverlappedInfo.clear();
}

void CCharacter::SkillA(_double TimeDelta)
{
	m_bUseSkill = true;
	m_bAttacking = true;
	SetAnimation(CLIP::ATTACK11, CAnimation::TYPE::ONE);
}

void CCharacter::SkillB(_double TimeDelta)
{
	m_bUseSkill = true;
	SetAnimation(CLIP::ATTACK21, CAnimation::TYPE::ONE);

}

void CCharacter::SkillC(_double TimeDelta)
{
	m_bUseSkill = true;
	SetAnimation(CLIP::ATTACK31, CAnimation::TYPE::ONE);

}

void CCharacter::SkillColliderControl(_double TimeDelta)
{
	if (m_bUseSkill)
	{
		if (mModel->AnimationIsFinishEx())
		{
			m_bUseSkill = false;
		}
	}

}

void CCharacter::TargetListDeastroyCehck()
{
	for (auto iter = m_Enemys.begin(); iter != m_Enemys.end(); )
	{
		if ((*iter)->IsDestroy())
		{
			if ((*iter) == m_pNearEnemy)
				m_pNearEnemy = nullptr;

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
	if (m_pNearEnemy != nullptr)
		return;

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

void CCharacter::NearTargetChange()
{
	if(m_iEnemyIndex < m_Enemys.size())
		m_iEnemyIndex++;

	int i = 0;
	for (auto iter = m_Enemys.begin(); iter != m_Enemys.end(); ++iter)
	{
		if (m_iEnemyIndex >= m_Enemys.size())
		{
			m_iEnemyIndex = 0;
			m_pNearEnemy = *(m_Enemys.begin());
		}

		if (m_iEnemyIndex == i)
		{
			m_pNearEnemy = *iter;
			return;
		}

		i++;
	}
}

_float3 CCharacter::LockOnCameraPosition()
{
	if(nullptr == m_pNearEnemy)
		return _float3();

	if(m_pNearEnemy->IsDestroy())
		return _float3();

	_float3 vLockPosition;

	_vector vDir = (XMLoadFloat4(&m_pNearEnemy->GetPosition()) - mTransform->Get_State(CTransform::STATE_POSITION)) * 0.7f;
	_float fHeight = XMVectorGetY(mCameraSocketTransform->Get_State(CTransform::STATE_POSITION));
	vDir = XMVectorSetY(vDir, fHeight);

	XMStoreFloat3(&vLockPosition, vDir);

	return vLockPosition;
}

void CCharacter::Hit()
{
	//대쉬중에 힛 스루
	if (mModel->AnimationCompare(CLIP::MOVE1) ||
		mModel->AnimationCompare(CLIP::MOVE2) ||
		mModel->AnimationCompare(CLIP::ATTACK11) ||
		mModel->AnimationCompare(CLIP::ATTACK21) ||
		mModel->AnimationCompare(CLIP::ATTACK31) ||
		mModel->AnimationCompare(CLIP::ATTACK12) ||
		mModel->AnimationCompare(CLIP::ATTACK32) ||
		mModel->AnimationCompare(CLIP::ATTACK51))
	{
		return;
	}

	if (m_bEvolution)
		return;

	m_bHit = true;
	m_bMoveable = false;
	m_bDashable = false;
	m_bAttackable = false;

	if (!mModel->AnimationCompare(CLIP::HIT1))
	{
		SetAnimation(CLIP::HIT1, CAnimation::TYPE::ONE);
	}
}

void CCharacter::RecvDamage(_float fDamage)
{
	m_fCurHp -= fDamage;
	if (m_fCurHp <= 0.f)
		m_bDie = true;
}

_double CCharacter::Freeze(_double TimeDelta)
{
	if (m_pAppManager->IsFreeze())
	{
		_vector CurTimeDelta = XMVectorSet(m_fCurTimeScale, m_fCurTimeScale, m_fCurTimeScale, m_fCurTimeScale);
		m_fCurTimeScale = XMVectorGetX(XMVectorLerp(CurTimeDelta, XMVectorSet(0.0, 0.0, 0.0, 0.0), TimeDelta * 0.8));

		_float3 vPrevPos;
		vPrevPos.x = m_matrixPrevPos._41;
		vPrevPos.y = 0.f;
		vPrevPos.z = m_matrixPrevPos._43;

		m_pAppManager->SetPlayerPrevPosition(vPrevPos);
	}

	if (m_fCurTimeScale <= 0.1)
	{
		m_bTimeStop = true;
		m_fCurTimeScale = 1.0;
	}

	if (m_bTimeStop)
	{
		m_fTimeStopLocal += TimeDelta;
		if (m_fTimeStopLocal >= m_fTimeStopTimeOut)
		{
			m_pAppManager->SetFreeze(false);
			m_fTimeStopLocal = 0.0;
			m_fCurTimeScale = 1.0;
			m_bTimeStop = false;
		}

		return TimeDelta;
	}
	else
		return TimeDelta = TimeDelta * m_fCurTimeScale;
}

void CCharacter::SavePrevPos()
{
	if (!m_bDashPrevPosSet && !m_bTimeStop)
	{
		m_bDashPrevPosSet = true;
		m_matrixPrevPos = mTransform->Get_WorldMatrix();
		m_matrixPrevPos._42 = 0.8f;
		mDashCheckCollider->SetActive(true);
	}
}

void CCharacter::SetAnimation(CLIP eClip, CAnimation::TYPE eAnimationType)
{
	ANIM_DESC.Clip = eClip;
	ANIM_DESC.Type = eAnimationType;
}

void CCharacter::AnimationControl(_double TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	mModel->Setup_Animation(ANIM_DESC.Clip, ANIM_DESC.Type, true);

	if (AnimationCompare(CLIP::RUN) ||
		AnimationCompare(CLIP::ATTACK1) ||
		AnimationCompare(CLIP::ATTACK2) ||
		AnimationCompare(CLIP::ATTACK3) ||
		AnimationCompare(CLIP::ATTACK4) ||
		AnimationCompare(CLIP::ATTACK5) ||
		AnimationCompare(CLIP::ATTACK11) ||
		AnimationCompare(CLIP::ATTACK21) ||
		AnimationCompare(CLIP::ATTACK31) ||
		AnimationCompare(CLIP::ATTACK12) ||
		AnimationCompare(CLIP::ATTACK22) ||
		AnimationCompare(CLIP::ATTACK32) ||
		AnimationCompare(CLIP::ATTACK41) ||
		AnimationCompare(CLIP::ATTACK42) ||
		AnimationCompare(CLIP::ATTACK43) ||
		AnimationCompare(CLIP::ATTACK44) ||
		AnimationCompare(CLIP::ATTACK45))
	{
		mModel->Play_Animation(TimeDelta, mTransform, 0.01f, m_bRootMotion);
	}
	else
		mModel->Play_Animation(TimeDelta, mTransform);
}

void CCharacter::CameraSocketUpdate()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (pGameInstance->Input_KeyState_Custom(DIK_DOWNARROW) == KEY_STATE(HOLD))
	{
		_vector vLook = XMVector3Normalize(mTransform->Get_State(CTransform::STATE_LOOK));
		_vector cameraPos = mTransform->Get_State(CTransform::STATE_POSITION);
		cameraPos = XMVectorSetY(cameraPos, XMVectorGetY(cameraPos) + 2.f);

		//플레이어 뒤로 좀 미는거 수정
		cameraPos = cameraPos + (vLook * 7.f);

		mCameraSocketTransform->Set_State(CTransform::STATE_POSITION, cameraPos);
	}
	else
	{
		_vector vLook = XMVector3Normalize(mTransform->Get_State(CTransform::STATE_LOOK));
		_vector cameraPos = mTransform->Get_State(CTransform::STATE_POSITION);
		cameraPos = XMVectorSetY(cameraPos, XMVectorGetY(cameraPos) + 2.f);

		//플레이어 뒤로 좀 미는거 수정
		cameraPos = cameraPos - (vLook * 5.f);

		mCameraSocketTransform->Set_State(CTransform::STATE_POSITION, cameraPos);
	}
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

void CCharacter::InputMove(_double TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (pGameInstance->Input_KeyState_Custom(DIK_UPARROW) == KEY_STATE::TAP)
		m_iWASDCount++;
	else if(pGameInstance->Input_KeyState_Custom(DIK_UPARROW) == KEY_STATE::AWAY)
		m_iWASDCount--;

	if (pGameInstance->Input_KeyState_Custom(DIK_LEFTARROW) == KEY_STATE::TAP)
		m_iWASDCount++;
	else if (pGameInstance->Input_KeyState_Custom(DIK_LEFTARROW) == KEY_STATE::AWAY)
		m_iWASDCount--;

	if (pGameInstance->Input_KeyState_Custom(DIK_DOWNARROW) == KEY_STATE::TAP)
		m_iWASDCount++;
	else if (pGameInstance->Input_KeyState_Custom(DIK_DOWNARROW) == KEY_STATE::AWAY)
		m_iWASDCount--;

	if (pGameInstance->Input_KeyState_Custom(DIK_RIGHTARROW) == KEY_STATE::TAP)
		m_iWASDCount++;
	else if (pGameInstance->Input_KeyState_Custom(DIK_RIGHTARROW) == KEY_STATE::AWAY)
		m_iWASDCount--;

	if (m_bMove &&
		pGameInstance->Input_KeyState_Custom(DIK_UPARROW) != KEY_STATE::HOLD &&
		pGameInstance->Input_KeyState_Custom(DIK_LEFTARROW) != KEY_STATE::HOLD &&
		pGameInstance->Input_KeyState_Custom(DIK_DOWNARROW) != KEY_STATE::HOLD &&
		pGameInstance->Input_KeyState_Custom(DIK_RIGHTARROW) != KEY_STATE::HOLD)
	{
		MoveStop(TimeDelta);
	}

	TimeDelta = TimeDelta / m_iWASDCount;

	if (pGameInstance->Input_KeyState_Custom(DIK_UPARROW) == KEY_STATE::HOLD)
	{
		if (!m_bMoveable)
			return;

		ForwardRotaion(TimeDelta);
		Movement(TimeDelta);
	}

	if (pGameInstance->Input_KeyState_Custom(DIK_DOWNARROW) == KEY_STATE::HOLD)
	{
		if (!m_bMoveable)
			return;

		BackwardRotaion(TimeDelta);
		Movement(TimeDelta);
	}

	if (pGameInstance->Input_KeyState_Custom(DIK_LEFTARROW) == KEY_STATE::HOLD)
	{
		if (!m_bMoveable)
			return;

		LeftRotation(TimeDelta);
		Movement(TimeDelta);
	}

	if (pGameInstance->Input_KeyState_Custom(DIK_RIGHTARROW) == KEY_STATE::HOLD)
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

	Safe_Release(mDashCheckCollider);
	Safe_Release(mWallCheckCollider);
	Safe_Release(mWeaponCollider);
	Safe_Release(mEnemyCheckCollider);
	Safe_Release(mSkillBCollider);
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

	if (src->Compare(mDashCheckCollider))
	{
		//초산공간 오픈
		CApplicationManager* pAppManager = CApplicationManager::GetInstance();
		CEnemy* pEnemy = dynamic_cast<CEnemy*>(dest->GetOwner());
		if (pEnemy)
		{
			pAppManager->SetFreeze(true);
		}
	}
}

void CCharacter::OnCollisionStay(CCollider * src, CCollider * dest)
{
	//
	CGameInstance* pInstance = CGameInstance::GetInstance();
	_float DeltaTime = pInstance->GetTimer(TEXT("144FPS"));

	if (src->Compare(mWallCheckCollider))
	{
		CWall* pWall = dynamic_cast<CWall*>(dest->GetOwner());
		if (pWall)
		{
			m_WallHit = true;

			_vector vPos = mTransform->Get_State(CTransform::STATE_POSITION);
			_vector vLook = mTransform->Get_State(CTransform::STATE_LOOK);

			_vector vNormal;
			if (m_pNearWall == nullptr)
			{
				m_pNearWall = pWall;
				vNormal = pWall->GetNormal(vPos, &m_fNearPlaneLength);
			}
			else
			{
				_float fLength = 0.f;
				vNormal = pWall->GetNormal(vPos, &fLength);
				if (fLength < m_fNearPlaneLength)
				{
					m_pNearWall = pWall;
					m_fNearPlaneLength = fLength;
				}
				else
				{
					vNormal = m_pNearWall->GetNormal(vPos, &m_fNearPlaneLength);
				}
			}

			_vector vPrevPos = XMLoadFloat3(&vPrevPosition);
			_float fDepth = XMVectorGetX(XMVector3Length(vPos - vPrevPos)) * 0.005f;

			_float vExtents = m_pNearWall->GetDepth();
						
			_vector vInput = vLook * fDepth;
			_vector vSlide = XMVector3Normalize(vInput - vNormal * XMVectorGetX(XMVector3Dot(vNormal, vInput)));

			//_vector vCurrentPos = vPrevPos + vSlide * fDepth;

			_vector vCurrentPos = vPrevPos + vNormal * fDepth;
			mTransform->Set_State(CTransform::STATE_POSITION, vCurrentPos);
			XMStoreFloat3(&vPrevPosition, vCurrentPos);
		}
	}

	if (src->Compare(mCollider))
	{
		CEnemy* pEnemy = dynamic_cast<CEnemy*>(dest->GetOwner());
		if (pEnemy && dest->Compare(pEnemy->GetBodyCollider()))
		{
			//if (mModel->AnimationCompare(CLIP::STAND2) ||
			//	mModel->AnimationCompare(CLIP::ATTACK51)||
			//	mModel->AnimationCompare(CLIP::HIT1) ||
			//	mModel->AnimationCompare(CLIP::HIT2) || 
			//	mModel->AnimationCompare(CLIP::HIT3) ||
			//	mModel->AnimationCompare(CLIP::HIT4))
			//	return;
			//
			////수정
			//_vector vLook = XMVector3Normalize(mTransform->Get_State(CTransform::STATE_LOOK));

			//_vector vPos = mTransform->Get_State(CTransform::STATE_POSITION);
			//_vector vEnemyPos = XMLoadFloat4(&pEnemy->GetPosition());
			//vEnemyPos = XMVectorSetY(vEnemyPos, 0.0f);

			//_vector vDistance = vPos - vEnemyPos;
			//_vector vDir = XMVector3Normalize(vDistance);

			////구의 반지름의 합
			//_float fTotalRadius = src->GetExtents().x + dest->GetExtents().x;
			//_float fLength = XMVectorGetX(XMVector3Length(vPos - vEnemyPos));

			////겹쳐진 부분의 양
			//_float fDepth = (fTotalRadius - fLength) - src->GetExtents().x;
			//_vector vCurrentPos = vPos + vDir * (fDepth * 1.1f);
			//mTransform->Set_State(CTransform::STATE_POSITION, vCurrentPos);

			if (m_pNearEnemy == pEnemy)
				m_bRootMotion = false;
			else
				m_bRootMotion = true;
		}
	}
}

void CCharacter::OnCollisionExit(CCollider * src, CCollider * dest)
{
	if (src->Compare(mCollider))
	{
		CEnemy* pEnemy = dynamic_cast<CEnemy*>(dest->GetOwner());
		if (pEnemy && dest->Compare(pEnemy->GetBodyCollider()))
		{
			if (m_pNearEnemy == pEnemy)
				m_bRootMotion = true;
		}
	}

	if (src->Compare(mWallCheckCollider))
	{
		CWall* pWall = dynamic_cast<CWall*>(dest->GetOwner());
		if (pWall)
		{
			m_WallHit = true;
		}
	}

	if (src->Compare(mEnemyCheckCollider))
	{
		CEnemy* pEnemy = dynamic_cast<CEnemy*>(dest->GetOwner());
		if (pEnemy)
			DeleteTargetFromList(dest->GetOwner());
	}

}
