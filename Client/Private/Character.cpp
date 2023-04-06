#include "pch.h"
#include "..\Public\Character.h"

#include "GameInstance.h"
#include "CollisionManager.h"
#include "ApplicationManager.h"
#include "Weapon.h"
#include "Bone.h"
#include "Enemy.h"
#include "Boss.h"
#include "Wall.h"

#include "SkillBallSystem.h"

#include "PlayerCamera.h"
#include "PlayerIcon.h"
#include "PlayerHealthBar.h"
#include "EnemyHealthBar.h"
#include "Layer.h"
#include "Thorn.h"

#include "SwordTrail.h"
#include "FloorCircle.h"

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

	CSkillBase::SKILL_INFO SkillInfo;
	ZeroMemory(&SkillInfo, sizeof CSkillBase::SKILL_INFO);
	SkillInfo.eType = CSkillBase::TYPE::INVALID;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (!CApplicationManager::GetInstance()->IsLevelFinish(CApplicationManager::LEVEL::GAMEPLAY))
	{
		SetPosition(_float3(31.f, 0.f, 20.f));
		if (nullptr == (m_pHealthBar = (CPlayerHealthBar*)pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, L"proto_obj_playerhp", L"layer_ui", L"playerhp")))
			return E_FAIL;
		if (nullptr == (m_pPlayerIcon = (CPlayerIcon*)pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, L"proto_obj_playericon", L"layer_ui", L"playericon")))
			return E_FAIL;
	}
	else
	{
		SetPosition(_float3(0.f, 0.f, -6.f));
		if (nullptr == (m_pHealthBar = (CPlayerHealthBar*)pGameInstance->Add_GameObject(LEVEL_BOSS, L"proto_obj_playerhp", L"layer_ui", L"playerhp1")))
			return E_FAIL;
		if (nullptr == (m_pPlayerIcon = (CPlayerIcon*)pGameInstance->Add_GameObject(LEVEL_BOSS, L"proto_obj_playericon", L"layer_ui", L"playericon1")))
			return E_FAIL;
	}

	m_pPlayerIcon->SetupPlayer(this);

	if (!CApplicationManager::GetInstance()->IsLevelFinish(CApplicationManager::LEVEL::GAMEPLAY))
	{
		for (int i = 0; i < SWORD_EFFECT_COUNT; ++i)
		{
			_tchar szTag[MAX_PATH] = L"";
			wsprintfW(szTag, L"sword_trail%d", i);
			CGameObject* pGameObject = nullptr;
			if (nullptr == (pGameObject = pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("proto_obj_sword_trail"), L"layer_swordeffect", szTag, this)))
				return E_FAIL;

			m_pSwordTrail[i] = static_cast<CSwordTrail*>(pGameObject);
		}
	}
	else
	{
		for (int i = 0; i < SWORD_EFFECT_COUNT; ++i)
		{
			_tchar szTag[MAX_PATH] = L"";
			wsprintfW(szTag, L"sword_trail%d", i);
			CGameObject* pGameObject = nullptr;
			if (nullptr == (pGameObject = pGameInstance->Add_GameObject(LEVEL_BOSS, TEXT("proto_obj_sword_trail"), L"layer_swordeffect", szTag, this)))
				return E_FAIL;

			m_pSwordTrail[i] = static_cast<CSwordTrail*>(pGameObject);
		}
	}

	return S_OK;
}

void CCharacter::Tick(_double TimeDelta)
{
	if (m_EvolutionCount >= 30)
	{
		m_bEvolutionReady = true;
		m_fCurEvolutionAcc = 0.f;
	}

	if (mModel->AnimationCompare(CLIP::ATTACK51))
	{
		if (mModel->AnimationIsFinishEx())
			m_bGageDownStart = true;
	}

	if (m_bGageDownStart)
	{
		m_fCurEvolutionAcc += TimeDelta;
		if (m_fCurEvolutionAcc >= m_fEvolutionTimeOut)
		{
			m_bGageDownStart = false;
			m_bEvolution = false;
			m_fCurEvolutionAcc = 0.f;
		}
	}


	if (m_iComboCount > 0)
	{
		m_fCurComboTimer += TimeDelta;
		if (m_fCurComboTimer >= m_fComboTimeOut)
		{
			m_iComboCount = 0;
			m_fCurComboTimer = 0.f;
		}
	}

	m_fCurDash += 5.f * TimeDelta;
	if (m_fCurDash >= 100.f)
		m_fCurDash = 100.f;

	if (m_bFreezeReadyWait)
	{
		m_fFreezeReadyTimer += TimeDelta;
		if (m_fFreezeReadyTimer >= m_fFreezeReadyTimeOut)
		{
			m_bFreezeReadyWait = false;
			m_pAppManager->SetFreezeReady(true);

			m_fFreezeReadyTimer = 0.f;
		}
	}

	TimeDelta = Freeze(TimeDelta);

	__super::Tick(TimeDelta);

	CBoss* pBoss = dynamic_cast<CBoss*>(m_pNearEnemy);
	if (pBoss)
	{
		if (pBoss->IsDie())
			m_pEnemyHealthBar->SetRender(false);
	}

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	pGameInstance->AddCollider(mCollider);
	pGameInstance->AddCollider(mWallCheckCollider, 1);
	pGameInstance->AddCollider(mEnemyCheckCollider);
	pGameInstance->AddCollider(mDashCheckCollider);
	
	pGameInstance->AddCollider(mWeaponCollider);
	pGameInstance->AddCollider(mSkillCollider);


	//초산진입시 RGB 분리
	if (CApplicationManager::GetInstance()->IsFreeze() && !m_bTimeStop)
	{
		m_fCurBlurAmount = 20.f;
		mRenderer->SetBlurAmount(m_fCurBlurAmount);
	}

	if(m_bTimeStop && m_fCurBlurAmount >= 0.f)
	{
		m_fCurBlurAmount -= TimeDelta * 40.f;
		mRenderer->SetBlurAmount(m_fCurBlurAmount);
		if (m_fCurBlurAmount <= 0.f)
		{
			m_fCurBlurAmount = 0.f;
			mRenderer->SetBlurAmount(0.f);
		}
	}

	if (m_bWin)
	{
		CApplicationManager::GetInstance()->SetWinMotion(true);
		m_pPlayerIcon->SetRender(false);
		m_pHealthBar->SetRender(false);

		if (!CApplicationManager::GetInstance()->IsLevelFinish(CApplicationManager::LEVEL::GAMEPLAY))
		{
			//첫번쨰 맵 엔딩위치 및 각도
			_vector vEndPosition = XMVectorSet(41.f, 0.f, 78.f, 1.f);
			mTransform->Set_State(CTransform::STATE_POSITION, vEndPosition);
			//mTransform->SetRotation(VECTOR_UP, XMConvertToRadians(255.f));
			mTransform->SetRotation(VECTOR_UP, XMConvertToRadians(225.f)); // 엔딩 위치
			SetAnimation(CLIP::WIN, CAnimation::TYPE::ONE);
		}
		else
		{
			//두번쨰 맵 엔딩위치 및 각도
			_vector vEndPosition = XMVectorSet(0.f, 0.f, 40.f, 1.f);
			mTransform->Set_State(CTransform::STATE_POSITION, vEndPosition);
			mTransform->SetRotation(VECTOR_UP, XMConvertToRadians(180.f));
			//mTransform->SetRotation(VECTOR_UP, XMConvertToRadians(225.f)); // 엔딩 위치
			SetAnimation(CLIP::WIN, CAnimation::TYPE::ONE);
		}

	}
	else
	{
		if (m_bHit)
		{
			if (mModel->AnimationIsPreFinishCustom(0.5))
			{
				//m_bHit = false;
				//m_bMoveable = true;
				m_bDashable = true;
				//m_bAttackable = true;
			}

			if (mModel->AnimationIsFinishEx())
			{
				m_bHit = false;
				m_bMoveable = true;
				m_bDashable = true;
				m_bAttackable = true;
			}

		}
		/*if(m_bAirbone)
			AirboneProcess(TimeDelta);*/

		KeyInput(TimeDelta);
		Dash(TimeDelta);
		Attack(TimeDelta);

		if (!m_WallHit)
			XMStoreFloat3(&vPrevPosition, mTransform->Get_State(CTransform::STATE_POSITION));

		if (m_bEnemyHolding)
			HoldEnemy();
	}

	AnimationControl(TimeDelta);
	SkillColliderControl(TimeDelta);
	//PositionHold(TimeDelta);
	AttackEffectControl(TimeDelta);

	CameraSocketUpdate();
	TargetListDeastroyCehck();
	m_pHealthBar->SetHealth(m_fCurHp, m_fMaxHp);
	m_pHealthBar->SetDash(m_fCurDash, m_fMaxDash);
}

