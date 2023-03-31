#include "pch.h"
#include "..\Public\PlayerCamera.h"
#include "GameInstance.h"
#include "Character.h"
#include "Transform.h"
#include "Model.h"
#include "Enemy.h"
#include "Bone.h"
#include "Boss.h"

#include "ApplicationManager.h"
#include "SkillBallSystem.h"

CPlayerCamera::CPlayerCamera(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CCamera(pDevice, pContext)
{
}

CPlayerCamera::CPlayerCamera(const CPlayerCamera & rhs)
	: CCamera(rhs)
{
}

HRESULT CPlayerCamera::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayerCamera::Initialize(void * pArg)
{
	m_pTransform = CTransform::Create(m_pDevice, m_pContext);
	if (nullptr == m_pTransform)
		return E_FAIL;
	
	assert(pArg);
	m_pTarget = (CGameObject*)pArg;

	m_pTargetTransform = static_cast<CTransform*>(m_pTarget->Find_Component(L"com_transform"));
	m_pSocketTransform = static_cast<CTransform*>(m_pTarget->Find_Component(L"com_camera_socket_transform"));

	m_CameraDesc.fFovy = XMConvertToRadians(45.f);
	m_CameraDesc.fAspect = g_iWinSizeX / (_float)g_iWinSizeY;
	m_CameraDesc.fNear = 0.1f;
	m_CameraDesc.fFar = 1500.f;

	m_pTransform->Set_State(CTransform::STATE_POSITION, m_pTargetTransform->Get_State(CTransform::STATE_POSITION));
	XMStoreFloat4(&vLookTarget, m_pTransform->Get_State(CTransform::STATE_POSITION));
	
	CApplicationManager::GetInstance()->SetWinMotion(false);
	if (CApplicationManager::GetInstance()->IsLevelFinish(CApplicationManager::LEVEL::GAMEPLAY))
	{
		m_pTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));
		m_pTransform->LookAt(m_pTargetTransform->Get_State(CTransform::STATE_POSITION));
	}
	else
	{
		//m_pTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));
		//m_pTransform->LookAt(m_pTargetTransform->Get_State(CTransform::STATE_POSITION));
	}

	CCollider::COLLIDER_DESC collDesc;
	collDesc.owner = this;
	collDesc.vCenter = _float3(0.f, 0.f, 0.f);
	collDesc.vExtents = _float3(2.5f, 3.f, 7.5f);
	collDesc.vRotation = _float3(0.f, 0.f, 0.f);

	(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("proto_com_obb_collider"), TEXT("com_collider"), (CComponent**)&m_pCollider, &collDesc));

	//Fade in out
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("proto_com_renderer"),
		TEXT("com_renderer"), (CComponent**)&m_pRenderer)))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("proto_com_vibuffer_rect"),
		TEXT("com_buffer"), (CComponent**)&m_pVIBuffer)))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("proto_com_texture_black"),
		TEXT("com_texture"), (CComponent**)&m_pTexture)))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("proto_com_shader_ui"),
		TEXT("com_shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	m_fWidth = g_iWinSizeX;
	m_fHeight = g_iWinSizeY;
	m_fX = g_iWinSizeX >> 1;
	m_fY = g_iWinSizeY >> 1;

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixScaling(m_fWidth, m_fHeight, 1.f) * XMMatrixTranslation(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f));
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));

	return S_OK;
}

void CPlayerCamera::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	if (m_bFinish && m_fAlpha >= 1.f)
	{
		m_fAlpha = 1.f;
		m_bFinish = false;
		if(!CApplicationManager::GetInstance()->IsLevelFinish(CApplicationManager::LEVEL::GAMEPLAY))
			CApplicationManager::GetInstance()->SetLevelFinish(CApplicationManager::LEVEL::GAMEPLAY);
		else
			CApplicationManager::GetInstance()->SetLevelFinish(CApplicationManager::LEVEL::BOSS);
	}

	if (m_bFadeIn)
	{
		m_fAlpha += TimeDelta;

		if (m_fAlpha >= 1.f)
			m_bFadeIn = false;
	}

	if (m_bFadeOut)
	{
		m_fAlpha -= TimeDelta;

		if (m_fAlpha < 0.f)
		{
			m_bFadeOut = false;
		}
	}

	if (!m_bWinAction)
		DefaultCameraMovement(TimeDelta);
	else
		WinActionMovement(TimeDelta);
}

