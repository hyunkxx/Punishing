#include "pch.h"
#include "..\Public\Character.h"

#include "GameInstance.h"
#include "Weapon.h"
#include "Bone.h"

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

	mTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));

	_float4x4 camMatrix;
	XMStoreFloat4x4(&camMatrix, XMMatrixIdentity());
	mCameraSocketTransform->Set_WorldMatrix(camMatrix);
	
	return S_OK;
}

void CCharacter::Tick(_double TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	__super::Tick(TimeDelta);

	KeyInput(TimeDelta);
	Dash(TimeDelta);
	Attack();

}

void CCharacter::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	AnimationControl(TimeDelta);

	CameraSocketUpdate();

	if (m_bOnTerrain)
	{
		_vector vPosition = mTransform->Get_State(CTransform::STATE_POSITION);
		_float fOverY = XMVectorGetY(vPosition) - 0.2f;

		if (fOverY < 0.f)
		{
			vPosition = XMVectorSetY(vPosition, XMVectorGetY(vPosition) + abs(fOverY));
			mTransform->Set_State(CTransform::STATE_POSITION, vPosition);
		}

		//_vector vPosition = mTransform->Get_State(CTransform::STATE_POSITION);
		//vPosition = XMVectorSetY(vPosition, 0.f);
		//mTransform->Set_State(CTransform::STATE_POSITION, vPosition);
	}

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
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("proto_com_renderer"), TEXT("com_renderer"), (CComponent**)&mRenderer)))
		return E_FAIL;

	CTransform::TRANSFORM_DESC TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORM_DESC));

	TransformDesc.fMoveSpeed = m_fMoveSpeed;
	TransformDesc.fRotationSpeed = XMConvertToRadians(m_fRotationSpeed);

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("proto_com_transform"), TEXT("com_transform"), (CComponent**)&mTransform, &TransformDesc)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("proto_com_transform"), TEXT("com_camera_socket_transform"), (CComponent**)&mCameraSocketTransform, &TransformDesc)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("proto_com_shader_vtxanimmodel"), TEXT("com_shader"), (CComponent**)&mShader)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("proto_com_model_kamui"), TEXT("com_model"), (CComponent**)&mModel)))
		return E_FAIL;

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
	{
		Idle();
	}

}

void CCharacter::Dash(_double TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (!AnimationCompare(CLIP::MOVE1) && !AnimationCompare(CLIP::MOVE2))
	{
		m_bDash = false;
		m_bAttackable = true;
	}

	if (AnimationCompare(CLIP::MOVE1) || AnimationCompare(CLIP::MOVE2))
	{
		m_bDashReady = false;

		if(mModel->AnimationIsPreFinish())
			m_bAttackable = true;
	}

	if (m_bDashReady)
		m_fDashTimer += TimeDelta;
	else
		m_fDashTimer = 0.0f;


	if (m_fDashTimer > 0.07f)
	{
		m_bDashReady = false;
		m_fDashTimer = 0.0;
	}

	if (!m_bDashReady)
	{
		if (pGameInstance->Input_KeyState_Custom(DIK_W) == KEY_STATE::AWAY ||
			(pGameInstance->Input_KeyState_Custom(DIK_S) == KEY_STATE::AWAY))
		{
			m_bDashReady = true;
		}
	}
	else
	{
		if ((pGameInstance->Input_KeyState_Custom(DIK_W) == KEY_STATE::TAP))
		{
			SetAnimation(CLIP::MOVE1, CAnimation::ONE);
			m_bAttackable = false;
			m_bDashReady = false;
			m_bDash = true;
		}

		if ((pGameInstance->Input_KeyState_Custom(DIK_S) == KEY_STATE::TAP))
		{
			SetAnimation(CLIP::MOVE2, CAnimation::ONE);
			m_bAttackable = false;
			m_bDashReady = false;
			m_bDash = true;
		}
	}

}

void CCharacter::Idle()
{
	m_bMoveable = true;
	m_bAttackable = true;

	SetAnimation(CLIP::STAND2, CAnimation::TYPE::LOOP);
}

void CCharacter::MoveForward(_double TimeDelta)
{
	if (!m_bMoveable)
		return;
	
	ForwardRotaion(TimeDelta);
	Movement(TimeDelta);
}

void CCharacter::MoveBackward(_double TimeDelta)
{
	if (!m_bMoveable)
		return;

	BackwardRotaion(TimeDelta);
	Movement(TimeDelta);
}

void CCharacter::MoveRight(_double TimeDelta)
{
	if (!m_bMoveable)
		return;
	  
	RigthRotation(TimeDelta);
	Movement(TimeDelta);
}

void CCharacter::MoveLeft(_double TimeDelta)
{
	if (!m_bMoveable)
		return;

	LeftRotation(TimeDelta);
	Movement(TimeDelta);
}

