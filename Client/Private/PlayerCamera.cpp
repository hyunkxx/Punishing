#include "pch.h"
#include "..\Public\PlayerCamera.h"
#include "GameInstance.h"
#include "Character.h"
#include "Transform.h"
#include "Model.h"
#include "Enemy.h"
#include "Bone.h"

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
	m_CameraDesc.fFar = 1000.f;

	m_pTransform->Set_State(CTransform::STATE_POSITION, m_pTargetTransform->Get_State(CTransform::STATE_POSITION));
	XMStoreFloat4(&vLookTarget, m_pTransform->Get_State(CTransform::STATE_POSITION));
	
	CCollider::COLLIDER_DESC collDesc;
	collDesc.owner = this;
	collDesc.vCenter = _float3(0.f, 0.f, 0.f);
	collDesc.vExtents = _float3(2.f, 2.f, 2.f);
	collDesc.vRotation = _float3(0.f, 0.f, 0.f);

	(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("proto_com_sphere_collider"), TEXT("com_collider"), (CComponent**)&m_pCollider, &collDesc));

	return S_OK;
}

void CPlayerCamera::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

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

		_float4 vLockOnTargetPos = ((CEnemy*)(static_cast<CCharacter*>(m_pTarget)->GetLockOnTarget()))->GetPosition();
		vLockOnTargetPos.y += 1.f;

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
}

void CPlayerCamera::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();


}

HRESULT CPlayerCamera::Render()
{
	return S_OK;
}

void CPlayerCamera::AttackShake()
{
	ShakeReset();
	StartShake(4.f, 2.5f);
}

HRESULT CPlayerCamera::Add_Components()
{
	return S_OK;
}

HRESULT CPlayerCamera::Setup_ShaderResources()
{
	return S_OK;
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
}