void CPlayerCamera::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (pGameInstance->Input_KeyState_Custom(DIK_8) == KEY_STATE::TAP)
	{
		m_bFadeIn = true;
		m_bFadeInStart = true;
	}

	if (m_bFadeInStart)
	{
		CSkillBallSystem* pSkillSystem = CSkillBallSystem::GetInstance();
		pSkillSystem->Clear();

		m_fFadeInWaitAcc += TimeDelta;
		if (m_fFadeInWaitAcc >= 1.f)
		{
			m_bFadeIn = false;
			m_bFadeInStart = false;
			m_bFadeOut = true;
			m_fFadeInWaitAcc = 0.f;
			static_cast<CCharacter*>(m_pTarget)->SetWinMotion(true);
			m_bWinAction = !m_bWinAction;
		}
	}

	if (m_bFadeOutStart)
	{
		m_fFadeOutWaitAcc += TimeDelta;
		if (m_fFadeOutWaitAcc >= 1.f)
		{
			m_bFadeOut = false;
			m_bFadeOutStart = false;
			m_fFadeOutWaitAcc = 0.f;

		}
	}

	//이거 풀면 콜리전 보임
	if (!m_bFadeIn && !m_bFadeOut)
		return;

	if (nullptr != m_pRenderer)
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_ENDING, this);
}

