#include "pch.h"
#include "..\Public\BuffHandler.h"

#include "GameInstance.h"

#include "Buff.h"

CBuffHandler::CBuffHandler(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CBuffHandler::CBuffHandler(const CBuffHandler & rhs)
	: CGameObject(rhs)
{
}

HRESULT CBuffHandler::Initialize_Prototype()
{
	if (FAILED(CGameObject::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBuffHandler::Initialize(void * pArg)
{
	if(FAILED(CGameObject::Initialize(pArg)))
		return E_FAIL;

	m_pPlayerTransform = (CTransform*)pArg;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (nullptr == (m_pBuff[0] = (CBuff*)pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, L"proto_obj_buff", L"layer_effect", L"buff0")))
		return E_FAIL;
	if (nullptr == (m_pBuff[1] = (CBuff*)pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, L"proto_obj_buff", L"layer_effect", L"buff1")))
		return E_FAIL;
	if (nullptr == (m_pBuff[2] = (CBuff*)pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, L"proto_obj_buff", L"layer_effect", L"buff2")))
		return E_FAIL;
	if (nullptr == (m_pBuff[3] = (CBuff*)pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, L"proto_obj_buff", L"layer_effect", L"buff3")))
		return E_FAIL;
	if (nullptr == (m_pBuff[4] = (CBuff*)pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, L"proto_obj_buff", L"layer_effect", L"buff4")))
		return E_FAIL;
	if (nullptr == (m_pBuff[5] = (CBuff*)pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, L"proto_obj_buff", L"layer_effect", L"buff5")))
		return E_FAIL;
	if (nullptr == (m_pBuff[6] = (CBuff*)pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, L"proto_obj_buff", L"layer_effect", L"buff6")))
		return E_FAIL;
	if (nullptr == (m_pBuff[7] = (CBuff*)pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, L"proto_obj_buff", L"layer_effect", L"buff7")))
		return E_FAIL;
	if (nullptr == (m_pBuff[8] = (CBuff*)pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, L"proto_obj_buff", L"layer_effect", L"buff8")))
		return E_FAIL;

	for (int i = 0; i < 9; ++i)
		m_pBuff[i]->SetPlayerTransform(m_pPlayerTransform);

	return S_OK;
}

void CBuffHandler::Tick(_double TimeDelta)
{

}

