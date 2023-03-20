#include "pch.h"
#include "..\Public\PlayerHealthBar.h"

#include "ApplicationManager.h"
#include "GameInstance.h"

CPlayerHealthBar::CPlayerHealthBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CPlayerHealthBar::CPlayerHealthBar(const CPlayerHealthBar& rhs)
	: CGameObject(rhs)
{
}

HRESULT CPlayerHealthBar::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayerHealthBar::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_fWidth = 300.f;
	m_fHeight = 20.f;
	m_fX = 175.f;
	m_fY = 100.f;

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixScaling(m_fWidth - 75.f, m_fHeight - 10.f, 1.f) * XMMatrixTranslation(m_fX + 25.f - g_iWinSizeX * 0.5f, -m_fY - 20.f + g_iWinSizeY * 0.5f, 0.f));
	XMStoreFloat4x4(&m_PlayerDashMatrix, XMMatrixScaling(m_fWidth - 75.f, m_fHeight - 10.f, 1.f) * XMMatrixTranslation(m_fX + 25.f - g_iWinSizeX * 0.5f, -m_fY - 20.f + g_iWinSizeY * 0.5f, 0.f));

	XMStoreFloat4x4(&m_PlayerBackWorld, XMMatrixScaling(m_fWidth + 100.f, m_fHeight * 5.f, 1.f) * XMMatrixTranslation((m_fX - 50.f) - g_iWinSizeX * 0.5f, -m_fY + (g_iWinSizeY - 50.f) * 0.5f, 0.f));
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));

	return S_OK;
}

void CPlayerHealthBar::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	CGameInstance* pInstance = CGameInstance::GetInstance();
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixScaling(m_fWidth - 75.f, m_fHeight - 10.f, 1.f) * XMMatrixTranslation(m_fX + 30.f - g_iWinSizeX * 0.5f, -m_fY + 35.f + g_iWinSizeY * 0.5f, 0.f));
	XMStoreFloat4x4(&m_PlayerDashMatrix, XMMatrixScaling(m_fWidth - 75.f, m_fHeight - 14.f, 1.f) * XMMatrixTranslation(m_fX + 30.f - g_iWinSizeX * 0.5f, -m_fY + 20.f + g_iWinSizeY * 0.5f, 0.f));
	XMStoreFloat4x4(&m_PlayerBackWorld, XMMatrixScaling(m_fWidth + 100.f, m_fHeight * 5.f, 1.f) * XMMatrixTranslation((m_fX) - g_iWinSizeX * 0.5f, (-m_fY + 50.f) + g_iWinSizeY * 0.5f, 0.f));
}

