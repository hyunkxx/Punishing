#include "..\Public\Camera.h"
#include "PipeLine.h"

#include "GameInstance.h"

CCamera::CCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
	, m_pPipeLine{ CPipeLine::GetInstance() }
{
	Safe_AddRef(m_pPipeLine);
}

CCamera::CCamera(const CCamera& rhs)
	: CGameObject(rhs)
	, m_pPipeLine(rhs.m_pPipeLine)
{
	Safe_AddRef(m_pPipeLine);
}

HRESULT CCamera::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCamera::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	memcpy(&m_CameraDesc, pArg, sizeof m_CameraDesc);

	m_pTransform = CTransform::Create(m_pDevice, m_pContext);
	if (nullptr == m_pTransform)
		return E_FAIL;

	m_pTransform->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&m_CameraDesc.vEye));
	m_pTransform->LookAt(XMLoadFloat3(&m_CameraDesc.vAt));
	m_pTransform->Set_TransformDesc(m_CameraDesc.TransformDesc);

	return S_OK;
}

void CCamera::Tick(_double TimeDelta)
{
	CGameInstance* pInstance = CGameInstance::GetInstance();

	m_pPipeLine->Set_Transform(CPipeLine::TS_VIEW, m_pTransform->Get_WorldMatrixInverse());
	m_pPipeLine->Set_Transform(CPipeLine::TS_PROJ, XMMatrixPerspectiveFovLH(m_CameraDesc.fFovy, m_CameraDesc.fAspect, m_CameraDesc.fNear, m_CameraDesc.fFar));

	//if (pInstance->Input_KeyState_Custom(DIK_LSHIFT) == KEY_STATE::HOLD)
	//{
	//	m_CameraDesc.TransformDesc.fMoveSpeed = m_CameraDesc.TransformDesc.fMoveSpeed * 2.f;
	//}
	//else
	//{
	//	m_CameraDesc.TransformDesc.fMoveSpeed = m_CameraDesc.TransformDesc.fMoveSpeed = m_CameraDesc.TransformDesc.fMoveSpeed * 0.5f;
	//}

}

void CCamera::LateTick(_double TimeDelta)
{
}

HRESULT CCamera::Render()
{
	return S_OK;
}

void CCamera::Free()
{
	__super::Free();
	Safe_Release(m_pTransform);
	Safe_Release(m_pPipeLine);
}
