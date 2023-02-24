#include "pch.h"
#include "..\Public\PlayerCamera.h"
#include "GameInstance.h"
#include "Kalienina.h"
#include "Transform.h"
#include "Model.h"
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
	m_pTargetTransform = static_cast<CTransform*>(m_pTarget->Find_Component(L"com_camera_socket_transform"));

	m_CameraDesc.fFovy = XMConvertToRadians(45.f);
	m_CameraDesc.fAspect = g_iWinSizeX / (_float)g_iWinSizeY;
	m_CameraDesc.fNear = 0.1f;
	m_CameraDesc.fFar = 1000.f;

	return S_OK;
}

void CPlayerCamera::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	assert(m_pTargetTransform);

	_vector vTargetPos = m_pTargetTransform->Get_State(CTransform::STATE_POSITION);
	_vector vTargetLook = XMVector3Normalize(m_pTargetTransform->Get_State(CTransform::STATE_LOOK));

	_vector vCameraPosition = vTargetPos - XMLoadFloat3(&vDistance);

	vCameraPosition = vTargetPos - (vTargetLook * vDistance.z);
	vCameraPosition = XMVectorSetY(vCameraPosition, vDistance.y);

	if (pGameInstance->Input_MouseState_Custom(DIMK_RB) == KEY_STATE::HOLD)
		m_bMouseLock = false;
	else
		m_bMouseLock = true;

	if (!m_bMouseLock)
	{
		_long MouseMove = 0;
		if (MouseMove = pGameInstance->Input_MouseMove(DIMM_X))
		{
			m_pTargetTransform->Rotate(VECTOR_UP, MouseMove * TimeDelta * 0.1f);
		}
	}

	//if (MouseMove = pGameInstance->Input_MouseMove(DIMM_Y))
	//	m_pTransform->Rotate(m_pTransform->Get_State(CTransform::STATE_RIGHT), MouseMove * TimeDelta * 0.1f);

	m_pTransform->Set_State(CTransform::STATE_POSITION, vCameraPosition);
	m_pTransform->LookAt(vTargetPos);
}

void CPlayerCamera::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);
}

HRESULT CPlayerCamera::Render()
{
	return S_OK;
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
