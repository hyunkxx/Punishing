#include "pch.h"
#include "..\Public\City.h"

#include "GameInstance.h"
#include "ApplicationManager.h"

CCity::CCity(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

HRESULT CCity::Initialize_Prototype()
{
	if (FAILED(CGameObject::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCity::Initialize(void * pArg)
{
	if (FAILED(CGameObject::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

void CCity::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	CApplicationManager* pAppManager = CApplicationManager::GetInstance();

	if (pAppManager->IsFreeze())
	{
		m_bDarkness = true;
		m_fTimeAcc -= TimeDelta;
		if (m_fTimeAcc <= 0.2f)
			m_fTimeAcc = 0.2f;
	}
	else
	{
		m_bDarkness = false;
		m_fTimeAcc += TimeDelta * 2.f;
		if (m_fTimeAcc >= 1.f)
			m_fTimeAcc = 1.f;
	}
}

void CCity::LateTick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	if (nullptr != m_pRenderer)
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);

}

HRESULT CCity::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(Setup_ShaderResources()))
		return E_FAIL;

	m_pShader->SetRawValue("g_LightPower", &m_fTimeAcc, sizeof(float));

	_uint iMeshCount = m_pModel->Get_MeshCount();
	for (_uint i = 0; i < iMeshCount; ++i)
	{
		m_pModel->Setup_ShaderMaterialResource(m_pShader, "g_DiffuseTexture", i, aiTextureType_DIFFUSE);

		m_pShader->Begin(4);
		m_pModel->Render(i);
	}

	return S_OK;
}

HRESULT CCity::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("proto_com_renderer"),
		TEXT("com_renderer"), (CComponent**)&m_pRenderer)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("proto_com_transform"),
		TEXT("com_transform"), (CComponent**)&m_pTransform)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("proto_com_model_city"),
		TEXT("com_model"), (CComponent**)&m_pModel)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("proto_com_shader_vtxmodel"),
		TEXT("com_shader"), (CComponent**)&m_pShader)))
		return E_FAIL;
	return S_OK;
}

HRESULT CCity::Setup_ShaderResources()
{
	if (nullptr == m_pShader)
		return E_FAIL;

	if (FAILED(m_pTransform->Setup_ShaderResource(m_pShader, "g_WorldMatrix")))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (FAILED(m_pShader->SetMatrix("g_ViewMatrix", &pGameInstance->Get_Transform_float4x4(CPipeLine::TS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShader->SetMatrix("g_ProjMatrix", &pGameInstance->Get_Transform_float4x4(CPipeLine::TS_PROJ))))
		return E_FAIL;

	return S_OK;
}

CCity* CCity::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCity* pInstance = new CCity(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CCity");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCity::Clone(void * pArg)
{
	CCity* pInstance = new CCity(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CCity");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCity::Free()
{
	__super::Free();

	Safe_Release(m_pRenderer);
	Safe_Release(m_pTransform);
	Safe_Release(m_pShader);
	Safe_Release(m_pModel);
}
