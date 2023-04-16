#include "pch.h"
#include "..\Public\SkillBlueEffect.h"

#include "ApplicationManager.h"
#include "GameInstance.h"
#include "Character.h"
#include "Thorn.h"

CSkillBlueEffect::CSkillBlueEffect(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CSkillBlueEffect::CSkillBlueEffect(const CSkillBlueEffect & rhs)
	: CGameObject(rhs)
{
}

HRESULT CSkillBlueEffect::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSkillBlueEffect::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (pArg != nullptr)
	{
		m_pOwner = (CCharacter*)pArg;
		m_pPlayerTransform = (CTransform*)m_pOwner->Find_Component(L"com_transform");
	}

	m_pTransform->SetRotation(VECTOR_RIGHT, XMConvertToRadians(90.f));

	return S_OK;
}

void CSkillBlueEffect::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	return;
}

void CSkillBlueEffect::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	m_pTransform->Set_WorldMatrix(m_CalcMatrix);

	for (int i = 0; i < THORN_COUNT; ++i)
	{
		m_pThorn[i]->SetTimeDelta(TimeDelta);

		if (m_pThorn[i]->IsScaleFinish() && !m_bScaleDown[i])
		{
			m_bScaleDown[i] = true;
			m_pThorn[i]->StartDessolve();
		}
	}

	if (m_bRender)
	{
		m_fAlphaAcc -= TimeDelta;
		if (m_fAlphaAcc <= 0.f)
		{
			m_fAlphaAcc = 2.f;
			m_bRender = false;
		}
	}

	if (nullptr != m_pRenderer && m_bRender)
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);

	return;
}

HRESULT CSkillBlueEffect::Render()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (FAILED(Setup_ShaderResources()))
		return E_FAIL;

	m_pDiffuseTexture->Setup_ShaderResource(m_pShader, "g_Texture");
	m_pMaskTexture->Setup_ShaderResource(m_pShader, "g_MaskTexture");
	m_pShader->SetRawValue("g_TimeAcc", &m_fAlphaAcc, sizeof(float));

	m_pShader->Begin(7);
	m_pVIBuffer->Render();

	return S_OK;
}

void CSkillBlueEffect::RenderGUI()
{
}