void CCharacter::MoveStop(_double TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	m_fStopTimer += TimeDelta;

	if (m_fStopTimer >= m_fStopTimeOut)
	{
		if (AnimationCompare(CLIP::RUN) || AnimationCompare(CLIP::RUN_START))
		{
			m_bMove = false;
			m_bRun = false;
			m_bMoveable = true;
			SetAnimation(CLIP::STOP, CAnimation::TYPE::ONE);
		}
	}
	else
	{
		if(AnimationCompare(CLIP::RUN))
			mTransform->MoveForward(TimeDelta);

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
	{
		mModel->Play_Animation(TimeDelta, mTransform, ANIM_DESC.Type);
	}

}

void CCharacter::CameraSocketUpdate()
{
	_vector cameraPos = mTransform->Get_State(CTransform::STATE_POSITION);
	cameraPos = XMVectorSetY(cameraPos, XMVectorGetY(cameraPos) + 1);

	mCameraSocketTransform->Set_State(CTransform::STATE_POSITION, cameraPos);
}

void CCharacter::ForwardRotaion(_double TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (m_bDash)
		return;

	_matrix cameraMatrix = pGameInstance->Get_Transform_Matrix_Inverse(CPipeLine::TRANSFORM_STATE::TS_VIEW);
	_vector vCameraRight = cameraMatrix.r[0];
	_vector vCameraLook = XMVector3Normalize(XMVector3Cross(vCameraRight, VECTOR_UP));
	_vector vPlayerLook = XMVector3Normalize(mTransform->Get_State(CTransform::STATE_LOOK));

	_float fRadianAngle = acos(XMVectorGetX(XMVector3Dot(vCameraLook, vPlayerLook)));
	_float AxisY = XMVectorGetY(XMVector3Cross(vPlayerLook, vCameraLook));

	if(AxisY >= 0.f)
		mTransform->Rotate(VECTOR_UP, TimeDelta * fRadianAngle);
	else
		mTransform->Rotate(VECTOR_UP, TimeDelta * -fRadianAngle);

}

void CCharacter::BackwardRotaion(_double TimeDelta)
{
	if (m_bDash)
		return;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	_matrix cameraMatrix = pGameInstance->Get_Transform_Matrix_Inverse(CPipeLine::TRANSFORM_STATE::TS_VIEW);
	_vector vCameraRight = cameraMatrix.r[0];
	_vector vCameraLook = -XMVector3Normalize(XMVector3Cross(vCameraRight, VECTOR_UP));
	_vector vPlayerLook = XMVector3Normalize(mTransform->Get_State(CTransform::STATE_LOOK));

	_float fRadianAngle = acos(XMVectorGetX(XMVector3Dot(vCameraLook, vPlayerLook)));
	_float AxisY = XMVectorGetY(XMVector3Cross(vPlayerLook, vCameraLook));

	if (AxisY >= 0.f)
		mTransform->Rotate(VECTOR_UP, TimeDelta * fRadianAngle);
	else
		mTransform->Rotate(VECTOR_UP, TimeDelta * -fRadianAngle);
}

void CCharacter::RigthRotation(_double TimeDelta)
{
	if (m_bDash)
		return;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	_matrix cameraMatrix = pGameInstance->Get_Transform_Matrix_Inverse(CPipeLine::TRANSFORM_STATE::TS_VIEW);
	_vector vCameraRight = cameraMatrix.r[0];
	_vector vCameraLook = -XMVector3Normalize(XMVector3Cross(vCameraRight, VECTOR_UP));
	_vector vPlayerRight = XMVector3Normalize(mTransform->Get_State(CTransform::STATE_RIGHT));

	_float fRadianAngle = acos(XMVectorGetX(XMVector3Dot(vCameraLook, vPlayerRight)));
	_float AxisY = XMVectorGetY(XMVector3Cross(vPlayerRight, vCameraLook));
   
	if (AxisY >= 0.f)
		mTransform->Rotate(VECTOR_UP, TimeDelta * fRadianAngle);
	else
		mTransform->Rotate(VECTOR_UP, TimeDelta * -fRadianAngle);
}

void CCharacter::LeftRotation(_double TimeDelta)
{
	if (m_bDash)
		return;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	_matrix cameraMatrix = pGameInstance->Get_Transform_Matrix_Inverse(CPipeLine::TRANSFORM_STATE::TS_VIEW);
	_vector vCameraRight = cameraMatrix.r[0];
	_vector vCameraLook = XMVector3Normalize(XMVector3Cross(vCameraRight, VECTOR_UP));
	_vector vPlayerRight = XMVector3Normalize(mTransform->Get_State(CTransform::STATE_RIGHT));

	_float fRadianAngle = acos(XMVectorGetX(XMVector3Dot(vCameraLook, vPlayerRight)));
	_float AxisY = XMVectorGetY(XMVector3Cross(vPlayerRight, vCameraLook));

	if (AxisY >= 0.f)
		mTransform->Rotate(VECTOR_UP, TimeDelta * fRadianAngle);
	else
		mTransform->Rotate(VECTOR_UP, TimeDelta * -fRadianAngle);
}

void CCharacter::Movement(_double TimeDelta)
{
	if (AnimationCompare(CLIP::STAND1) || AnimationCompare(CLIP::STAND2) || AnimationCompare(CLIP::RUN_START_END) || AnimationCompare(CLIP::STOP))
	{
		m_bMove = true;
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
	else
	{
		m_fStopTimer = 0.0;
	}


	TimeDelta = TimeDelta / m_iWASDCount;

	if (pGameInstance->Input_KeyState_Custom(DIK_W) == KEY_STATE::HOLD)
	{
		MoveForward(TimeDelta);
	}

	if (pGameInstance->Input_KeyState_Custom(DIK_S) == KEY_STATE::HOLD)
	{
		MoveBackward(TimeDelta);
	}

	if (pGameInstance->Input_KeyState_Custom(DIK_A) == KEY_STATE::HOLD)
	{
		MoveLeft(TimeDelta);
	}

	if (pGameInstance->Input_KeyState_Custom(DIK_D) == KEY_STATE::HOLD)
	{
		MoveRight(TimeDelta);
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

	Safe_Release(mRenderer);
	Safe_Release(mTransform);
	Safe_Release(mCameraSocketTransform);
	Safe_Release(mModel);
	Safe_Release(mShader);
}
