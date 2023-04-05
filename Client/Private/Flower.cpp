#include "pch.h"
#include "..\Public\Flower.h"

#include "GameInstance.h"
#include "Character.h"

CFlower::CFlower(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CFlower::CFlower(const CFlower & rhs)
	: CGameObject(rhs)
{
}

HRESULT CFlower::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CFlower::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (pArg)
	{
		CCharacter* pPlayer = (CCharacter*)pArg;
		m_pPlayerTransform = static_cast<CTransform*>(pPlayer->Find_Component(L"com_transform"));
	}

	_vector vPos = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	vPos = XMVectorSetY(vPos, 30.f);
	m_pTransform->Set_State(CTransform::STATE_POSITION, vPos);
	m_pTransform->Set_Scale(_float3(1.f, 1.f, 1.f));
	return S_OK;
}

void CFlower::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	_vector vPos = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	vPos = XMVectorSetY(vPos, 30.f);
	m_pTransform->Set_State(CTransform::STATE_POSITION, vPos);

	m_pVIBuffer->Update(TimeDelta);
}

void CFlower::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	if (nullptr != m_pRenderer)
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);
}

HRESULT CFlower::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShader->Begin(1);
	m_pVIBuffer->Render();

	return S_OK;
}

HRESULT CFlower::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("proto_com_renderer"),
		TEXT("renderer"), (CComponent**)&m_pRenderer)))
		return E_FAIL;

	CTransform::TRANSFORM_DESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORM_DESC));
	TransformDesc.fMoveSpeed = 5.f;
	TransformDesc.fRotationSpeed = XMConvertToRadians(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("proto_com_transform"),
		TEXT("transform_main"), (CComponent**)&m_pTransform, &TransformDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("proto_com_vibuffer_instance_rect"),
		TEXT("vibuffer_instance"), (CComponent**)&m_pVIBuffer)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("proto_com_shader_vtxinstance_rect"),
		TEXT("Com_Shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("proto_com_texture_light"),
		TEXT("texture"), (CComponent**)&m_pTexture)))
		return E_FAIL;

	return S_OK;
}

HRESULT CFlower::SetUp_ShaderResources()
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

	if (FAILED(m_pTexture->Setup_ShaderResource(m_pShader, "g_DiffuseTexture")))
		return E_FAIL;

	if (FAILED(m_pShader->SetRawValue("g_vCamPosition", (_float3*)&pGameInstance->Get_CamPosition(), sizeof(_float3))))
		return E_FAIL;

	return S_OK;
}

CFlower * CFlower::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CFlower* pInstance = new CFlower(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CFlower");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CFlower::Clone(void * pArg)
{
	CFlower* pInstance = new CFlower(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CFlower");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFlower::Free()
{
	__super::Free();

	Safe_Release(m_pTransform);
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pShader);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pTexture);
}