void CSkillBlueEffect::SetupPosition()
{
	m_bRender = true;
	m_fAlphaAcc = 4.f;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	_float4x4 PlayerMatrix = m_pPlayerTransform->Get_WorldMatrix();
	_vector vEffectPos = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION) + XMVector3Normalize(m_pPlayerTransform->Get_State(CTransform::STATE_LOOK)) * 4.f;
	_vector vEffectPos2 = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION) + XMVector3Normalize(m_pPlayerTransform->Get_State(CTransform::STATE_LOOK)) * 3.f;
	_vector vEffectPos3 = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION) + XMVector3Normalize(m_pPlayerTransform->Get_State(CTransform::STATE_LOOK)) * 2.f;

	PlayerMatrix._41 = XMVectorGetX(vEffectPos);

	//y
	if (!CApplicationManager::GetInstance()->IsLevelFinish(CApplicationManager::LEVEL::GAMEPLAY))
		PlayerMatrix._42 = 0.02f;
	else
		PlayerMatrix._42 = 0.03f;

	PlayerMatrix._43 = XMVectorGetZ(vEffectPos);
	
	_matrix RotationMatrix = XMMatrixRotationX(XMConvertToRadians(90.f));

	ZeroMemory(&m_CalcMatrix, sizeof(_float4x4));
	XMStoreFloat4x4(&m_CalcMatrix, XMMatrixScaling(8.f, 8.f, 1.f) * RotationMatrix * XMLoadFloat4x4(&PlayerMatrix));
	
	_vector vLook = XMVector3Normalize(m_pPlayerTransform->Get_State(CTransform::STATE_LOOK));
	_vector vRight = XMVector3Normalize(m_pPlayerTransform->Get_State(CTransform::STATE_RIGHT));

	for (int i = 0; i < THORN_COUNT; ++i)
	{
		int iRand = rand() % 360;
		XMConvertToRadians(iRand);
		m_pThorn[i]->SetRotationXYZ(_float3(0.f, iRand, 0.f));

		m_bScaleDown[i] = false;

		if (i < 4)
		{
			if (i % 2 == 0)
			{
				vEffectPos = vEffectPos + vRight * (i * 0.9f);
				m_pThorn[i]->SetPosition(vEffectPos);
				m_pThorn[i]->SetupScaleUpStart(max(3.2f * i, 2.5f));
			}
			else
			{
				vEffectPos = vEffectPos - vRight * (i * 0.7f);
				m_pThorn[i]->SetPosition(vEffectPos);
				m_pThorn[i]->SetupScaleUpStart(max(3.1f * i, 2.5f));
			}
		}
		else if (i < 8)
		{
			if (i % 2 == 0)
			{
				vEffectPos2 = vEffectPos2 + vRight * ((i - 4) * 1.5f);
				m_pThorn[i]->SetPosition(vEffectPos2);
				m_pThorn[i]->SetupScaleUpStart(max(2.5f * (i - 4), 1.9f));
			}
			else
			{
				vEffectPos2 = vEffectPos2 - vRight * ((i - 4) * 1.5f);
				m_pThorn[i]->SetPosition(vEffectPos2);
				m_pThorn[i]->SetupScaleUpStart(max(2.5f * (i - 4), 1.9));
			}
		}
		else
		{
			if (i % 2 == 0)
			{
				vEffectPos3 = vEffectPos3 + vRight * ((i - 8) * 1.1f);
				m_pThorn[i]->SetPosition(vEffectPos3);
				m_pThorn[i]->SetupScaleUpStart(max(2.1f * (i - 6), 2.1f));
			}
			else
			{
				vEffectPos3 = vEffectPos3 - vRight * ((i - 8) * 0.8f);
				m_pThorn[i]->SetPosition(vEffectPos3);
				m_pThorn[i]->SetupScaleUpStart(max(2.1f * (i - 6), 2.1f));
			}

		}
	}
}

HRESULT CSkillBlueEffect::Add_Components()
{
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("proto_com_renderer"), TEXT("com_renderer"), (CComponent**)&m_pRenderer)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("proto_com_transform"), TEXT("com_transform"), (CComponent**)&m_pTransform)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("proto_com_shader_vtxtex"), TEXT("com_shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("proto_com_texture_skillfloor"), TEXT("com_diffuse"), (CComponent**)&m_pDiffuseTexture)))
		return E_FAIL;
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("proto_com_texture_skillfloormask"), TEXT("com_mask"), (CComponent**)&m_pMaskTexture)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("proto_com_vibuffer_rect"), TEXT("com_vibuffer"), (CComponent**)&m_pVIBuffer)))
		return E_FAIL;

	CGameInstance* pGI = CGameInstance::GetInstance();
	for (int i = 0; i < THORN_COUNT; ++i)
	{
		_tchar szTag[MAX_PATH] = L"";
		wsprintfW(szTag, L"player_thorn%d", i);
		if (nullptr == (m_pThorn[i] = (CThorn*)pGI->Add_GameObject(LEVEL_GAMEPLAY, TEXT("proto_obj_thorn"), TEXT("layer_effect"), szTag)))
			return E_FAIL;
		m_pThorn[i]->SetType(CThorn::TYPE::PLAYER_THORN);

		m_bScaleDown[i] = false;
		m_fScaleDownWait[i] = 0.f;
	}

	return S_OK;
}

HRESULT CSkillBlueEffect::Setup_ShaderResources()
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
	
	return S_OK;
}

CSkillBlueEffect * CSkillBlueEffect::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CSkillBlueEffect* pInstance = new CSkillBlueEffect(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CSkillBlueEffect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CSkillBlueEffect::Clone(void * pArg)
{
	CSkillBlueEffect* pInstance = new CSkillBlueEffect(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CSkillBlueEffect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSkillBlueEffect::Free()
{
	__super::Free();

	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);
	Safe_Release(m_pDiffuseTexture);
	Safe_Release(m_pTransform);
	Safe_Release(m_pVIBuffer);
}
