#include "pch.h"
#include "..\Public\FreezeArea.h"

#include "GameInstance.h"
#include "Character.h"
#include "Bone.h"

#include "ApplicationManager.h"

CFreezeArea::CFreezeArea(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

HRESULT CFreezeArea::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CFreezeArea::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(AddComponents()))
		return E_FAIL;

	if (nullptr != pArg)
	{
		m_pPlayerTransform = static_cast<CTransform*>(static_cast<CGameObject*>(pArg)->Find_Component(L"com_transform"));
		if (m_pPlayerTransform == nullptr)
			return E_FAIL;
	}
	else
		return E_FAIL;

	return S_OK;
}

void CFreezeArea::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	CApplicationManager* pAppManager = CApplicationManager::GetInstance();
	if (pAppManager->IsFreeze())
	{
		m_bActive = true;

		_vector vCurrentScale = XMLoadFloat3(&m_vScale);
		_vector vDestScale = XMVectorSet(2.f, 2.f, 2.f, 1.f);

		XMStoreFloat3(&m_vScale, XMVectorLerp(vCurrentScale, vDestScale, (_float)TimeDelta * 3.f));

		if (m_vScale.x >= 0.9f)
		{
			m_bFinish = true;
		}

		if (m_bFinish)
		{
			m_fTimeAcc += TimeDelta * 0.2f;
			//m_bColorChange = true;
		}

		m_pTransform->Set_Scale(m_vScale);
		m_pTransform->Set_State(CTransform::STATE_POSITION, pAppManager->GetPlayerPrevPosition());
	}
	else
	{
		m_bFinish = false;
		m_bColorChange = false;

		m_fTimeAcc = 0.f;

		_vector vCurrentScale = XMLoadFloat3(&m_vScale);
		_vector vDestScale = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

		_vector vCurrentPos = XMLoadFloat3(&m_vPosition);
		_vector vDestPos = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);

		XMStoreFloat3(&m_vScale, XMVectorLerp(vCurrentScale, vDestScale, (_float)TimeDelta * 12.f));
		XMStoreFloat3(&m_vPosition, XMVectorLerp(vCurrentPos, vDestPos, (_float)TimeDelta * 12.f));

		m_pTransform->Set_Scale(m_vScale);
		m_pTransform->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&m_vPosition));

		if (m_vScale.x < 0.05f)
		{
			m_vScale = { 0.f, 0.f, 0.f };
			m_bActive = false;
		}
	}


}

void CFreezeArea::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	if (m_bActive)
	{
		if (nullptr != m_pRenderer)
			m_pRenderer->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
	}
}

HRESULT CFreezeArea::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetupShaderResources()))
		return E_FAIL;

	_uint MeshCount = m_pModel->Get_MeshCount();

	CApplicationManager* pAppManager = CApplicationManager::GetInstance();

	if (m_bFinish)
	{
		//¿­·ÈÀ»¶§
		m_pModel->Setup_ShaderMaterialResource(m_pShader, "g_DiffuseTexture", 0, aiTextureType::aiTextureType_DIFFUSE);
		m_pNoiseTexture->Setup_ShaderResource(m_pShader, "g_NoiseTexture", 0);
		m_pMaskTexture->Setup_ShaderResource(m_pShader, "g_MaskTexture", 0);
		m_pMaskTexture2->Setup_ShaderResource(m_pShader, "g_MaskTexture2", 0);

		m_pShader->SetRawValue("g_TimeAcc", &m_fTimeAcc, sizeof(float));

		if (m_bColorChange)
		{
			m_pShader->Begin(4);
			m_pModel->Render(0);

		}
		else
		{
			m_pShader->Begin(3);
			m_pModel->Render(0);
		}

	}
	else
	{
		//´ÝÈú¶§?

		for (_uint i = 0; i < MeshCount; ++i)
		{
			//if (!strcmp("Cheek", m_pModel->GetMeshName(i)) || !strcmp("Cheek01", m_pModel->GetMeshName(i)))
			//	continue;

			m_pModel->Setup_ShaderMaterialResource(m_pShader, "g_DiffuseTexture", i, aiTextureType::aiTextureType_DIFFUSE);
			m_pNoiseTexture->Setup_ShaderResource(m_pShader, "g_NoiseTexture", 0);
			m_pMaskTexture->Setup_ShaderResource(m_pShader, "m_pMaskTexture2", 0);

			m_pShader->Begin(i);
			m_pModel->Render(i);
		}
	}


	return S_OK;
}

void CFreezeArea::RenderGUI()
{
}

void CFreezeArea::SetPosition(_vector vPosition)
{
	XMStoreFloat3(&m_vPosition, vPosition);
}

HRESULT CFreezeArea::AddComponents()
{
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("proto_com_renderer"), TEXT("com_renderer"), (CComponent**)&m_pRenderer)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("proto_com_transform"), TEXT("com_transform"), (CComponent**)&m_pTransform)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("proto_com_shader_freeze"), TEXT("com_shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("proto_com_texture_freezemask0"), TEXT("com_texture_noise"), (CComponent**)&m_pNoiseTexture)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("proto_com_texture_freezemask1"), TEXT("com_texture_mask"), (CComponent**)&m_pMaskTexture)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("proto_com_texture_freezemask3"), TEXT("com_texture_mask2"), (CComponent**)&m_pMaskTexture2)))
		return E_FAIL;

	if (!CApplicationManager::GetInstance()->IsLevelFinish(CApplicationManager::LEVEL::GAMEPLAY))
	{
		if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("proto_com_model_freeze_area"), TEXT("com_model"), (CComponent**)&m_pModel)))
			return E_FAIL;
	}
	else
	{
		if (FAILED(CGameObject::Add_Component(LEVEL_BOSS, TEXT("proto_com_model_freeze_area"), TEXT("com_model"), (CComponent**)&m_pModel)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CFreezeArea::SetupShaderResources()
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

CFreezeArea * CFreezeArea::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CFreezeArea*	pInstance = new CFreezeArea(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CFreezeArea");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CFreezeArea::Clone(void * pArg)
{
	CFreezeArea* pInstance = new CFreezeArea(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CFreezeArea");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFreezeArea::Free()
{
	__super::Free();

	Safe_Release(m_pNoiseTexture);
	Safe_Release(m_pMaskTexture2);
	Safe_Release(m_pMaskTexture);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pTransform);
	Safe_Release(m_pModel);
	Safe_Release(m_pShader);

}
