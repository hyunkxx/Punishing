#include "pch.h"
#include "..\Public\FloorCircle.h"

#include "GameInstance.h"

CFloorCircle::CFloorCircle(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CFloorCircle::CFloorCircle(const CFloorCircle & rhs)
	: CGameObject(rhs)
{
}

HRESULT CFloorCircle::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CFloorCircle::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (pArg)
	{
		m_pOwnerTransform = (CTransform*)pArg;
	}

	return S_OK;
}

void CFloorCircle::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	_vector vOwnerPos = m_pOwnerTransform->Get_State(CTransform::STATE_POSITION);
	vOwnerPos = XMVectorSetY(vOwnerPos, 0.08f);
	m_pTransform->Set_State(CTransform::STATE_POSITION, vOwnerPos);

	m_pTransform->Rotate(VECTOR_UP, TimeDelta);
}

void CFloorCircle::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	if (nullptr != m_pRenderer)
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);
}

HRESULT CFloorCircle::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(Setup_ShaderResources()))
		return E_FAIL;

	switch (m_eCircleType)
	{
	case CIRCLE_PLAYER:
	{
		_uint MeshCount = m_pPlayerCircle->Get_MeshCount();
		for (_uint i = 0; i < MeshCount; ++i)
		{
			m_pPlayerCircle->Setup_ShaderMaterialResource(m_pShader, "g_DiffuseTexture", i, aiTextureType::aiTextureType_DIFFUSE);
			m_pShader->Begin(8);
			m_pPlayerCircle->Render(i);
		}
		break;
	}
	case CIRCLE_ENEMY:
	{
		_uint MeshCount = m_pEnemyCircle->Get_MeshCount();
		for (_uint i = 0; i < MeshCount; ++i)
		{
			m_pEnemyCircle->Setup_ShaderMaterialResource(m_pShader, "g_DiffuseTexture", i, aiTextureType::aiTextureType_DIFFUSE);
			m_pShader->Begin(8);
			m_pEnemyCircle->Render(i);
		}
		break;
	}
	}


	return S_OK;

}

void CFloorCircle::RenderGUI()
{
}

void CFloorCircle::SetScale(_float3 vScale)
{
	m_pTransform->Set_Scale(vScale);
}

HRESULT CFloorCircle::Add_Components()
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

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("proto_com_model_circle1"), TEXT("com_model1"), (CComponent**)&m_pPlayerCircle)))
		return E_FAIL;
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("proto_com_model_circle1"), TEXT("com_model2"), (CComponent**)&m_pEnemyCircle)))
		return E_FAIL;

	return S_OK;
}

HRESULT CFloorCircle::Setup_ShaderResources()
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

CFloorCircle* CFloorCircle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CFloorCircle* pInstance = new CFloorCircle(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CFloorCircle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CFloorCircle::Clone(void* pArg)
{
	CFloorCircle* pInstance = new CFloorCircle(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CFloorCircle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFloorCircle::Free()
{
	__super::Free();

	Safe_Release(m_pRenderer);
	Safe_Release(m_pTransform);
	Safe_Release(m_pPlayerCircle);
	Safe_Release(m_pEnemyCircle);
	Safe_Release(m_pShader);
}
