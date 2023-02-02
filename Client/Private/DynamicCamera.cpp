#include "pch.h"
#include "..\Public\DynamicCamera.h"

#include "GameInstance.h"

CDynamicCamera::CDynamicCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera(pDevice, pContext)
{
}

CDynamicCamera::CDynamicCamera(const CDynamicCamera& rhs)
	: CCamera(rhs)
{
}

HRESULT CDynamicCamera::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CDynamicCamera::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CDynamicCamera::Tick(_double TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	m_CameraDesc;
	if (pGameInstance->Input_KeyState_Custom(DIK_W) == KEY_STATE::HOLD)
		m_pTransform->MoveForward(TimeDelta);

	if (pGameInstance->Input_KeyState_Custom(DIK_S) == KEY_STATE::HOLD)
		m_pTransform->MoveBackward(TimeDelta);

	if (pGameInstance->Input_KeyState_Custom(DIK_A) == KEY_STATE::HOLD)
		m_pTransform->MoveLeft(TimeDelta);

	if (pGameInstance->Input_KeyState_Custom(DIK_D) == KEY_STATE::HOLD)
		m_pTransform->MoveRight(TimeDelta);

	_long MouseMove = 0;
	
	if (MouseMove = pGameInstance->Input_MouseMove(DIMM_X))
		m_pTransform->Rotate(XMVectorSet(0.f,1.f,0.f,0.f), MouseMove * TimeDelta * 0.1f);

	if (MouseMove = pGameInstance->Input_MouseMove(DIMM_Y))
		m_pTransform->Rotate(m_pTransform->Get_State(CTransform::STATE_RIGHT), MouseMove * TimeDelta * 0.1f);

	__super::Tick(TimeDelta);
}

void CDynamicCamera::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);
}

HRESULT CDynamicCamera::Render()
{
	return S_OK;
}

HRESULT CDynamicCamera::Add_Components()
{
	return S_OK;
}

HRESULT CDynamicCamera::Setup_ShaderResources()
{
	return S_OK;
}

CDynamicCamera* CDynamicCamera::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDynamicCamera*	pInstance = new CDynamicCamera(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CDynamicCamera");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDynamicCamera::Clone(void* pArg)
{
	CDynamicCamera*	pInstance = new CDynamicCamera(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CDynamicCamera");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDynamicCamera::Free()
{
	__super::Free();
}
