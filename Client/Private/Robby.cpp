#include "pch.h"
#include "..\Public\Robby.h"

#include "GameInstance.h"
#include "ApplicationManager.h"

CRobby::CRobby(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CRobby::CRobby(const CRobby& rhs)
	: CGameObject(rhs)
{
}

HRESULT CRobby::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRobby::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_fWidth = g_iWinSizeX;
	m_fHeight = g_iWinSizeY;
	m_fX = g_iWinSizeX >> 1;
	m_fY = g_iWinSizeY >> 1;

	m_LogoW = 90.f;
	m_LogoH = 90.f;
	m_LogofX = g_iWinSizeX - 90.f;
	m_LogofY = g_iWinSizeY - 90.f;


	XMStoreFloat4x4(&m_LogoMatrix, XMMatrixScaling(m_LogoW, m_LogoH, 1.f) * XMMatrixTranslation(m_LogofX - g_iWinSizeX * 0.5f, -m_LogofY + g_iWinSizeY * 0.5f, 0.f));
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixScaling(m_fWidth, m_fHeight, 1.f) * XMMatrixTranslation(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f));
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));

	return S_OK;
}

void CRobby::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	if (m_bOpen)
	{
		m_fAlphaAcc += TimeDelta * 0.4f;
		if (m_fAlphaAcc >= 1.0f)
		{
			m_fAlphaAcc = 1.f;
			m_bOpen = false;
			m_bWait = true;
		}
	}
	else
	{
		if (m_bWait)
		{
			m_fTimeAcc += TimeDelta;
			if (m_fTimeAcc >= 1.f)
			{
				m_fTimeAcc = 0.f;
				m_bWait = false;
			}
		}
		else
		{
			m_fAlphaAcc -= TimeDelta * 0.4f;
			if (m_fAlphaAcc <= 0.f)
			{
				m_fAlphaAcc = 0.f;
				m_bOpen = true;
				if (m_iCurrentIndex < 2)
					m_iCurrentIndex++;
				else
					m_iCurrentIndex = 0;
			}
		}
	}


	if (m_fAlphaAcc >= 1.f)
		m_fAlphaAcc = 1.f;

	if (m_fAlphaAcc <= 0.f)
		m_fAlphaAcc = 0.f;
}

void CRobby::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	if (nullptr != m_pRenderer)
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CRobby::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(Setup_ShaderResources()))
		return E_FAIL;

	//백
	if (FAILED(m_pRobbyLoadingTexture->Setup_ShaderResource(m_pShader, "g_Texture", m_iCurrentIndex)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	m_pGradientMask->Setup_ShaderResource(m_pShader, "g_MaskTexture");
	m_pShader->SetRawValue("g_TimeAcc", &m_fAlphaAcc, sizeof(float));
	m_pShader->Begin(8);
	m_pVIBuffer->Render();

	//로고
	//if (FAILED(m_pLogoTexture->Setup_ShaderResource(m_pShader, "g_Texture", 0)))
	//	return E_FAIL;
	//if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_LogoMatrix)))
	//	return E_FAIL;
	//float value = 1.f;
	//m_pShader->SetRawValue("g_TimeAcc", &value, sizeof(float));
	//m_pShader->Begin(9);
	//m_pVIBuffer->Render();

	////로고
	//m_pLogoTexture->Setup_ShaderResource(m_pShader, "g_Texture", 1);
	//if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_LogoMatrix)))
	//	return E_FAIL;
	//value = 1.f;
	//m_pShader->SetRawValue("g_TimeAcc", &value, sizeof(float));
	//m_pShader->Begin(8);
	//m_pVIBuffer->Render();

	return S_OK;
}

void CRobby::RenderGUI()
{
}

HRESULT CRobby::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("proto_com_renderer"),
		TEXT("com_renderer"), (CComponent**)&m_pRenderer)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("proto_com_vibuffer_rect"),
		TEXT("com_vibuffer"), (CComponent**)&m_pVIBuffer)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("proto_com_shader_vtxtex"),
		TEXT("com_shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("proto_com_texture_robbyloading"),
		TEXT("con_texture_loading"), (CComponent**)&m_pRobbyLoadingTexture)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("proto_com_texture_gradmask"),
		TEXT("con_texture_gradmask"), (CComponent**)&m_pGradientMask)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("proto_com_texture_logo"),
		TEXT("con_texture_logo"), (CComponent**)&m_pLogoTexture)))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CRobby::Setup_ShaderResources()
{
	if (nullptr == m_pShader)
		return E_FAIL;

	if (FAILED(m_pShader->SetMatrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	return S_OK;
}

CRobby* CRobby::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRobby* pInstance = new CRobby(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		wstring message = L"Failed to Create : CRobby";
		MESSAGE(message);
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CRobby::Clone(void* pArg)
{
	CRobby* pInstance = new CRobby(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		wstring message = L"Failed to Clone : CRobby";
		MESSAGE(message);
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRobby::Free()
{
	__super::Free();

	Safe_Release(m_pRobbyLoadingTexture);
	Safe_Release(m_pLogoTexture);
	Safe_Release(m_pGradientMask);

	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);
	Safe_Release(m_pVIBuffer);
}