void CPlayerHealthBar::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	if (nullptr != m_pRenderer && m_bRender)
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CPlayerHealthBar::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(Setup_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_PlayerBackWorld)))
		return E_FAIL;

	static const _float value = 1.f;
	static const _float discardValue = 0.f;
	if (FAILED(m_pShader->SetRawValue("g_FillAmount", &value, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShader->SetRawValue("g_DiscardValue", &discardValue, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShader->SetRawValue("g_Alpha", &value, sizeof(_float))))
		return E_FAIL; 

	if (FAILED(m_pPlayerBackImage->Setup_ShaderResource(m_pShader, "g_Texture")))
		return E_FAIL;

	m_pShader->Begin(0);
	m_pPlayerBackBuffer->Render();

	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_PlayerDashMatrix)))
		return E_FAIL;

	if (FAILED(m_pDashImage->Setup_ShaderResource(m_pShader, "g_Texture")))
		return E_FAIL;

	if (FAILED(m_pShader->SetRawValue("g_FillAmount", &m_fDashFill, sizeof(_float))))
		return E_FAIL;

	//빨강 대쉬 불가 , 주황 레디 , blue
	if(m_fCurDashGage < 20.f)
		m_pShader->Begin(2);
	else if(!CApplicationManager::GetInstance()->IsFreezeReady())
		m_pShader->Begin(3);
	else
		m_pShader->Begin(0);

	m_pDashBuffer->Render();

	if (FAILED(m_pShader->SetRawValue("g_FillAmount", &value, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;

	if (FAILED(m_pTexture->Setup_ShaderResource(m_pShader, "g_Texture")))
		return E_FAIL;

	m_pShader->Begin(0);
	m_pVIBuffer->Render();

	if (FAILED(m_pTextureBlood->Setup_ShaderResource(m_pShader, "g_Texture")))
		return E_FAIL;

	if (FAILED(m_pShader->SetRawValue("g_FillAmount", &m_fCurFill, sizeof(_float))))
		return E_FAIL;

	m_pShader->Begin(0);
	m_pVIBufferBlood->Render();

	if (FAILED(m_pTextureFront->Setup_ShaderResource(m_pShader, "g_Texture")))
		return E_FAIL;

	if (FAILED(m_pShader->SetRawValue("g_FillAmount", &m_fFill, sizeof(_float))))
		return E_FAIL;

	m_pShader->Begin(0);
	m_pVIBufferFront->Render();

	return S_OK;
}

void CPlayerHealthBar::RenderGUI()
{
}

void CPlayerHealthBar::SetFillAmount(_float fCurHP, _float fMaxHP)
{
	_float fHealthCount = fMaxHP / 1000.f;
	m_iCurHealthCount = fCurHP / 1000.f;

	m_fFill = (fCurHP - (m_iCurHealthCount * 1000.f)) / 1000.f;
	m_fCurFill = m_fFill;
}

void CPlayerHealthBar::SetHealth(_float fCurHP, _float fMaxHP)
{
	CGameInstance* pInstance = CGameInstance::GetInstance();
	_float DeltaTime = pInstance->GetTimer(TEXT("144FPS"));

	_float fHealthCount = fMaxHP / 1000.f;
	m_iCurHealthCount = fCurHP / 1000.f;

	if ((_int)fHealthCount == m_iCurHealthCount)
	{
		m_fFill = 1.f;
		m_fCurFill = 1.f;
	}
	else
	{
		m_fFill = (fCurHP - (m_iCurHealthCount * 1000.f)) / 1000.f;

		if (m_fCurFill < m_fFill)
			m_fCurFill = 1.f;
	}

	_vector vCur = XMVectorSet(m_fCurFill, m_fCurFill, m_fCurFill, m_fCurFill);
	_vector vTarget = XMVectorSet(m_fFill, m_fFill, m_fFill, m_fFill);

	m_fCurFill = XMVectorGetX(XMVectorLerp(vCur, vTarget, (_float)DeltaTime * 1.5));

}

void CPlayerHealthBar::SetDashFillAmount(_float fCurDashGage, _float fMaxDashGage)
{
	m_fDashFill = fCurDashGage / 100.f;
	m_fDashCurFill = m_fDashFill;
}

void CPlayerHealthBar::SetDash(_float fCurDashGage, _float fMaxDashGage)
{
	CGameInstance* pInstance = CGameInstance::GetInstance();
	_float DeltaTime = pInstance->GetTimer(TEXT("144FPS"));

	m_fCurDashGage = fCurDashGage;

	m_fDashFill = fCurDashGage / 100.f;

	_vector vCur = XMVectorSet(m_fDashCurFill, m_fDashCurFill, m_fDashCurFill, m_fDashCurFill);
	_vector vTarget = XMVectorSet(m_fDashFill, m_fDashFill, m_fDashFill, m_fDashFill);

	m_fDashCurFill = XMVectorGetX(XMVectorLerp(vCur, vTarget, (_float)DeltaTime * 1.5));
}

HRESULT CPlayerHealthBar::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("proto_com_renderer"),
		TEXT("com_renderer"), (CComponent**)&m_pRenderer)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("proto_com_vibuffer_rect"),
		TEXT("com_vibuffer"), (CComponent**)&m_pVIBuffer)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("proto_com_vibuffer_rect"),
		TEXT("com_vibuffer_blood"), (CComponent**)&m_pVIBufferBlood)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("proto_com_vibuffer_rect"),
		TEXT("com_vibuffer_front"), (CComponent**)&m_pVIBufferFront)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("proto_com_shader_ui"),
		TEXT("com_shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("proto_com_texture_healthbar_back"),
		TEXT("com_texture_back"), (CComponent**)&m_pTexture)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("proto_com_texture_healthbar_blood"),
		TEXT("com_texture_blood"), (CComponent**)&m_pTextureBlood)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("proto_com_texture_healthbar_front"),
		TEXT("com_texture_front"), (CComponent**)&m_pTextureFront)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("proto_com_vibuffer_rect"),
		TEXT("com_texture_playerback_buffer"), (CComponent**)&m_pPlayerBackBuffer)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("proto_com_texture_player_back"),
		TEXT("com_texture_playerback"), (CComponent**)&m_pPlayerBackImage)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("proto_com_vibuffer_rect"),
		TEXT("com_texture_dash_buffer"), (CComponent**)&m_pDashBuffer)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("proto_com_texture_dash"),
		TEXT("com_texture_dash"), (CComponent**)&m_pDashImage)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayerHealthBar::Setup_ShaderResources()
{
	if (nullptr == m_pShader || nullptr == m_pTextureFront)
		return E_FAIL;

	CGameInstance* pInstance = CGameInstance::GetInstance();

	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;

	if (FAILED(m_pShader->SetMatrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShader->SetMatrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	return S_OK;
}

CPlayerHealthBar* CPlayerHealthBar::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlayerHealthBar* pInstance = new CPlayerHealthBar(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		wstring message = L"Failed to Create : CPlayerHealthBar";
		MESSAGE(message);
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPlayerHealthBar::Clone(void* pArg)
{
	CPlayerHealthBar* pInstance = new CPlayerHealthBar(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		wstring message = L"Failed to Clone : CPlayerHealthBar";
		MESSAGE(message);
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerHealthBar::Free()
{
	__super::Free();

	Safe_Release(m_pDashImage);
	Safe_Release(m_pDashBuffer);

	Safe_Release(m_pPlayerBackBuffer);
	Safe_Release(m_pPlayerBackImage);

	Safe_Release(m_pTexture);//Background
	Safe_Release(m_pTextureBlood);
	Safe_Release(m_pTextureFront);

	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);

	Safe_Release(m_pVIBuffer);//Background
	Safe_Release(m_pVIBufferBlood);
	Safe_Release(m_pVIBufferFront);
}
