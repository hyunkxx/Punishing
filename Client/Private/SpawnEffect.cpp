#include "pch.h"
#include "..\Public\SpawnEffect.h"

#include "GameInstance.h"
#include "Character.h"
#include "Bone.h"

CSpawnEffect::CSpawnEffect(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

HRESULT CSpawnEffect::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSpawnEffect::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(AddComponents()))
		return E_FAIL;
	
	return S_OK;
}

void CSpawnEffect::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
}

void CSpawnEffect::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);
	
	if (!m_bRender)
		return;

	m_fTimeAcc += TimeDelta * 1.f;
	if (m_fTimeAcc >= 1.f)
		m_bRender = false;

	if (nullptr != m_pRenderer && m_bRender)
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CSpawnEffect::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetupShaderResources()))
		return E_FAIL;

	_uint MeshCount = m_pModel->Get_MeshCount();
	for (_uint i = 0; i < MeshCount; ++i)
	{
		m_pTexture0->Setup_ShaderResource(m_pShader, "g_DiffuseTexture");
		m_pTexture1->Setup_ShaderResource(m_pShader, "g_MaskTexture");
		m_pShader->SetRawValue("g_fTimeAcc", &m_fTimeAcc, sizeof(float));

		m_pShader->Begin(0);
		m_pModel->Render(i);
	}

	return S_OK;
}

void CSpawnEffect::RenderGUI()
{
}

_float CSpawnEffect::GetLengthFromCamera()
{
	CPipeLine* pPipeLine = CPipeLine::GetInstance();

	_vector vPos = m_pTransform->Get_State(CTransform::STATE_POSITION);
	_vector vCamPos = XMLoadFloat4(&pPipeLine->Get_CamPosition());

	_float fLength = XMVectorGetX(XMVector3Length(vPos - vCamPos));

	return fLength;
}

void CSpawnEffect::SetScale(_float3 vScale)
{
	m_pTransform->Set_Scale(vScale);
}

_float3 CSpawnEffect::GetScale() const
{
	return m_pTransform->Get_Scale();
}

void CSpawnEffect::SetTransfrom(_fvector vPos)
{
	m_pTransform->Set_State(CTransform::STATE_POSITION, vPos);
}

HRESULT CSpawnEffect::AddComponents()
{
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("proto_com_renderer"), TEXT("com_renderer"), (CComponent**)&m_pRenderer)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("proto_com_transform"), TEXT("com_transform"), (CComponent**)&m_pTransform)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("proto_com_shader_spawneffect"), TEXT("com_shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("proto_com_model_spawneffect"), TEXT("com_model"), (CComponent**)&m_pModel)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("proto_com_texture_spawneffect0"), TEXT("com_texture0"), (CComponent**)&m_pTexture0)))
		return E_FAIL;
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("proto_com_texture_spawneffect1"), TEXT("com_texture1"), (CComponent**)&m_pTexture1)))
		return E_FAIL;


	return S_OK;
}

HRESULT CSpawnEffect::SetupShaderResources()
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

CSpawnEffect * CSpawnEffect::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CSpawnEffect*	pInstance = new CSpawnEffect(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CSpawnEffect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CSpawnEffect::Clone(void * pArg)
{
	CSpawnEffect* pInstance = new CSpawnEffect(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CSpawnEffect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSpawnEffect::Free()
{
	__super::Free();

	Safe_Release(m_pRenderer);
	Safe_Release(m_pTransform);
	Safe_Release(m_pModel);
	Safe_Release(m_pShader);

	Safe_Release(m_pTexture0);
	Safe_Release(m_pTexture1);

}
