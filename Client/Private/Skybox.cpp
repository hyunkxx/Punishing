#include "pch.h"
#include "..\Public\Skybox.h"

#include "GameInstance.h"

CSkybox::CSkybox(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

HRESULT CSkybox::Initialize_Prototype()
{
	if (FAILED(CGameObject::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSkybox::Initialize(void * pArg)
{
	if (FAILED(CGameObject::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

void CSkybox::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
}

void CSkybox::LateTick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	m_pTransform->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&pGameInstance->Get_CamPosition()));

	if (nullptr != m_pRenderer)
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_PRIORITY, this);
}

HRESULT CSkybox::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(Setup_ShaderResources()))
		return E_FAIL;

	_uint iMeshCount = m_pModel->Get_MeshCount();
	for (_uint i = 0; i < iMeshCount; ++i)
	{
		m_pModel->Setup_ShaderMaterialResource(m_pShader, "g_DiffuseTexture", i, aiTextureType_DIFFUSE);

		m_pShader->Begin(5);

		m_pModel->Render(i);
	}

	return S_OK;
}

HRESULT CSkybox::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("proto_com_renderer"),
		TEXT("com_renderer"), (CComponent**)&m_pRenderer)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("proto_com_transform"),
		TEXT("com_transform"), (CComponent**)&m_pTransform)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("proto_com_model_sky"),
		TEXT("com_model"), (CComponent**)&m_pModel)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("proto_com_shader_vtxmodel"),
		TEXT("com_shader"), (CComponent**)&m_pShader)))
		return E_FAIL;
	return S_OK;
}

HRESULT CSkybox::Setup_ShaderResources()
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

CSkybox* CSkybox::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CSkybox* pInstance = new CSkybox(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CSkybox");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSkybox::Clone(void * pArg)
{
	CSkybox* pInstance = new CSkybox(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CSkybox");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSkybox::Free()
{
	__super::Free();

	Safe_Release(m_pRenderer);
	Safe_Release(m_pTransform);
	Safe_Release(m_pShader);
	Safe_Release(m_pModel);
}