void CCharacter::LateTick(_double TimeDelta)
{
	m_fSmokeTimeAcc += TimeDelta * 0.25;
	m_fSmokeTimeAcc2 += TimeDelta * 0.3f;
	m_fSmokeTimeAcc3 += TimeDelta * 0.2f;

	m_pPlayerIcon->SetCombo(m_iComboCount);
	m_pPlayerIcon->SetComboPerTime(m_fCurComboTimer);

	TimeDelta = Freeze(TimeDelta);
	__super::LateTick(TimeDelta);

	FindNearTarget();
	RenderEnemyHealth(TimeDelta);

	//콜리전 세팅
	_matrix transMatrix = XMLoadFloat4x4(&bone->GetCombinedMatrix()) * XMLoadFloat4x4(&mTransform->Get_WorldMatrix());
	mCollider->Update(transMatrix);
	mWallCheckCollider->Update(transMatrix);
	mEnemyCheckCollider->Update(XMLoadFloat4x4(&mTransform->Get_WorldMatrix()));
	//mWeaponCollider->Update(XMLoadFloat4x4(&mTransform->Get_WorldMatrix()));
	mWeaponCollider->Update(XMLoadFloat4x4(&m_pWeaponBone->GetOffSetMatrix()) * XMLoadFloat4x4(&m_pWeaponBone->GetCombinedMatrix()) * XMLoadFloat4x4(&mModel->GetLocalMatrix()) *  XMLoadFloat4x4(&mTransform->Get_WorldMatrix()));
	mDashCheckCollider->Update(XMLoadFloat4x4(&m_matrixPrevPos));
	mSkillCollider->Update(XMLoadFloat4x4(&mTransform->Get_WorldMatrix()));

	if (nullptr != mRenderer)
		mRenderer->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
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

		if(i == 3 || i == 4)
			mShader->Begin(0);
		else
			mShader->Begin(6);

		mModel->Render(i);

		mShader->Begin(2);
		mModel->Render(i);
	}
	
	CGameInstance* pInstance = CGameInstance::GetInstance();
	_float4 vCamPos = CPipeLine::GetInstance()->Get_CamPosition();

	_vector vRight = XMVector3Normalize(mTransform->Get_State(CTransform::STATE_RIGHT));
	_vector vSmokePos;

	vSmokePos = mTransform->Get_State(CTransform::STATE_POSITION);
	vSmokePos = XMVectorSetY(vSmokePos, XMVectorGetY(vSmokePos) + 0.3f);
	m_pSmokeTransform1->Set_State(CTransform::STATE_POSITION, vSmokePos);
	m_pSmokeTransform1->LookAt(XMLoadFloat4(&vCamPos));
	m_pSmokeTransform1->Set_Scale(_float3(0.8f, 0.8f, 0.8f));

	vSmokePos = mTransform->Get_State(CTransform::STATE_POSITION);
	vSmokePos = XMVectorSetY(vSmokePos, XMVectorGetY(vSmokePos) + 1.f);
	m_pSmokeTransform3->Set_State(CTransform::STATE_POSITION, vSmokePos);
	m_pSmokeTransform3->LookAt(XMLoadFloat4(&vCamPos));
	m_pSmokeTransform3->Set_Scale(_float3(0.8f, 0.8f, 0.8f));

	if (FAILED(m_pSmokeTransform1->Setup_ShaderResource(m_pRectShader, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pRectShader->SetMatrix("g_ViewMatrix", &pInstance->Get_Transform_float4x4(CPipeLine::TS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pRectShader->SetMatrix("g_ProjMatrix", &pInstance->Get_Transform_float4x4(CPipeLine::TS_PROJ))))
		return E_FAIL;
	
	m_pRectShader->SetRawValue("g_TimeAcc", &m_fSmokeTimeAcc, sizeof(_float));
	m_pSmokeDiffuse->Setup_ShaderResource(m_pRectShader, "g_Texture");
	m_pSmokeMask->Setup_ShaderResource(m_pRectShader, "g_MaskTexture");

	m_pRectShader->Begin(4);
	m_pEvolutionSmoke->Render();

	if (FAILED(m_pSmokeTransform3->Setup_ShaderResource(m_pRectShader, "g_WorldMatrix")))
		return E_FAIL;
	m_pRectShader->SetRawValue("g_TimeAcc", &m_fSmokeTimeAcc3, sizeof(_float));
	m_pRectShader->Begin(3);
	m_pEvolutionSmoke->Render();

	return S_OK;
}

void CCharacter::RenderGUI()
{
	ImGui::Begin("Player Position");

	_float3 vPos;
	XMStoreFloat3(&vPos, mTransform->Get_State(CTransform::STATE_POSITION));
	ImGui::InputFloat3("World Pos ", (_float*)&vPos);

	XMStoreFloat3(&m_vPrevLook, mTransform->Get_State(CTransform::STATE_LOOK));
	ImGui::InputFloat3("LOOK  ", (_float*)&m_vPrevLook);

	ImGui::End();
}

_float CCharacter::GetLengthFromCamera()
{
	CPipeLine* pPipeLine = CPipeLine::GetInstance();

	_vector vPos = mTransform->Get_State(CTransform::STATE_POSITION);
	_vector vCamPos = XMLoadFloat4(&pPipeLine->Get_CamPosition());

	_float fLength = XMVectorGetX(XMVector3Length(vPos - vCamPos));

	return fLength;
}

void CCharacter::SetPosition(_float3 vPos)
{
	mTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(vPos.x, 0.f, vPos.z, 1.f));
}

_fmatrix CCharacter::GetTargetMatrix()
{
	if(m_pNearEnemy != nullptr)
		return m_pNearEnemy->GetWorldMatrix();
	else
		return _fmatrix();
}

_float2 CCharacter::GetTargetWindowPos()
{
	if (m_pNearEnemy == nullptr)
		return _float2(FLT_MAX, FLT_MAX);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	_vector vPos = XMLoadFloat4(&m_pNearEnemy->GetPosition());
	_vector vFixPos;

	//일반 몹일경우 보스몹일경우 위치 구분
	if (!CApplicationManager::GetInstance()->IsLevelFinish(CApplicationManager::LEVEL::GAMEPLAY))
		vFixPos = XMVector3TransformCoord(XMVectorSetY(vPos, XMVectorGetY(vPos) + 1.f), pGameInstance->Get_Transform_Matrix(CPipeLine::TS_VIEW));
	else
		vFixPos = XMVector3TransformCoord(XMVectorSetY(vPos, XMVectorGetY(vPos) + 2.3f), pGameInstance->Get_Transform_Matrix(CPipeLine::TS_VIEW));

	vFixPos = XMVector3TransformCoord(vFixPos, pGameInstance->Get_Transform_Matrix(CPipeLine::TS_PROJ));

	return _float2(XMVectorGetX(vFixPos), XMVectorGetY(vFixPos) + 1.f);
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

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("proto_com_vibuffer_rect"), TEXT("com_vibuffer_rect"), (CComponent**)&m_pEvolutionSmoke)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("proto_com_shader_vtxtex"), TEXT("com_shader_rect"), (CComponent**)&m_pRectShader)))
		return E_FAIL;
	
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("proto_com_texture_smokediffuse"), TEXT("com_texutre_rect"), (CComponent**)&m_pSmokeDiffuse)))
		return E_FAIL;
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("proto_com_texture_smokemask"), TEXT("com_texutre_rect2"), (CComponent**)&m_pSmokeMask)))
		return E_FAIL;

	CTransform::TRANSFORM_DESC TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORM_DESC));

	TransformDesc.fMoveSpeed = m_fMoveSpeed;
	TransformDesc.fRotationSpeed = XMConvertToRadians(m_fRotationSpeed);

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("proto_com_transform"), TEXT("com_transform"), (CComponent**)&mTransform, &TransformDesc)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("proto_com_transform"), TEXT("com_transform2"), (CComponent**)&m_pSmokeTransform1, &TransformDesc)))
		return E_FAIL;
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("proto_com_transform"), TEXT("com_transform3"), (CComponent**)&m_pSmokeTransform2, &TransformDesc)))
		return E_FAIL;
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("proto_com_transform"), TEXT("com_transform4"), (CComponent**)&m_pSmokeTransform3, &TransformDesc)))
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
	collDesc.vExtents = _float3(1.9f, 1.9f, 1.9f);
	collDesc.vRotation = _float3(0.f, 0.f, 0.f);

	if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("proto_com_sphere_collider"), TEXT("com_collider"), (CComponent**)&mCollider, &collDesc)))
		return E_FAIL;
	
	ZeroMemory(&collDesc, sizeof(collDesc));
	collDesc.owner = this;
	collDesc.vCenter = _float3(0.f, 1.f, 1.f);
	collDesc.vExtents = _float3(2.f, 1.f, 3.5f);
	collDesc.vRotation = _float3(0.f, 0.f, 0.f);

	if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("proto_com_obb_collider"), TEXT("com_collider_skill"), (CComponent**)&mSkillCollider, &collDesc)))
		return E_FAIL;

	mSkillCollider->SetActive(false);

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
	collDesc.vExtents = _float3(70.f, 70.f, 70.f);
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
	collDesc.vExtents = _float3(1.7f, 0.2f, 0.3f);
	collDesc.vRotation = _float3(0.f, 0.f, 0.f);

	if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("proto_com_obb_collider"), TEXT("com_collider_weapon"), (CComponent**)&mWeaponCollider, &collDesc)))
		return E_FAIL;

	mWeaponCollider->SetActive(false);
	mWeaponCollider->SetColor(_float4(0.f, 1.f, 0.f, 1.f));

	if (!m_pAppManager->IsLevelFinish(CApplicationManager::LEVEL::GAMEPLAY))
	{
		CGameObject* pCircle = nullptr;
		if (nullptr == (pCircle = pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, L"proto_obj_circle", L"layer_ui", L"circle", this)))
			return E_FAIL;
		static_cast<CFloorCircle*>(pCircle)->SetType(CFloorCircle::CIRCLE_PLAYER);
	}
	else
	{
		CGameObject* pCircle = nullptr;
		if (nullptr == (pCircle = pGameInstance->Add_GameObject(LEVEL_BOSS, L"proto_obj_circle", L"layer_ui", L"circle", this)))
			return E_FAIL;
		static_cast<CFloorCircle*>(pCircle)->SetType(CFloorCircle::CIRCLE_PLAYER);
	}


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

	if (CApplicationManager::GetInstance()->IsLevelFinish(CApplicationManager::LEVEL::BOSS))
	{
		_float4 vDir = { -1.f, -1.f, 0.f, 0.f };
		if (FAILED(mShader->SetRawValue("g_vLightDir", &vDir, sizeof(_float4))))
			return E_FAIL;
	}
	else
	{
		if (FAILED(mShader->SetRawValue("g_vLightDir", &LightDesc->vDirection, sizeof(_float4))))
			return E_FAIL;
	}

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
	CSkillBallSystem* pSkillSystem = CSkillBallSystem::GetInstance();

	InputMove(TimeDelta);

	if (pGameInstance->Input_KeyState_Custom(DIK_TAB) == KEY_STATE::TAP)
	{
		NearTargetChange();
	}

	//테스트 코드
	if (pGameInstance->Input_KeyState_Custom(DIK_P) == KEY_STATE::TAP)
	{
		if (!m_bUseSkill && m_bEvolutionReady)
		{
			m_EvolutionCount = 0;

			m_bEvolution = true;
			SetAnimation(CLIP::ATTACK51, CAnimation::TYPE::ONE);
			m_bAttackable = false;
			m_bMoveable = false;
			m_bDashable = false;
			m_bEvolutionAttack = false;
			m_bEvolutionReady = false;

			m_pCamera->EvolutionStart();
			m_bRedAddGageReady = true;
			m_bYellowAddGageReady = true;
			m_bBlueAddGageReady = true;
		}
	}

	//넉백
	if (mModel->AnimationCompare(CLIP::ATTACK51))
	{
		if (mModel->AnimationIsPreFinishCustom(0.55) && !m_bEvolutionAttack)
		{
			m_bEvolutionAttack = true;
			for (auto& pEnemy : m_Enemys)
			{
				_vector vEnemyPos = XMLoadFloat4(&pEnemy->GetPosition());
				_vector vPos = mTransform->Get_State(CTransform::STATE_POSITION);

				_float fLength = XMVectorGetX(XMVector3Length(vEnemyPos - vPos));
				if (fLength < 5.f)
				{
					if (!m_pAppManager->IsFreeze())
						pEnemy->SetNuckback(40.f);

					pEnemy->SetAirborne(GetDamage());
				}

				m_pCamera->StartShake(5.f, 50.f);
			}
		}

	}

	if (pGameInstance->Input_KeyState_Custom(DIK_A) == KEY_STATE::TAP)
	{
		if (!m_bUseSkill && m_bSkillReady)
		{
			m_SkillInfo = pSkillSystem->UseSkill(0);
			AddEvolutionGage(m_SkillInfo);
			switch (m_SkillInfo.eType)
			{
			case Client::CSkillBase::TYPE::RED:
				SkillA(TimeDelta);
				break;
			case Client::CSkillBase::TYPE::BLUE:
				SkillB(TimeDelta);
				break;
			case Client::CSkillBase::TYPE::YELLOW:
				SkillC(TimeDelta);
				break;
			case Client::CSkillBase::TYPE::INVALID:
				break;
			default:
				break;
			}
		}
	}

	if (pGameInstance->Input_KeyState_Custom(DIK_S) == KEY_STATE::TAP)
	{
		if (!m_bUseSkill && m_bSkillReady)
		{
			m_SkillInfo = pSkillSystem->UseSkill(1);
			AddEvolutionGage(m_SkillInfo);
			switch (m_SkillInfo.eType)
			{
			case Client::CSkillBase::TYPE::RED:
				SkillA(TimeDelta);
				break;
			case Client::CSkillBase::TYPE::BLUE:
				SkillB(TimeDelta);
				break;
			case Client::CSkillBase::TYPE::YELLOW:
				SkillC(TimeDelta);
				break;
			case Client::CSkillBase::TYPE::INVALID:
				break;
			default:
				break;
			}
		}
	}

	if (pGameInstance->Input_KeyState_Custom(DIK_D) == KEY_STATE::TAP)
	{
		if (!m_bUseSkill && m_bSkillReady)
		{
			m_SkillInfo = pSkillSystem->UseSkill(2);
			AddEvolutionGage(m_SkillInfo);
			switch (m_SkillInfo.eType)
			{
			case Client::CSkillBase::TYPE::RED:
				SkillA(TimeDelta);
				break;
			case Client::CSkillBase::TYPE::BLUE:
				SkillB(TimeDelta);
				break;
			case Client::CSkillBase::TYPE::YELLOW:
				SkillC(TimeDelta);
				break;
			case Client::CSkillBase::TYPE::INVALID:
				break;
			default:
				break;
			}
		}
	}

	if (pGameInstance->Input_KeyState_Custom(DIK_F) == KEY_STATE::TAP)
	{
		if (!m_bUseSkill && m_bSkillReady)
		{
			m_SkillInfo = pSkillSystem->UseSkill(3);
			AddEvolutionGage(m_SkillInfo);
			switch (m_SkillInfo.eType)
			{
			case Client::CSkillBase::TYPE::RED:
				SkillA(TimeDelta);
				break;
			case Client::CSkillBase::TYPE::BLUE:
				SkillB(TimeDelta);
				break;
			case Client::CSkillBase::TYPE::YELLOW:
				SkillC(TimeDelta);
				break;
			case Client::CSkillBase::TYPE::INVALID:
				break;
			default:
				break;
			}
		}
	}

	if (pGameInstance->Input_KeyState_Custom(DIK_Q) == KEY_STATE::TAP)
	{
		if (!m_bUseSkill && m_bSkillReady)
		{
			m_SkillInfo = pSkillSystem->UseSkill(4);
			AddEvolutionGage(m_SkillInfo);
			switch (m_SkillInfo.eType)
			{
			case Client::CSkillBase::TYPE::RED:
				SkillA(TimeDelta);
				break;
			case Client::CSkillBase::TYPE::BLUE:
				SkillB(TimeDelta);
				break;
			case Client::CSkillBase::TYPE::YELLOW:
				SkillC(TimeDelta);
				break;
			case Client::CSkillBase::TYPE::INVALID:
				break;
			default:
				break;
			}
		}
	}

	if (pGameInstance->Input_KeyState_Custom(DIK_W) == KEY_STATE::TAP)
	{
		if (!m_bUseSkill && m_bSkillReady)
		{
			m_SkillInfo = pSkillSystem->UseSkill(5);
			AddEvolutionGage(m_SkillInfo);
			switch (m_SkillInfo.eType)
			{
			case Client::CSkillBase::TYPE::RED:
				SkillA(TimeDelta);
				break;
			case Client::CSkillBase::TYPE::BLUE:
				SkillB(TimeDelta);
				break;
			case Client::CSkillBase::TYPE::YELLOW:
				SkillC(TimeDelta);
				break;
			case Client::CSkillBase::TYPE::INVALID:
				break;
			default:
				break;
			}
		} 
	}

	if (pGameInstance->Input_KeyState_Custom(DIK_E) == KEY_STATE::TAP)
	{
		if (!m_bUseSkill && m_bSkillReady)
		{
			m_SkillInfo = pSkillSystem->UseSkill(6);
			AddEvolutionGage(m_SkillInfo);
			switch (m_SkillInfo.eType)
			{
			case Client::CSkillBase::TYPE::RED:
				SkillA(TimeDelta);
				break;
			case Client::CSkillBase::TYPE::BLUE:
				SkillB(TimeDelta);
				break;
			case Client::CSkillBase::TYPE::YELLOW:
				SkillC(TimeDelta);
				break;
			case Client::CSkillBase::TYPE::INVALID:
				break;
			default:
				break;
			}
		}
	}

	if (pGameInstance->Input_KeyState_Custom(DIK_R) == KEY_STATE::TAP)
	{
		if (!m_bUseSkill && m_bSkillReady)
		{
			m_SkillInfo = pSkillSystem->UseSkill(7);
			AddEvolutionGage(m_SkillInfo);
			switch (m_SkillInfo.eType)
			{
			case Client::CSkillBase::TYPE::RED:
				SkillA(TimeDelta);
				break;
			case Client::CSkillBase::TYPE::BLUE:
				SkillB(TimeDelta);
				break;
			case Client::CSkillBase::TYPE::YELLOW:
				SkillC(TimeDelta);
				break;
			case Client::CSkillBase::TYPE::INVALID:
				break;
			default:
				break;
			}
		}
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

	if (m_bStart)
	{
		SetAnimation(CLIP::BORN, CAnimation::TYPE::ONE);
		if (mModel->AnimationCompare(CLIP::BORN))
		{
			if (!m_bStartAction)
			{
				if (mModel->GetCurrentTimeAcc() >= 0.2f)
				{
					m_bStartAction = true;
					m_pCamera->AttackShake();
				}
			}

			if (mModel->AnimationIsFinishEx())
			{
				m_bStart = false;
				m_bStartAction = false;
			}
		}
	}
	else if (mModel->AnimationIsFinish())
		Idle();
}

