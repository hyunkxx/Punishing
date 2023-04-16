#include "pch.h"
#include "..\Public\FootSmoke.h"

#include "GameInstance.h"
#include "Enemy.h"
#include "Character.h"

CFootSmoke::CFootSmoke(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CFootSmoke::CFootSmoke(const CFootSmoke & rhs)
	: CGameObject(rhs)
{
}

HRESULT CFootSmoke::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CFootSmoke::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (pArg)
	{
		m_pOwner = static_cast<CGameObject*>(pArg);
		m_pOwnerTransform = static_cast<CTransform*>(m_pOwner->Find_Component(L"com_transform"));
	}

	m_pTransform->Set_Scale(_float3(m_vSaveScale, 1.f, m_vSaveScale));

	return S_OK;
}

void CFootSmoke::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	if (m_bActive)
	{
		m_vCurScale += TimeDelta * 2.f;
		m_pTransform->Set_Scale(_float3(m_vCurScale, 1.f, m_vCurScale));

		m_fTimeAcc += TimeDelta * 1.f;
		if (m_fTimeAcc >= 1.f)
		{
			m_fTimeAcc = 0.f;
			m_bActive = false;
			m_pTransform->Set_Scale(_float3(m_vSaveScale, 1.f, m_vSaveScale));
		}
	}

}

void CFootSmoke::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	if (nullptr != m_pRenderer && m_bActive)
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CFootSmoke::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(Setup_ShaderResources()))
		return E_FAIL;

	m_pDiffuseTexture->Setup_ShaderResource(m_pShader, "g_DiffuseTexture");
	m_pShader->SetRawValue("g_fTimeAcc", &m_fTimeAcc, sizeof(_float));

	switch (m_eFootType)
	{
	case FOOT::PLAYER:
		m_pShader->Begin(16);
		m_pModel->Render(0);
		break;
	case FOOT::ENEMY:
		m_pShader->Begin(17);
		m_pModel->Render(0);
	break;
	}


	return S_OK;
}

void CFootSmoke::RenderGUI()
{
}

void CFootSmoke::StartEffect()
{
	_vector vOwnerPos = m_pOwnerTransform->Get_State(CTransform::STATE_POSITION);
	vOwnerPos = XMVectorSetY(vOwnerPos, 0.1f);
	m_pTransform->Set_State(CTransform::STATE_POSITION, vOwnerPos);

	m_bActive = true;
	m_fTimeAcc = 0.f;
	m_vCurScale = m_vSaveScale;
}

HRESULT CFootSmoke::Add_Components()
{
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("proto_com_renderer"), TEXT("com_renderer"), (CComponent**)&m_pRenderer)))
		return E_FAIL;

	CTransform::TRANSFORM_DESC desc;
	desc.fMoveSpeed = 0.1f;
	desc.fRotationSpeed = XMConvertToRadians(20.f);

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("proto_com_transform"), TEXT("com_transform"), (CComponent**)&m_pTransform, &desc)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("proto_com_shader_vtxmodel"), TEXT("com_shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("proto_com_model_footsmoke"), TEXT("com_model"), (CComponent**)&m_pModel)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("proto_com_texture_footsmokediffuse"), TEXT("com_diffuse"), (CComponent**)&m_pDiffuseTexture)))
		return E_FAIL;

	return S_OK;
}

HRESULT CFootSmoke::Setup_ShaderResources()
{
	if (nullptr == m_pShader)
		return E_FAIL;

	CGameInstance* pInstance = CGameInstance::GetInstance();

	if (FAILED(m_pTransform->Setup_ShaderResource(m_pShader, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShader->SetMatrix("g_ViewMatrix", &pInstance->Get_Transform_float4x4(CPipeLine::TS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShader->SetMatrix("g_ProjMatrix", &pInstance->Get_Transform_float4x4(CPipeLine::TS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShader->SetRawValue("g_vCamPosition", &pInstance->Get_CamPosition(), sizeof(_float3))))
		return E_FAIL;

	const LIGHT_DESC* LightDesc = pInstance->GetLightDesc(0);
	if (nullptr == LightDesc)
		return E_FAIL;

	if (FAILED(m_pShader->SetRawValue("g_vLightDir", &LightDesc->vDirection, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_vLightDiffuse", &LightDesc->vDiffuse, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_vLightSpecular", &LightDesc->vSpecular, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_vLightAmbient", &LightDesc->vAmbient, sizeof(_float4))))
		return E_FAIL;

	return S_OK;
}

CFootSmoke* CFootSmoke::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CFootSmoke* pInstance = new CFootSmoke(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CFootSmoke");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CFootSmoke::Clone(void* pArg)
{
	CFootSmoke* pInstance = new CFootSmoke(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CFootSmoke");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFootSmoke::Free()
{
	__super::Free();

	Safe_Release(m_pRenderer);
	Safe_Release(m_pTransform);
	Safe_Release(m_pModel);
	Safe_Release(m_pShader);
}