HRESULT CPlayerCamera::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	_float fValue = 1.f;
	_float fDiscardValue = 0.f;

	//버튼 백그라운드 이미지
	if (FAILED(m_pTexture->Setup_ShaderResource(m_pShader, "g_Texture")))
		return E_FAIL;

	if (FAILED(m_pShader->SetRawValue("g_FillAmount", &fValue, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShader->SetRawValue("g_Alpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShader->SetRawValue("g_DiscardValue", &fDiscardValue, sizeof(_float))))
		return E_FAIL;

	m_pShader->Begin(0);
	m_pVIBuffer->Render();

	return S_OK;
}

void CPlayerCamera::AttackShake()
{
	if (!CApplicationManager::GetInstance()->IsSpawned())
		return;

	ShakeReset();
	StartShake(4.f, 2.5f);
}

void CPlayerCamera::ThornShake()
{
	ShakeReset();
	StartShake(2.f, 1.5f);
}


HRESULT CPlayerCamera::Add_Components()
{
	return S_OK;
}

HRESULT CPlayerCamera::Setup_ShaderResources()
{
	return S_OK;
}

void CPlayerCamera::DefaultCameraMovement(_double TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	_vector vTargetPos = m_pTargetTransform->Get_State(CTransform::STATE_POSITION);
	_vector vTargetLook = XMVector3Normalize(m_pTargetTransform->Get_State(CTransform::STATE_LOOK));
	vTargetPos = XMVectorSetY(vTargetPos, 1.5f);

	//_vector vCameraPosition = vTargetPos - XMLoadFloat3(&vDistance);

	//vCameraPosition = vTargetPos - (vTargetLook * vDistance.z);
	//vCameraPosition = XMVectorSetY(vCameraPosition, vDistance.y);
	//
	//if (pGameInstance->Input_MouseState_Custom(DIMK_RB) == KEY_STATE::HOLD)
	//	m_bMouseLock = false;
	//else
	//	m_bMouseLock = true;
	//
	//if (!m_bMouseLock)
	//{
	//	_long MouseMove = 0;
	//	if (MouseMove = pGameInstance->Input_MouseMove(DIMM_X))
	//	{
	//		m_pTargetTransform->Rotate(VECTOR_UP, MouseMove * TimeDelta * 0.1f);
	//	}
	//}
	
	//if (MouseMove = pGameInstance->Input_MouseMove(DIMM_Y))
	//	m_pTransform->Rotate(m_pTransform->Get_State(CTransform::STATE_RIGHT), MouseMove * TimeDelta * 0.1f);

	//카메라가 락온 되었을때
	if (static_cast<CCharacter*>(m_pTarget)->IsCameraLockOn())
	{
		CTransform* pPlayerTransform = (CTransform*)static_cast<CCharacter*>(m_pTarget)->Find_Component(L"com_transform");
		_vector vCameraPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION) - XMLoadFloat3(&static_cast<CCharacter*>(m_pTarget)->LockOnCameraPosition());

		_vector vDistance = XMLoadFloat4(&((CEnemy*)(static_cast<CCharacter*>(m_pTarget)->GetLockOnTarget()))->GetPosition()) - vCameraPos;
		_float fLength = XMVectorGetX(XMVector3Length(vDistance));

		//카메라가까울경우 최소거리
		if (fLength <= 7.0f)
		{
			_vector vDir = XMVector3Normalize(vDistance);
			vCameraPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION) - (vDir * 7.0f);
		}

		vCameraPos = XMVectorSetY(vCameraPos, XMVectorGetY(m_pTransform->Get_State(CTransform::STATE_POSITION)));

		_vector vCurPosition = m_pTransform->Get_State(CTransform::STATE_POSITION);
		_vector vPosition = XMVectorLerp(vCurPosition, vCameraPos, (_float)TimeDelta * 4.0f);

		//쿼터니온 테스트
		//_vector q1 = XMQuaternionRotationMatrix(XMLoadFloat4x4(&m_pSocketTransform->Get_WorldMatrix()));
		//_vector q2 = XMQuaternionRotationMatrix(XMLoadFloat4x4(&pPlayerTransform->Get_WorldMatrix()));
		//_vector vPosition = XMQuaternionSlerp(q1, q2, (_float)TimeDelta * 3.0f);
		//테스트
		
		_float4 vLockOnTargetPos;
		CBoss* pBossCheck = dynamic_cast<CBoss*>(static_cast<CCharacter*>(m_pTarget)->GetLockOnTarget());
		if (pBossCheck)
		{
			//보스일경우
			vLockOnTargetPos = ((CEnemy*)(static_cast<CCharacter*>(m_pTarget)->GetLockOnTarget()))->GetPosition();
			vLockOnTargetPos.y += 2.f;
		}
		else
		{
			//일반몹
			vLockOnTargetPos = ((CEnemy*)(static_cast<CCharacter*>(m_pTarget)->GetLockOnTarget()))->GetPosition();
			vLockOnTargetPos.y += 1.f;
		}

		m_pTransform->Set_State(CTransform::STATE_POSITION, vPosition);

		_vector vCurLook = XMVectorLerp(XMLoadFloat4(&vLookTarget), XMLoadFloat4(&vLockOnTargetPos), (_float)TimeDelta * 2.f);
		XMStoreFloat4(&vLookTarget, vCurLook);

		m_pTransform->LookAt(XMLoadFloat4(&vLookTarget));
	}
	else
	{
		_vector vCurPosition = m_pTransform->Get_State(CTransform::STATE_POSITION);
		_vector vPosition = XMVectorLerp(vCurPosition, m_pSocketTransform->Get_State(CTransform::STATE_POSITION), (_float)TimeDelta * 4.0f);
		//_vector vPosition = XMQuaternionSlerp(vCurPosition, m_pSocketTransform->Get_State(CTransform::STATE_POSITION), (_float)TimeDelta * 3.0f);

		m_pTransform->Set_State(CTransform::STATE_POSITION, vPosition);

		_vector vCurLook = XMVectorLerp(XMLoadFloat4(&vLookTarget), vTargetPos, (_float)TimeDelta * 2.f);
		XMStoreFloat4(&vLookTarget, vCurLook);
		m_pTransform->LookAt(vCurLook);
	}
	m_CameraDesc.vAt = _float3{ vLookTarget.x, vLookTarget.y, vLookTarget.z };

	pGameInstance->AddCollider(m_pCollider);
	m_pCollider->Update(pGameInstance->Get_Transform_Matrix_Inverse(CPipeLine::TS_VIEW));
}

void CPlayerCamera::WinActionMovement(_double TimeDelta)
{
	if (m_isGoal)
		return;

	m_fWinActionAcc += TimeDelta * 1.5f;

	if (m_fWinActionAcc >= 6.f)
	{
		m_isGoal = true;
		m_bFadeIn = true;
		m_bFinish = true;
		//m_bWinAction = false;
		m_fWinActionAcc = 0.f;
	}

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	_vector vTargetPos = m_pTargetTransform->Get_State(CTransform::STATE_POSITION);
	_vector vTargetLook = XMVector3Normalize(m_pTargetTransform->Get_State(CTransform::STATE_LOOK));

	vTargetPos = XMVectorSetY(vTargetPos, 1.5f);
	_vector vCurCamPos = vTargetPos + vTargetLook * 1.2f;

	vCurCamPos = XMVectorSetY(vCurCamPos, XMVectorGetY(vCurCamPos) + 0.05f * cos(m_fWinActionAcc));

	m_pTransform->Set_State(CTransform::STATE_POSITION, vCurCamPos);
	m_pTransform->LookAt(vTargetPos);

}

CPlayerCamera * CPlayerCamera::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPlayerCamera*	pInstance = new CPlayerCamera(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPlayerCamera");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPlayerCamera::Clone(void * pArg)
{
	CPlayerCamera*	pInstance = new CPlayerCamera(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPlayerCamera");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerCamera::Free()
{
	__super::Free();

	Safe_Release(m_pTransform);

	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pTexture);
}