void CBuffHandler::LateTick(_double TimeDelta)
{
	m_fTimer[0] += TimeDelta;
	if (m_fTimer[0] >= 3.f)
	{
		m_fTimer[0] = 0.f;
		_vector vPos = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);
		_float4 vLeft;
		XMStoreFloat4(&vLeft, -m_pPlayerTransform->Get_State(CTransform::STATE_RIGHT) * 0.2f);

		_float4 vDir;
		XMStoreFloat4(&vDir, XMLoadFloat4(&vLeft) + m_pPlayerTransform->Get_State(CTransform::STATE_RIGHT) * (0 * 0.2f));
		m_pBuff[0]->StartEffect(vPos, 1.4f, vDir);
	}


	m_fTimer[1] += TimeDelta;
	if (m_fTimer[1] >= 2.f)
	{
		m_fTimer[1] = 0.f;
		_vector vPos = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);
		_float4 vLeft;
		XMStoreFloat4(&vLeft, -m_pPlayerTransform->Get_State(CTransform::STATE_RIGHT));

		_float4 vDir;
		XMStoreFloat4(&vDir, XMLoadFloat4(&vLeft) + m_pPlayerTransform->Get_State(CTransform::STATE_RIGHT) * (1 * 0.2f));
		m_pBuff[1]->StartEffect(vPos, 1.0f, vDir);
	}

	m_fTimer[2] += TimeDelta;
	if (m_fTimer[2] >= 4.f)
	{
		m_fTimer[2] = 0.f;
		_vector vPos = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);
		_float4 vLeft;
		XMStoreFloat4(&vLeft, -m_pPlayerTransform->Get_State(CTransform::STATE_RIGHT));

		_float4 vDir;
		XMStoreFloat4(&vDir, XMLoadFloat4(&vLeft) + m_pPlayerTransform->Get_State(CTransform::STATE_RIGHT) * (2 * 0.2f));
		m_pBuff[2]->StartEffect(vPos, 1.3f, vDir);
	}

	m_fTimer[3] += TimeDelta;
	if (m_fTimer[3] >= 2.5f)
	{
		m_fTimer[3] = 0.f;
		_vector vPos = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);
		_float4 vLeft;
		XMStoreFloat4(&vLeft, -m_pPlayerTransform->Get_State(CTransform::STATE_RIGHT));

		_float4 vDir;
		XMStoreFloat4(&vDir, XMLoadFloat4(&vLeft) + m_pPlayerTransform->Get_State(CTransform::STATE_RIGHT) * (3 * 0.2f));
		m_pBuff[3]->StartEffect(vPos, 1.0f, vDir);
	}

	m_fTimer[4] += TimeDelta;
	if (m_fTimer[4] >= 2.2f)
	{
		m_fTimer[4] = 0.f;
		_vector vPos = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);
		_float4 vLeft;
		XMStoreFloat4(&vLeft, -m_pPlayerTransform->Get_State(CTransform::STATE_RIGHT));

		_float4 vDir;
		XMStoreFloat4(&vDir, XMLoadFloat4(&vLeft) + m_pPlayerTransform->Get_State(CTransform::STATE_RIGHT) * (4 * 0.2f));
		m_pBuff[4]->StartEffect(vPos, 1.5f, vDir);
	}

	m_fTimer[5] += TimeDelta;
	if (m_fTimer[5] >= 4.2f)
	{
		m_fTimer[5] = 0.f;
		_vector vPos = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);
		_float4 vLeft;
		XMStoreFloat4(&vLeft, -m_pPlayerTransform->Get_State(CTransform::STATE_RIGHT));

		_float4 vDir;
		XMStoreFloat4(&vDir, XMLoadFloat4(&vLeft) + m_pPlayerTransform->Get_State(CTransform::STATE_RIGHT) * (5 * 0.2f));
		m_pBuff[5]->StartEffect(vPos, 1.3f, vDir);
	}


	m_fTimer[6] += TimeDelta;
	if (m_fTimer[6] >= 3.2f)
	{
		m_fTimer[6] = 0.f;
		_vector vPos = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);
		_float4 vLeft;
		XMStoreFloat4(&vLeft, -m_pPlayerTransform->Get_State(CTransform::STATE_RIGHT));

		_float4 vDir;
		XMStoreFloat4(&vDir, XMLoadFloat4(&vLeft) + m_pPlayerTransform->Get_State(CTransform::STATE_RIGHT) * (6 * 0.2f));
		m_pBuff[6]->StartEffect(vPos, 1.7f, vDir);
	}


	m_fTimer[7] += TimeDelta;
	if (m_fTimer[7] >= 2.2f)
	{
		m_fTimer[7] = 0.f;
		_vector vPos = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);
		_float4 vLeft;
		XMStoreFloat4(&vLeft, -m_pPlayerTransform->Get_State(CTransform::STATE_RIGHT));

		_float4 vDir;
		XMStoreFloat4(&vDir, XMLoadFloat4(&vLeft) + m_pPlayerTransform->Get_State(CTransform::STATE_RIGHT) * (7 * 0.2f));
		m_pBuff[7]->StartEffect(vPos, 1.3f, vDir);
	}

	m_fTimer[8] += TimeDelta;
	if (m_fTimer[8] >= 2.9f)
	{
		m_fTimer[8] = 0.f;
		_vector vPos = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);
		_float4 vLeft;
		XMStoreFloat4(&vLeft, -m_pPlayerTransform->Get_State(CTransform::STATE_RIGHT));

		_float4 vDir;
		XMStoreFloat4(&vDir, XMLoadFloat4(&vLeft) + m_pPlayerTransform->Get_State(CTransform::STATE_RIGHT) * (8 * 0.2f));
		m_pBuff[8]->StartEffect(vPos, 1.4f, vDir);
	}
}

HRESULT CBuffHandler::Render()
{
	return S_OK;
}

void CBuffHandler::RenderGUI()
{
}

void CBuffHandler::StartEffect(_fvector vPosition, _float fSpeed)
{
}

void CBuffHandler::SetPosition(_fvector vPosition)
{
}

CBuffHandler * CBuffHandler::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBuffHandler*pInstance = new CBuffHandler(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CBuffHandler");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CBuffHandler::Clone(void * pArg)
{
	CBuffHandler* pInstance = new CBuffHandler(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CBuffHandler");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBuffHandler::Free()
{
	__super::Free();

	for(int i = 0 ; i < 8; ++i)
		Safe_Release(m_pBuff[i]);
}
