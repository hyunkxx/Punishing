#include "pch.h"
#include "..\Public\Cube.h"

#include "Component_Manager.h"
#include "GameInstance.h"

_uint CCube::m_iID = 0;

CCube::CCube(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CCube::CCube(const CCube& rhs)
	: CGameObject(rhs)
{
	m_iID++;
}

HRESULT CCube::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCube::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(1.f, 1.f, m_iID * 1.f, 1.f));

	return S_OK;
}

void CCube::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

}

void CCube::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	if (m_pRenderer)
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);
}

HRESULT CCube::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(Setup_ShaderResources()))
		return E_FAIL;

	m_pShader->Begin(0);
	m_pVIBuffer->Render();

	//동적리소스?ㅇㅇ
	return S_OK;
}

void CCube::RenderGUI()
{
}

HRESULT CCube::Add_Components()
{
 	if (FAILED(__super::Add_Component(TOOL_STATIC, TEXT("proto_com_renderer"),
		TEXT("com_renderer"), (CComponent**)&m_pRenderer)))
		return E_FAIL;

	CTransform::TRANSFORM_DESC TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORM_DESC));

	TransformDesc.fMoveSpeed = 5.f;
	TransformDesc.fRotationSpeed = XMConvertToRadians(90.f);

	if (FAILED(__super::Add_Component(TOOL_STATIC, TEXT("proto_com_transform"),
		TEXT("com_transform"), (CComponent**)&m_pTransform)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(TOOL_STATIC, TEXT("proto_com_vibuffer_cube"),
		TEXT("com_vibuffer"), (CComponent**)&m_pVIBuffer)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(TOOL_STATIC, TEXT("proto_com_shader_vtxcube"),
		TEXT("com_shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	return S_OK;
}

HRESULT CCube::Setup_ShaderResources()
{
	if (nullptr == m_pShader)
		return E_FAIL;

	CPipeLine* pPipeline = CPipeLine::GetInstance();

	if (FAILED(m_pTransform->Setup_ShaderResource(m_pShader, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShader->SetMatrix("g_ViewMatrix", &pPipeline->Get_Transform_float4x4(CPipeLine::TS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShader->SetMatrix("g_ProjMatrix", &pPipeline->Get_Transform_float4x4(CPipeLine::TS_PROJ))))
		return E_FAIL;

	return S_OK;
}

CCube* CCube::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCube* pInstance = new CCube(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCube");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCube::Clone(void* pArg)
{
	CCube* pInstance = new CCube(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CCube");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCube::Free()
{
	__super::Free();

	Safe_Release(m_pTransform);
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pShader);
	Safe_Release(m_pRenderer);
}