void CCharacter::Dash(_double TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (AnimationCompare(CLIP::ATTACK51))
		return;

	if (m_bAttacking)
	{
		//m_bDashable = false;
		if (mModel->AnimationIsPreFinish())
			m_bDashable = true;
	}
	else
		m_bDashable = true;

	if (m_bDashPrevPosSet)
	{
		m_fActiveDuration += (_float)TimeDelta;
		if (m_fActiveDuration >= m_fActiveTimeOut)
		{
			m_fActiveDuration = 0.0f;
			m_bDashPrevPosSet = false;
			mDashCheckCollider->SetActive(false);
		}
	}

	if (AnimationCompare(CLIP::MOVE1) || AnimationCompare(CLIP::MOVE2))
	{
		//mCollider->SetActive(false);
		mWeaponCollider->SetActive(false);
		mSkillCollider->SetActive(false);

		m_bUseSkill = false;
		m_bSkillReady = false;

		if (mModel->AnimationIsPreFinish())
		{
			//mCollider->SetActive(true);
			m_bDashable = true;
			
			m_bSkillReady = true;
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
	
	if(!AnimationCompare(CLIP::MOVE1) && !AnimationCompare(CLIP::MOVE2))
		mCollider->SetActive(true);

	//이동공격 스킬 사용중에 대쉬금지
	if (AnimationCompare(CLIP::ATTACK31) || AnimationCompare(CLIP::ATTACK32))
		return;

	//정면 대쉬
	if (m_bDashable)
	{
		if (!m_bFrontDashReady)
		{
			if (pGameInstance->Input_KeyState_Custom(DIK_UPARROW) == KEY_STATE::AWAY)
			{
				if (m_fCurDash >= 20.f)
				{
					m_bFrontDashReady = true;
					m_bLeftDashReady = false;
					m_bRightDashReady = false;
				}
			}
		}
		else
		{
			if (pGameInstance->Input_KeyState_Custom(DIK_UPARROW) == KEY_STATE::TAP)
			{
				if (m_fCurDash >= 20.f && !mModel->AnimationCompare(CLIP::MOVE1))
				{
					m_fCurDash -= 20.f;
					SetAnimation(CLIP::MOVE1, CAnimation::ONE);
					m_bAttackable = false;
					m_bMoveable = false;
					m_bDashable = false;
					m_bSkillReady = false;

					SavePrevPos();
				}
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
				if (m_fCurDash >= 20.f)
				{
					m_bFrontDashReady = false;
					m_bLeftDashReady = true;
					m_bRightDashReady = false;
				}
			}
		}
		else
		{
			if (pGameInstance->Input_KeyState_Custom(DIK_LEFTARROW) == KEY_STATE::TAP)
			{
				if (m_fCurDash >= 20.f && !mModel->AnimationCompare(CLIP::MOVE1))
				{
					if (m_bAttacking)
						mTransform->LookAt(mTransform->Get_State(CTransform::STATE_POSITION) + -XMVector3Normalize(mTransform->Get_State(CTransform::STATE_RIGHT)));

					m_fCurDash -= 20.f;
					SetAnimation(CLIP::MOVE1, CAnimation::ONE);
					m_bAttackable = false;
					m_bMoveable = false;
					m_bDashable = false;
					m_bSkillReady = false;

					SavePrevPos();
				}
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
				if (m_fCurDash >= 20.f)
				{
					m_bFrontDashReady = false;
					m_bLeftDashReady = false;
					m_bRightDashReady = true;
				}
			}
		}
		else
		{
			if (pGameInstance->Input_KeyState_Custom(DIK_RIGHTARROW) == KEY_STATE::TAP)
			{
				if (m_fCurDash >= 20.f && !mModel->AnimationCompare(CLIP::MOVE1))
				{
					if (m_bAttacking)
						mTransform->LookAt(mTransform->Get_State(CTransform::STATE_POSITION) + XMVector3Normalize(mTransform->Get_State(CTransform::STATE_RIGHT)));

					m_fCurDash -= 20.f;
					SetAnimation(CLIP::MOVE1, CAnimation::ONE);
					m_bAttackable = false;
					m_bMoveable = false;
					m_bDashable = false;
					m_bSkillReady = false;

					SavePrevPos();
				}
			}
		}
	}

	//백 대쉬
	if (m_bDashable)
	{
		if (pGameInstance->Input_KeyState_Custom(DIK_Z) == KEY_STATE::TAP)
		{
			if (m_fCurDash >= 20.f && !mModel->AnimationCompare(CLIP::MOVE2))
			{
				m_fCurDash -= 20.f;
				SavePrevPos();
				SetAnimation(CLIP::MOVE2, CAnimation::ONE);
				m_bAttackable = false;
				m_bMoveable = false;
				m_bDashable = false;
				m_bSkillReady = false;
			}
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
	m_bMoveable = true;
	m_bDashable = true;
	m_bAttackable = true;
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

	//변신중 공격X
	if (mModel->AnimationCompare(CLIP::ATTACK51))
		return;

	if (!m_bAttackable)
		return;

	if (m_bAttacking)
	{
		mWeaponCollider->SetActive(true);
		if (mModel->AnimationIsPreFinishCustom(0.4))
		{
			m_bHitColliderCheck = true;
			mWeaponCollider->SetActive(false);
		}
	}

	if (pGameInstance->Input_KeyState_Custom(DIK_LCONTROL) == KEY_STATE::TAP)
	{
		m_bHitColliderCheck = false;
		
		if (m_pNearEnemy)
		{
			_float4 vPos = m_pNearEnemy->GetPosition();
			vPos.y = 0.f;
			mTransform->LookAt(XMLoadFloat4(&vPos));
		}

		if (!m_bEvolution)
		{

			if (m_bAttacking)
			{
				m_bMoveable = false;
				//m_bDashable = false;
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
				//m_bDashable = false;
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
		!AnimationCompare(CLIP::ATTACK21) &&
		!AnimationCompare(CLIP::ATTACK31) &&
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
	if (!m_bSkillReady)
		return;

	mWeaponCollider->SetActive(false);
	m_bUseSkill = true;
	m_bMoveable = false;
	m_bAttackable = false;
	//m_bDashable = false;
	m_bSkillReady = false;

	mWeaponCollider->SetActive(true);

	if(!m_bEvolution)
		SetAnimation(CLIP::ATTACK11, CAnimation::TYPE::ONE);
	else
	{
		if (m_pNearEnemy)
		{
			_float4 vPos = m_pNearEnemy->GetPosition();
			vPos.y = 0.f;
			mTransform->LookAt(XMLoadFloat4(&vPos));
			m_pAttackTargetEnemy = m_pNearEnemy;
		}

		SetAnimation(CLIP::ATTACK12, CAnimation::TYPE::ONE);
		
	}
}

void CCharacter::SkillB(_double TimeDelta)
{
	if (!m_bSkillReady)
		return;

	mWeaponCollider->SetActive(false);
	m_bUseSkill = true;
	m_bMoveable = false;
	m_bAttackable = false;
	//m_bDashable = false;
	m_bSkillReady = false;

	mSkillCollider->HitColliderReset();

	if(!m_bEvolution)
		SetAnimation(CLIP::ATTACK21, CAnimation::TYPE::ONE);
	else
	{
		SetAnimation(CLIP::ATTACK22, CAnimation::TYPE::ONE);
	}

}

void CCharacter::SkillC(_double TimeDelta)
{
	if (!m_bSkillReady)
		return;

	if (m_pNearEnemy != nullptr)
	{
		_float4 vPos = m_pNearEnemy->GetPosition();
		vPos.y = 0.f;
		mTransform->LookAt(XMLoadFloat4(&vPos));
	}

	mWeaponCollider->SetActive(false);
	m_bUseSkill = true;
	m_bMoveable = false;
	m_bAttackable = false;
	m_bDashable = false;
	m_bSkillReady = false;

	if(m_pNearEnemy != nullptr)
		m_pPrevHoldEnemy = m_pNearEnemy;

	if(!m_bEvolution)
		SetAnimation(CLIP::ATTACK31, CAnimation::TYPE::ONE);
	else
	{
		mWeaponCollider->SetActive(true);
		SetAnimation(CLIP::ATTACK32, CAnimation::TYPE::ONE);
	}
	
}

void CCharacter::SkillColliderControl(_double TimeDelta)
{
	if (mModel->AnimationCompare(CLIP::ATTACK11) || mModel->AnimationCompare(CLIP::ATTACK12))
	{
		if (mModel->AnimationCompare(CLIP::ATTACK11))
		{
			if (mModel->AnimationIsPreFinishCustom(0.5))
			{
				mWeaponCollider->SetActive(false);
			}
		}
		else if (mModel->AnimationCompare(CLIP::ATTACK12))
		{
			if (mModel->AnimationIsPreFinishCustom(0.8))
			{
				mWeaponCollider->SetActive(false);
			}

			if (mModel->AnimationIsFinishEx())
			{
				if (m_pAttackTargetEnemy != nullptr)
				{
					m_pAttackTargetEnemy->RecvDamage(100.f);
					m_pCamera->AttackShake();
					m_pAttackTargetEnemy = nullptr;
				}

			}

		}

		if (mModel->AnimationIsFinishEx())
		{
			m_bUseSkill = false;
			m_bSkillReady = true;
			m_bAttackable = true;
			m_bMoveable = true;
			m_bDashable = true;
		}
	}
	else if (mModel->AnimationCompare(CLIP::ATTACK21) || mModel->AnimationCompare(CLIP::ATTACK22))
	{

		if (mModel->AnimationCompare(CLIP::ATTACK21))
		{
			if (mModel->AnimationIsPreFinishEx())
			{
				mSkillCollider->SetActive(true);
			}

			if (mModel->AnimationIsPreFinishCustom(0.5))
			{
				mSkillCollider->SetActive(false);
			}
		}

		if (mModel->AnimationCompare(CLIP::ATTACK22))
		{
			if (mModel->AnimationIsPreFinishCustom(0.2))
			{
				mSkillCollider->SetActive(true);
			}

			if (mModel->AnimationIsPreFinish())
			{
				mSkillCollider->SetActive(false);
			}
		}

		if (mModel->AnimationIsFinishEx())
		{
			m_bUseSkill = false;
			m_bSkillReady = true;
			m_bAttackable = true;
			m_bMoveable = true;
			m_bDashable = true;
		}

	}
	else if (mModel->AnimationCompare(CLIP::ATTACK31) || mModel->AnimationCompare(CLIP::ATTACK32))
	{
		if (!m_bSkillYellowAttack && m_pNearEnemy)
		{
			if (mModel->AnimationIsPreFinishCustom(0.5))
			{
				m_bSkillYellowAttack = true;
				m_pNearEnemy->RecvDamage(GetDamage());
				//m_pNearEnemy->SetHold(false);

				if (!m_bEvolution)
					m_EvolutionCount++;

				m_pPlayerIcon->StartShake();
				m_pEnemyHealthBar->StartShake();

				m_pCamera->AttackShake();
			}
		}

		if (m_pPrevHoldEnemy && m_pPrevHoldEnemy->IsDeadWait())
		{
			m_pPrevHoldEnemy = nullptr;
			m_bEnemyHolding = false;
		}

		if (mModel->AnimationIsFinishEx())
		{
			if (m_pNearEnemy && m_pNearEnemy == m_pPrevHoldEnemy)
				m_pNearEnemy->SetHold(false);

			m_pPrevHoldEnemy = nullptr;
			m_bSkillYellowAttack = false;
			m_bUseSkill = false;
			m_bSkillReady = true;
			m_bAttackable = true;
			m_bMoveable = true;
			m_bDashable = true;
			m_bRootMotion = true;

			//몬스터 잡기 끝
			m_bEnemyHolding = false;
		}
	}

	if (!mModel->AnimationCompare(CLIP::ATTACK11) &&
		!mModel->AnimationCompare(CLIP::ATTACK21) &&
		!mModel->AnimationCompare(CLIP::ATTACK31) &&
		!mModel->AnimationCompare(CLIP::ATTACK12) &&
		!mModel->AnimationCompare(CLIP::ATTACK22) &&
		!mModel->AnimationCompare(CLIP::ATTACK32) &&
		!mModel->AnimationCompare(CLIP::MOVE1) &&
		!mModel->AnimationCompare(CLIP::MOVE2))
	{
		m_bUseSkill = false;
		m_bSkillReady = true;
		//m_bAttackable = true;
		//m_bMoveable = true;
		//m_bDashable = true;
	}
}

_bool CCharacter::IsCameraLockOn()
{
	CBoss* pBoss = dynamic_cast<CBoss*>(m_pNearEnemy);
	if (pBoss)
		return pBoss->IsSpawned();

	return m_pNearEnemy != nullptr;
}

void CCharacter::TargetListDeastroyCehck()
{
	for (auto iter = m_Enemys.begin(); iter != m_Enemys.end(); )
	{
		if ((*iter)->IsDisable())
		{
			if ((*iter) == m_pNearEnemy)
			{
				m_pNearEnemy = nullptr;
			}

			iter = m_Enemys.erase(iter);

			m_bEnemyHealthDraw = false;
			m_pEnemyHealthBar->SetRender(false);
		}
		else
			++iter;
	}

	if (m_Enemys.empty())
	{
		mEnemyCheckCollider->HitColliderReset();
		m_iEnemyIndex = 0;
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
			m_bRootMotion = true;
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

			m_bEnemyHealthDraw = false;
			m_pEnemyHealthBar->SetRender(false);
		}
	}
}

void CCharacter::NearTargetChange()
{
	if (m_bEnemyHolding)
		return;

	if (mModel->AnimationCompare(CLIP::ATTACK31) || mModel->AnimationCompare(CLIP::ATTACK32))
		return;

	if(m_iEnemyIndex < m_Enemys.size())
		m_iEnemyIndex++;

	int i = 0;
	for (auto iter = m_Enemys.begin(); iter != m_Enemys.end(); ++iter)
	{
		if (m_iEnemyIndex >= m_Enemys.size())
		{
			m_iEnemyIndex = 0;
			m_pNearEnemy = *(m_Enemys.begin());
			m_pEnemyHealthBar->SetFillAmount(m_pNearEnemy->GetHpState().fCurHp, m_pNearEnemy->GetHpState().fMaxHp);
			m_bRootMotion = true;
			m_bEnemyHealthDraw = false;
			m_pEnemyHealthBar->SetRender(false);
		}

		if (m_iEnemyIndex == i)
		{
			m_pNearEnemy = *iter;
			m_bRootMotion = true;
			m_pEnemyHealthBar->SetFillAmount(m_pNearEnemy->GetHpState().fCurHp, m_pNearEnemy->GetHpState().fMaxHp);
			m_bEnemyHealthDraw = false;
			m_pEnemyHealthBar->SetRender(false);
			return;
		}

		i++;
	}

}

_float3 CCharacter::LockOnCameraPosition()
{
	if(nullptr == m_pNearEnemy)
		return _float3();

	if(m_pNearEnemy->IsDisable())
		return _float3();

	_float3 vLockPosition;

	_vector vDir = (XMLoadFloat4(&m_pNearEnemy->GetPosition()) - mTransform->Get_State(CTransform::STATE_POSITION)) * 0.7f;
	_float fHeight = XMVectorGetY(mCameraSocketTransform->Get_State(CTransform::STATE_POSITION));
	vDir = XMVectorSetY(vDir, fHeight);

	XMStoreFloat3(&vLockPosition, vDir);

	return vLockPosition;
}

void CCharacter::HoldEnemy()
{
	if (m_pPrevHoldEnemy == nullptr)
		return;
	
	if (dynamic_cast<CBoss*>(m_pPrevHoldEnemy))
		return;

	CTransform* pTargetTransform = static_cast<CTransform*>(static_cast<CGameObject*>(m_pPrevHoldEnemy)->Find_Component(L"com_transform"));
	CCollider* pTargetCollider = static_cast<CCollider*>(static_cast<CGameObject*>(m_pPrevHoldEnemy)->Find_Component(L"com_collider"));
	
	_float fTotalRadius = mCollider->GetExtents().x;// +pTargetCollider->GetExtents().x;
	_vector pTargetPos = pTargetTransform->Get_State(CTransform::STATE_POSITION);
	_vector vPlayerPos = mTransform->Get_State(CTransform::STATE_POSITION);
	_vector vLook = XMVector3Normalize(mTransform->Get_State(CTransform::STATE_LOOK));

	_vector vPosition = vPlayerPos + vLook * fTotalRadius;

	pTargetTransform->Set_State(CTransform::STATE_POSITION, vPosition);
	
}

void CCharacter::Hit()
{
	//대쉬 강화상태 변신중 힛 모션 쓰루
	if (mModel->AnimationCompare(CLIP::MOVE1) ||
		mModel->AnimationCompare(CLIP::MOVE2) ||
		mModel->AnimationCompare(CLIP::ATTACK11) ||
		mModel->AnimationCompare(CLIP::ATTACK21) ||
		mModel->AnimationCompare(CLIP::ATTACK31) ||
		mModel->AnimationCompare(CLIP::ATTACK12) ||
		mModel->AnimationCompare(CLIP::ATTACK22) ||
		mModel->AnimationCompare(CLIP::ATTACK32) ||
		mModel->AnimationCompare(CLIP::ATTACK41) ||
		mModel->AnimationCompare(CLIP::ATTACK42) ||
		mModel->AnimationCompare(CLIP::ATTACK43) ||
		mModel->AnimationCompare(CLIP::ATTACK44) ||
		mModel->AnimationCompare(CLIP::ATTACK45) ||
		mModel->AnimationCompare(CLIP::ATTACK51))
	{
		return;
	}

	if (AnimationCompare(CLIP::MOVE1) || AnimationCompare(CLIP::MOVE2))
		return;

	if (m_bUseSkill)
		return;

	if (m_bEvolution)
		return;

	m_bHit = true;
	m_bMoveable = false;
	m_bDashable = false;
	m_bAttackable = false;
	mWeaponCollider->SetActive(false);

	if (!mModel->AnimationCompare(CLIP::HIT3))
	{
		SetAnimation(CLIP::HIT3, CAnimation::TYPE::ONE);
	}
}

void CCharacter::Airbone()
{
	if (mModel->AnimationCompare(CLIP::MOVE1) || mModel->AnimationCompare(CLIP::MOVE2) || mModel->AnimationCompare(CLIP::ATTACK51))
		return;

	m_bAirbone = true;
	m_iWASDCount = 0;
	m_bMoveable = false;
	m_bDashable = false;
	m_bAttackable = false;
	mWeaponCollider->SetActive(false);

	_vector vPos = mTransform->Get_State(CTransform::STATE_POSITION);
	_vector vLook = mTransform->Get_State(CTransform::STATE_LOOK);
	XMStoreFloat3(&m_vPrevLook, vLook);

	if (!mModel->AnimationCompare(CLIP::HITDOWN))
	{
		XMStoreFloat3(&m_vPrevLook, vLook);
		SetAnimation(CLIP::HITDOWN, CAnimation::TYPE::ONE);
	}

}

void CCharacter::AirboneProcess(_double TimeDelta)
{
	if (mModel->AnimationCompare(CLIP::HITDOWN))
	{
		if (mModel->AnimationIsFinishEx())
		{
			m_bAirbone = false;
			mTransform->Set_State(CTransform::STATE_LOOK, XMLoadFloat3(&m_vPrevLook));
			SetAnimation(CLIP::STAND_UP, CAnimation::TYPE::ONE);
			m_iWASDCount = 0;
		}
	}

	//if (mModel->AnimationCompare(CLIP::STAND_UP))
	//{
	//	if (mModel->AnimationIsFinishEx())
	//	{
	//		mTransform->Set_State(CTransform::STATE_LOOK, XMLoadFloat3(&m_vPrevLook));
	//		m_bAirbone = false;
	//		m_iWASDCount = 0;
	//	}
	//}

	//}
	//else if (mModel->AnimationCompare(CLIP::STAND_UP))
	//{
	//	if (mModel->AnimationIsFinishEx())
	//	{
	//		mTransform->Set_State(CTransform::STATE_RIGHT, XMVector3Cross(VECTOR_UP, XMLoadFloat3(&m_vPrevLook)));
	//		mTransform->Set_State(CTransform::STATE_UP, VECTOR_UP);
	//		mTransform->Set_State(CTransform::STATE_LOOK, XMLoadFloat3(&m_vPrevLook));
	//	}
	//}

	//if (mModel->AnimationCompare(CLIP::FALLDOWN) && !m_vDirUp)
	//{
	//	if (mModel->AnimationIsFinishEx())
	//	{
	//		m_bRootMotion = true;

	//		SetAnimation(CLIP::STAND_UP, CAnimation::TYPE::ONE);
	//		m_bAirbone = false;
	//		m_bMoveable = true;
	//		m_bDashable = true;
	//		m_bAttackable = true;
	//		m_vDirUp = true;

	//		return;
	//	}
	//}

	//if (mModel->AnimationCompare(CLIP::HIT_FLY) && !m_vDirUp)
	//{
	//	if (XMVectorGetY(vPos) <= m_fHeight)
	//	{
	//		m_fAirboneAcc = 0.f;
	//		vPos = XMVectorSetY(vPos, m_fHeight);
	//		mTransform->Set_State(CTransform::STATE_POSITION, vPos);
	//		SetAnimation(CLIP::FALLDOWN, CAnimation::TYPE::ONE);

	//		return;
	//	}
	//}

	//_vector vCurrentPos;
	//_vector vPos = mTransform->Get_State(CTransform::STATE_POSITION);
	//m_fAirboneAcc += TimeDelta * 2.f;
	//if (m_fAirboneAcc >= m_fAirboneTime)
	//{
	//	m_vDirUp = false;
	//	m_bLanding = true;
	//	m_fAirboneAcc = 0.f;
	//}

	//if (XMVectorGetY(vPos) <= 0.f && m_bLanding)
	//{
	//	m_bLanding = false;
	//	m_bAirbone = false;

	//	m_fAirboneAcc = 0.f;
	//	vPos = XMVectorSetY(vPos, 0.f);
	//	mTransform->Set_State(CTransform::STATE_POSITION, vPos);
	//	SetAnimation(CLIP::STAND_UP, CAnimation::TYPE::ONE);

	//	return;
	//}

	//if(m_vDirUp)
	//	vCurrentPos = vPos + (VECTOR_UP * 2.f) * (1.f - m_fAirboneAcc) * TimeDelta;
	//else
	//	vCurrentPos = vPos + (-VECTOR_UP * 2.f) * (1.f - m_fAirboneAcc) * TimeDelta;
	//mTransform->Set_State(CTransform::STATE_POSITION, vCurrentPos);

	return;
}

void CCharacter::RecvDamage(_float fDamage)
{
	//대쉬중 데미지 X
	if (AnimationCompare(CLIP::MOVE1) || AnimationCompare(CLIP::MOVE2))
		return;

	m_fCurHp -= fDamage;
	if (m_fCurHp <= 0.f)
	{
		m_fCurHp = 0.f;
		m_bDie = true;
	}
}

_double CCharacter::Freeze(_double TimeDelta)
{
	if (m_pAppManager->IsFreeze())
	{
		m_pAppManager->SetFreezeReady(false);

		_vector CurTimeDelta = XMVectorSet((_float)m_fCurTimeScale, (_float)m_fCurTimeScale, (_float)m_fCurTimeScale, (_float)m_fCurTimeScale);
		m_fCurTimeScale = XMVectorGetX(XMVectorLerp(CurTimeDelta, XMVectorSet(0.0, 0.0, 0.0, 0.0), (_float)TimeDelta * 0.8f));

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
			//대기쿨타임 돌림
			m_bFreezeReadyWait = true;
			m_pAppManager->SetFreeze(false);
			m_fTimeStopLocal = 0.0f;
			m_fCurTimeScale = 1.0f;
			m_bTimeStop = false;
		}

		return TimeDelta;
	}
	else
		return TimeDelta = TimeDelta * m_fCurTimeScale;
}

void CCharacter::RenderEnemyHealth(_double TimeDelta)
{
	//적 Health UI
	if (m_pNearEnemy != nullptr)
	{
		CEnemy::ENEMY_STATE Health = m_pNearEnemy->GetHpState();
		m_pEnemyHealthBar->SetHealth(Health.fCurHp, Health.fMaxHp);
		m_pEnemyHealthBar->SetRender(m_bEnemyHealthDraw);
	}
	else
	{
		if (m_pEnemyHealthBar)
		{
			m_bEnemyHealthDraw = false;
			m_pEnemyHealthBar->SetRender(false);
		}
	}

	if (m_bEnemyHealthDraw)
	{
		m_fDrawEnemyHealthTimer += (_float)TimeDelta;
		if (m_fDrawEnemyHealthTimer >= m_fDrawEnemyHealthTimeOut)
		{
			m_bEnemyHealthDraw = false;
			m_fDrawEnemyHealthTimer = 0.0f;
		}
	}
}

_float CCharacter::GetDamage()
{
	_float fDamage;

	if (!m_bUseSkill)
	{
		//일반적인 데미지 80 ~ 125
		fDamage = rand() % 40 + 85;
	}
	else
	{
		//스킬 사용중에는 체인 수에 해당하는 공격력을 부여 200% / 400% / 600%
		fDamage = rand() % 40 + 85;
		
		switch (m_SkillInfo.iChainCount)
		{
		case 0:
			fDamage = (rand() % 40 + 85) * 2.f;
			break;
		case 1:
			fDamage = (rand() % 40 + 85) * 4.f;
			break;
		case 2:
			fDamage = (rand() % 40 + 85) * 6.f;
			break;
		default:
			fDamage = rand() % 40 + 85;
			break;
		}
	}

	return fDamage;
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
		AnimationCompare(CLIP::ATTACK2) ||
		AnimationCompare(CLIP::ATTACK3) ||
		AnimationCompare(CLIP::ATTACK4) ||
		AnimationCompare(CLIP::ATTACK5) ||
		AnimationCompare(CLIP::ATTACK41) ||
		AnimationCompare(CLIP::ATTACK42) ||
		AnimationCompare(CLIP::ATTACK43) ||
		AnimationCompare(CLIP::ATTACK44) ||
		AnimationCompare(CLIP::HIT4) ||
		AnimationCompare(CLIP::HIT_FLY) ||
		AnimationCompare(CLIP::FALLDOWN) ||
		AnimationCompare(CLIP::STAND_UP) ||
		AnimationCompare(CLIP::ATTACK45))
	{
		mModel->Play_Animation(TimeDelta, mTransform, 0.03f, m_bRootMotion);
	}
	else if (AnimationCompare(CLIP::ATTACK1) || 
		AnimationCompare(CLIP::ATTACK41) ||
		AnimationCompare(CLIP::ATTACK11) ||
		AnimationCompare(CLIP::ATTACK21) ||
		AnimationCompare(CLIP::ATTACK31) ||
		AnimationCompare(CLIP::ATTACK12) ||
		AnimationCompare(CLIP::ATTACK22) ||
		AnimationCompare(CLIP::ATTACK32))
	{
		if (!dynamic_cast<CBoss*>(m_pPrevHoldEnemy))
		{
			if (m_bEnemyHolding && !m_bSkillYellowAttack)
			{
				m_bRootMotion = false;
				mTransform->MoveForward(TimeDelta * 0.65f);
			}

		}

		if (AnimationCompare(CLIP::STAND2))
			mModel->Play_Animation(TimeDelta, mTransform, 0.01f, m_bRootMotion);
		else
			mModel->Play_Animation(TimeDelta, mTransform, 0.1f, m_bRootMotion);
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
	if (m_bStart)
		return;

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

void CCharacter::LookPos(_fvector vLookPos)
{	
	if (!m_bHit && !m_bEvolution && !m_bUseSkill)
	{
		if (mModel->AnimationCompare(MOVE1) || mModel->AnimationCompare(MOVE2))
			return;

		if (!mModel->AnimationCompare(CLIP::HIT3))
		{
			mTransform->LookAt(vLookPos);
		}
	}
}

void CCharacter::AddEvolutionGage(CSkillBase::SKILL_INFO SkillInfo)
{
	//변신중 스킬사용시 분노게이지 회복
	if (SkillInfo.eType == Client::CSkillBase::TYPE::RED)
	{
		if (!m_bRedAddGageReady)
			return;
		else
			m_bRedAddGageReady = false;
	}
	else if(SkillInfo.eType == Client::CSkillBase::TYPE::YELLOW)
	{
		if (!m_bYellowAddGageReady)
			return;
		else
			m_bYellowAddGageReady = false;
	}
	else if (SkillInfo.eType == Client::CSkillBase::TYPE::BLUE)
	{
		if (!m_bBlueAddGageReady)
			return;
		else
			m_bBlueAddGageReady = false;
	}

	if (m_bEvolution)
	{ 
		if (SkillInfo.iChainCount == 0)
		{
			if (m_fCurEvolutionAcc - 2.5f >= 0.f)
				m_fCurEvolutionAcc -= 2.5f;
			else
				m_fCurEvolutionAcc = 0.f;
		}
		else if (SkillInfo.iChainCount == 1)
		{
			if (m_fCurEvolutionAcc - 4.0f >= 0.f)
				m_fCurEvolutionAcc -= 4.0f;
			else
				m_fCurEvolutionAcc = 0.f;
		}
		else if (SkillInfo.iChainCount == 2)
		{
			if (m_fCurEvolutionAcc - 6.0f >= 0.f)
				m_fCurEvolutionAcc -= 6.0f;
			else
				m_fCurEvolutionAcc = 0.f;
		}
	}
}

void CCharacter::InputMove(_double TimeDelta)
{
	if (m_bStart)
		return;

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

		if (m_bUseSkill)
			return;

		ForwardRotaion(TimeDelta);
		Movement(TimeDelta);
	}

	if (pGameInstance->Input_KeyState_Custom(DIK_DOWNARROW) == KEY_STATE::HOLD)
	{
		if (!m_bMoveable)
			return;

		if (m_bUseSkill)
			return;

		BackwardRotaion(TimeDelta);
		Movement(TimeDelta);
	}

	if (pGameInstance->Input_KeyState_Custom(DIK_LEFTARROW) == KEY_STATE::HOLD)
	{
		if (!m_bMoveable)
			return;

		if (m_bUseSkill)
			return;

		LeftRotation(TimeDelta);
		Movement(TimeDelta);
	}

	if (pGameInstance->Input_KeyState_Custom(DIK_RIGHTARROW) == KEY_STATE::HOLD)
	{
		if (!m_bMoveable)
			return;

		if (m_bUseSkill)
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

CSwordTrail * CCharacter::GetNotUsedEffect()
{
	for (int i = 0; i < SWORD_EFFECT_COUNT; ++i)
	{
		if (!m_pSwordTrail[i]->IsUsed())
			return m_pSwordTrail[i];
	}

	return nullptr;
}

void CCharacter::UseSwordEffect(_float3 vOffsetPos, _float3 fDegreeAngles)
{
	m_pCamera->AttackShake();
	CSwordTrail* pEffect = GetNotUsedEffect();
	if (nullptr != pEffect)
		pEffect->EffectStart(vOffsetPos, fDegreeAngles);
}

void CCharacter::AttackEffectControl(_double TimeDelta)
{
	if (!m_bEvolution)
	{
		//강화상태X

#pragma region 기본공격 강화X
		if (mModel->AnimationCompare(ATTACK1))
		{
			if (!m_bUseAttack1 && mModel->GetCurrentTimeAcc() >= 0.25)
			{
				m_bUseAttack1 = true;
				UseSwordEffect(_float3(0.f, 1.5f, 0.7f), _float3(0.f, 0.f, 170.f));
			}
		}
		else
			m_bUseAttack1 = false;

		if (mModel->AnimationCompare(ATTACK2))
		{
			if (!m_bUseAttack2 && mModel->GetCurrentTimeAcc() >= 0.1)
			{
				m_bUseAttack2 = true;
				UseSwordEffect(_float3(-0.5f, 1.3f, 0.7f), _float3(0.f, 0.f, 290.f));
			}
		}
		else
			m_bUseAttack2 = false;

		if (mModel->AnimationCompare(ATTACK3))
		{
			if (!m_bUseAttack3[0] && mModel->GetCurrentTimeAcc() >= 0.2)
			{
				m_bUseAttack3[0] = true;
				UseSwordEffect(_float3(0.0f, 1.3f, 0.7f), _float3(0.f, 0.f, 190.f));
			}

			if (!m_bUseAttack3[1] && mModel->GetCurrentTimeAcc() >= 0.4)
			{
				m_bUseAttack3[1] = true;
				UseSwordEffect(_float3(0.2f, 1.3f, 0.7f), _float3(-90.f, 0.f, 90.f));
			}
		}
		else
		{
			m_bUseAttack3[0] = false;
			m_bUseAttack3[1] = false;
			m_bUseAttack3[2] = false;
		}

		if (mModel->AnimationCompare(ATTACK4))
		{
			if (!m_bUseAttack4 && mModel->GetCurrentTimeAcc() >= 0.25)
			{
				m_bUseAttack4 = true;
				UseSwordEffect(_float3(0.25f, 1.1f, 1.3f), _float3(0.f, 0.f, 30.f));
			}
		}
		else
		{
			m_bUseAttack4 = false;
		}

		if (mModel->AnimationCompare(ATTACK5))
		{
			if (!m_bUseAttack5[0] && mModel->GetCurrentTimeAcc() >= 0.33)
			{
				m_bUseAttack5[0] = true;
				UseSwordEffect(_float3(0.2f, 1.2f, 0.7f), _float3(0.f, 0.f, 170.f));
			}
		}
		else
		{
			m_bUseAttack5[0] = false;
			m_bUseAttack5[1] = false;
			m_bUseAttack5[2] = false;
			m_bUseAttack5[3] = false;
			m_bUseAttack5[4] = false;
		}

#pragma endregion

#pragma region 스킬 강화 X
		if (mModel->AnimationCompare(ATTACK11))
		{
			if (!m_bUseSkillA[0] && mModel->GetCurrentTimeAcc() >= 0.30)
			{
				m_bUseSkillA[0] = true;
				UseSwordEffect(_float3(0.f, 1.2f, 0.7f), _float3(0.f, 0.f, 180.f));
			}

			if (!m_bUseSkillA[1] && mModel->GetCurrentTimeAcc() >= 0.6)
			{
				m_bUseSkillA[1] = true;
				UseSwordEffect(_float3(-0.3f, 1.2f, 0.7f), _float3(0.f, 0.f, 300.f));
			}
		}
		else
		{
			m_bUseSkillA[0] = false;
			m_bUseSkillA[1] = false;
		}

		if (mModel->AnimationCompare(ATTACK21))
		{
			if (!m_bUseSkillB && mModel->GetCurrentTimeAcc() >= 0.25)
			{
				m_bUseSkillB = true;
				UseSwordEffect(_float3(0.f, 1.0f, 0.7f), _float3(0.f, 0.f, 310.f));
			}
		}
		else
		{
			m_bUseSkillB = false;
		}

		if (mModel->AnimationCompare(ATTACK31))
		{
			if (!m_bUseSkillC[0] && mModel->GetCurrentTimeAcc() >= 0.75)
			{
				m_bUseSkillC[0] = true;
				UseSwordEffect(_float3(0.f, 1.3f, 0.7f), _float3(10.f, 0.f, 195.f));
			}
		}
		else
		{
			m_bUseSkillC[0] = false;
		}

#pragma endregion

	}
	else
	{
		//강화상태
#pragma region 기본공격 강화

		if (mModel->AnimationCompare(ATTACK41))
		{
			if (!m_bUseAttack1 && mModel->GetCurrentTimeAcc() >= 0.11)
			{
				m_bUseAttack1 = true;
				UseSwordEffect(_float3(0.f, 1.3f, 0.7f), _float3(0.f, 0.f, 180.f));
			}
		}
		else
			m_bUseAttack1 = false;

		if (mModel->AnimationCompare(ATTACK42))
		{
			if (!m_bUseAttack2 && mModel->GetCurrentTimeAcc() >= 0.11)
			{
				m_bUseAttack2 = true;
				UseSwordEffect(_float3(0.2f, 1.8f, 0.7f), _float3(0.f, 0.f, 145.f));
			}
		}
		else
			m_bUseAttack2 = false;

		if (mModel->AnimationCompare(ATTACK43))
		{
			if (!m_bUseAttack3[0] && mModel->GetCurrentTimeAcc() >= 0.1)
			{
				m_bUseAttack3[0] = true;
				UseSwordEffect(_float3(-0.2f, 0.8f, 0.8f), _float3(0.f, 0.f, 290.f));
			}

			if (!m_bUseAttack3[1] && mModel->GetCurrentTimeAcc() >= 0.4)
			{
				m_bUseAttack3[1] = true;
				UseSwordEffect(_float3(-0.2f, 0.8f, 0.7f), _float3(0.f, 0.f, 300.f));
			}

			if (!m_bUseAttack3[2] && mModel->GetCurrentTimeAcc() >= 0.8)
			{
				m_bUseAttack3[2] = true;
				UseSwordEffect(_float3(-0.2f, 1.1f, 0.7f), _float3(0.f, 0.f, 230.f));
			}

		}
		else
		{
			m_bUseAttack3[0] = false;
			m_bUseAttack3[1] = false;
			m_bUseAttack3[2] = false;
		}

		if (mModel->AnimationCompare(ATTACK44))
		{
			if (!m_bUseAttack4 && mModel->GetCurrentTimeAcc() >= 0.11)
			{
				m_bUseAttack4 = true;
				UseSwordEffect(_float3(0.0f, 1.2f, 1.f), _float3(0.f, 0.f, 170.f));
			}
		}
		else
		{
			m_bUseAttack4 = false;
		}

		if (mModel->AnimationCompare(ATTACK45))
		{
			//1타
			if (!m_bUseAttack5[0] && mModel->GetCurrentTimeAcc() >= 0.08)
			{
				m_bUseAttack5[0] = true;
				UseSwordEffect(_float3(0.25f, 1.6f, 1.f), _float3(-90.f, 0.f, 90.f));
			}

			//2타
			if (!m_bUseAttack5[1] && mModel->GetCurrentTimeAcc() >= 0.14)
			{
				m_bUseAttack5[1] = true;
				UseSwordEffect(_float3(0.25f, 2.f, 1.f), _float3(-90.f, 0.f, 90.f));
			}

			//3타
			if (!m_bUseAttack5[2] && mModel->GetCurrentTimeAcc() >= 0.2)
			{
				m_bUseAttack5[2] = true;
				UseSwordEffect(_float3(0.25f, 2.f, 1.f), _float3(-90.f, 0.f, 90.f));
			}

			//4타
			if (!m_bUseAttack5[3] && mModel->GetCurrentTimeAcc() >= 0.26)
			{
				m_bUseAttack5[3] = true;
				UseSwordEffect(_float3(0.25f, 2.f, 1.f), _float3(-90.f, 0.f, 90.f));
			}

			//5타
			if (!m_bUseAttack5[4] && mModel->GetCurrentTimeAcc() >= 0.32)
			{
				m_bUseAttack5[4] = true;
				UseSwordEffect(_float3(0.25f, 1.7f, 1.f), _float3(-90.f, 0.f, 90.f));
			}

		}
		else
		{
			m_bUseAttack5[0] = false;
			m_bUseAttack5[1] = false;
			m_bUseAttack5[2] = false;
			m_bUseAttack5[3] = false;
			m_bUseAttack5[4] = false;
		}

#pragma endregion

#pragma region 스킬공격 강화
		if (mModel->AnimationCompare(ATTACK12))
		{
			if (!m_bUseSkillA[0] && mModel->GetCurrentTimeAcc() >= 0.3)
			{
				m_bUseSkillA[0] = true;
				UseSwordEffect(_float3(0.4f, 1.0f, 0.7f), _float3(-90.f, 0.f, 90.f));
			}

			if (!m_bUseSkillA[1] && mModel->GetCurrentTimeAcc() >= 0.7f)
			{
				m_bUseSkillA[1] = true;
				UseSwordEffect(_float3(0.35f, 1.0f, 0.7f), _float3(-90.f, 0.f, 95));
			}

			if (!m_bUseSkillA[2] && mModel->GetCurrentTimeAcc() >= 1.2)
			{
				m_bUseSkillA[2] = true;
				UseSwordEffect(_float3(0.3f, 1.0f, 0.7f), _float3(-90.f, 0.f, 100.f));
			}

			if (!m_bUseSkillA[3] && mModel->GetCurrentTimeAcc() >= 1.7f)
			{
				m_bUseSkillA[3] = true;
				UseSwordEffect(_float3(0.f, 1.3f, 0.7f), _float3(0.f, 0.f, 150.f));
			}
		}
		else
		{
			m_bUseSkillA[0] = false;
			m_bUseSkillA[1] = false;
			m_bUseSkillA[2] = false;
			m_bUseSkillA[3] = false;
		}


		if (mModel->AnimationCompare(ATTACK32))
		{
			if (!m_bUseSkillC[0] && mModel->GetCurrentTimeAcc() >= 0.2)
			{
				m_bUseSkillC[0] = true;
				UseSwordEffect(_float3(0.f, 1.f, 0.7f), _float3(0.f, 0.f, 20.f));
			}

			if (!m_bUseSkillC[1] && mModel->GetCurrentTimeAcc() >= 0.45f)
			{
				m_bUseSkillC[1] = true;
				UseSwordEffect(_float3(0.f, 1.f, 0.7f), _float3(0.f, 0.f, 160.f));
			}

			if (!m_bUseSkillC[2] && mModel->GetCurrentTimeAcc() >= 0.7f)
			{
				m_bUseSkillC[2] = true;
				UseSwordEffect(_float3(0.f, 1.f, 0.7f), _float3(0.f, 0.f, 20.f));
			}

			if (!m_bUseSkillC[3] && mModel->GetCurrentTimeAcc() >= 0.95f)
			{
				m_bUseSkillC[3] = true;
				UseSwordEffect(_float3(0.f, 1.f, 0.7f), _float3(0.f, 0.f, 160.f));
			}

			if (!m_bUseSkillC[4] && mModel->GetCurrentTimeAcc() >= 1.2f)
			{
				m_bUseSkillC[4] = true;
				UseSwordEffect(_float3(0.f, 0.8f, 0.7f), _float3(-90.f, 55.f, 90.f));
			}
		}
		else
		{
			m_bUseSkillC[0] = false;
			m_bUseSkillC[1] = false;
			m_bUseSkillC[2] = false;
			m_bUseSkillC[3] = false;
			m_bUseSkillC[4] = false;
		}
#pragma endregion

	}

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

	Safe_Release(m_pHealthBar);
	Safe_Release(m_pPlayerIcon);

	Safe_Release(mDashCheckCollider);
	Safe_Release(mWallCheckCollider);
	Safe_Release(mWeaponCollider);
	Safe_Release(mEnemyCheckCollider);
	Safe_Release(mSkillCollider);
	Safe_Release(mCollider);
	Safe_Release(mRenderer);
	Safe_Release(mTransform);
	Safe_Release(mCameraSocketTransform);
	Safe_Release(mModel);
	Safe_Release(mShader);

	for (int i = 0; i < SWORD_EFFECT_COUNT; ++i)
		Safe_Release(m_pSwordTrail[i]);
}

void CCharacter::OnCollisionEnter(CCollider * src, CCollider * dest)
{
	if (src->Compare(mEnemyCheckCollider))
	{
		CEnemy* pEnemy = dynamic_cast<CEnemy*>(dest->GetOwner());
		if (pEnemy && dest->Compare(pEnemy->GetOverlapCollider()))
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
		if (pEnemy && dest->Compare(pEnemy->GetWeaponCollider()))
		{
			if(pAppManager->IsFreezeReady())
				pAppManager->SetFreeze(true);
		}

		CThorn* pThorn = dynamic_cast<CThorn*>(dest->GetOwner());
		if (pThorn)
		{
			if (pAppManager->IsFreezeReady())
				pAppManager->SetFreeze(true);
		}

		CBoss* pBoss = dynamic_cast<CBoss*>(dest->GetOwner());
		if (pBoss)
		{
			//바디와 오버랩 콜라이더가 아닐경우 무조건 다 공격용 콜라이더임
			if (!dest->Compare(pEnemy->GetBodyCollider()) && !dest->Compare(pEnemy->GetOverlapCollider()))
			{
				if (pAppManager->IsFreezeReady())
					pAppManager->SetFreeze(true);
			}
		}
	}

	if (src->Compare(mCollider))
	{
		if (mModel->AnimationCompare(CLIP::ATTACK31) || mModel->AnimationCompare(CLIP::ATTACK32))
		{
			CEnemy* pEnemy = dynamic_cast<CEnemy*>(dest->GetOwner());
			if (pEnemy && dest->Compare(pEnemy->GetBodyCollider()))
			{
				m_bEnemyHealthDraw = true;
				m_fDrawEnemyHealthTimer = 0.0f;
				AddCombo();
			}
		}

		CBoss* pEnemy = dynamic_cast<CBoss*>(dest->GetOwner());
		if (pEnemy)
		{
			//바디와 오버랩 콜라이더가 아닐경우 무조건 다 공격용 콜라이더임
			if (!dest->Compare(pEnemy->GetBodyCollider()) && !dest->Compare(pEnemy->GetOverlapCollider()))
			{
				Hit();
				RecvDamage(50.f);
			}
		}

	}

	if (src->Compare(mWeaponCollider))
	{
		CEnemy* pEnemy = dynamic_cast<CEnemy*>(dest->GetOwner());
		if (pEnemy && dest->Compare(pEnemy->GetBodyCollider()))
		{
			m_bEnemyHealthDraw = true;
			m_fDrawEnemyHealthTimer = 0.0f;

			if (pEnemy == m_pNearEnemy)
			{
				m_iPrevClip = (CLIP)ANIM_DESC.Clip;
				//m_pCamera->AttackShake();
				m_pEnemyHealthBar->StartShake();
				m_pPlayerIcon->StartShake();

				for (int i = 0; i < SWORD_EFFECT_COUNT; ++i)
				{
					if (m_pSwordTrail[i]->IsUsed())
					{
						m_pSwordTrail[i]->SetHit(true);
						m_pSwordTrail[i]->SetHitPosition(XMLoadFloat4(&m_pNearEnemy->GetPosition()));
					}
				}

				if (!m_bEvolution)
					m_EvolutionCount++;

				AddCombo();
				ResetComboTime();
			}
		}
	}

	if (src->Compare(mSkillCollider))
	{
		CEnemy* pEnemy = dynamic_cast<CEnemy*>(dest->GetOwner());
		if (pEnemy && dest->Compare(pEnemy->GetBodyCollider()))
		{
			pEnemy->SetAirborne(GetDamage());
			m_bEnemyHealthDraw = true;
			m_fDrawEnemyHealthTimer = 0.0f;

			if (pEnemy == m_pNearEnemy)
			{
				m_pCamera->AttackShake();
				m_pEnemyHealthBar->StartShake();
				m_pPlayerIcon->StartShake();

				if(!m_bEvolution)
					m_EvolutionCount++;

				AddCombo();
				ResetComboTime();
			}
		}
	}
}

void CCharacter::OnCollisionStay(CCollider * src, CCollider * dest)
{
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
			{
				m_bRootMotion = false;
			}
			else
			{
				if(m_bRootMotion)
					m_bRootMotion = true;
			}

			//if (mModel->AnimationCompare(CLIP::ATTACK11) ||
			//	mModel->AnimationCompare(CLIP::ATTACK21) || 
			//	mModel->AnimationCompare(CLIP::ATTACK12) ||
			//	mModel->AnimationCompare(CLIP::ATTACK22))
			//	m_bRootMotion = false;

			if (mModel->AnimationCompare(CLIP::ATTACK31) && m_pPrevHoldEnemy == pEnemy ||
				mModel->AnimationCompare(CLIP::ATTACK32) && m_pPrevHoldEnemy == pEnemy)
			{
				CBoss* pBoss = dynamic_cast<CBoss*>(m_pNearEnemy);
				if (!pBoss)
				{
					if (!m_bEnemyHolding)
					{
						pEnemy->SetHold(true);
						m_bEnemyHolding = true;
						m_bRootMotion = true;
					}
				}
			}
			
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
			m_WallHit = false;
		}
	}

	if (src->Compare(mEnemyCheckCollider))
	{
		CEnemy* pEnemy = dynamic_cast<CEnemy*>(dest->GetOwner());
		if (pEnemy)
			DeleteTargetFromList(dest->GetOwner());
	}

}
