#include "pch.h"
#include "..\Public\ToolCamera.h"

#include "GameInstance.h"

CToolCamera::CToolCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera(pDevice, pContext)
{
}

CToolCamera::CToolCamera(const CToolCamera& rhs)
	: CCamera(rhs)
{
}

HRESULT CToolCamera::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CToolCamera::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CToolCamera::Tick(_double TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (pGameInstance->Input_KeyState_Custom(DIK_W) == KEY_STATE::HOLD)
		m_pTransform->MoveForward(TimeDelta);

	if (pGameInstance->Input_KeyState_Custom(DIK_S) == KEY_STATE::HOLD)
		m_pTransform->MoveBackward(TimeDelta);

	if (pGameInstance->Input_KeyState_Custom(DIK_A) == KEY_STATE::HOLD)
		m_pTransform->MoveLeft(TimeDelta);

	if (pGameInstance->Input_KeyState_Custom(DIK_D) == KEY_STATE::HOLD)
		m_pTransform->MoveRight(TimeDelta);

	if (pGameInstance->Input_MouseState(DIMK_RB) == KEY_STATE::HOLD)
	{
		m_isRotationLock = false;
	}
	else
	{
		m_isRotationLock = true;
	}

	if (m_isRotationLock)
	{
		_long MouseMove = 0;

		if (MouseMove = pGameInstance->Input_MouseMove(DIMM_X))
			m_pTransform->Rotate(XMVectorSet(0.f, 1.f, 0.f, 0.f), MouseMove * TimeDelta * 0.1f);

		if (MouseMove = pGameInstance->Input_MouseMove(DIMM_Y))
			m_pTransform->Rotate(m_pTransform->Get_State(CTransform::STATE_RIGHT), MouseMove * TimeDelta * 0.1f);
	}

	__super::Tick(TimeDelta);
}

void CToolCamera::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);
}

HRESULT CToolCamera::Render()
{
	return S_OK;
}

HRESULT CToolCamera::Add_Components()
{
	return S_OK;
}

HRESULT CToolCamera::Setup_ShaderResources()
{
	return S_OK;
}

CToolCamera* CToolCamera::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CToolCamera*	pInstance = new CToolCamera(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CToolCamera");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CToolCamera::Clone(void* pArg)
{
	CToolCamera* pInstance = new CToolCamera(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CToolCamera");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CToolCamera::Free()
{
	__super::Free();
}
