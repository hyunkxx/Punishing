#include "pch.h"
#include "..\Public\EnemyHealthBar.h"

#include "GameInstance.h"

CEnemyHealthBar::CEnemyHealthBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CEnemyHealthBar::CEnemyHealthBar(const CEnemyHealthBar& rhs)
	: CGameObject(rhs)
{
}

HRESULT CEnemyHealthBar::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEnemyHealthBar::Initialize(void * pArg)
{
 	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_fWidth = 450.f;
	m_fHeight = 10.f;
	m_fX = g_iWinSizeX >> 1;
	m_fY = 70.f;

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixScaling(m_fWidth, m_fHeight, 1.f) * XMMatrixTranslation(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f));
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));
	
	return S_OK;
}

void CEnemyHealthBar::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	CGameInstance* pInstance = CGameInstance::GetInstance();
}

void CEnemyHealthBar::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	if (nullptr != m_pRenderer && m_bRender)
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CEnemyHealthBar::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(Setup_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pTexture->Setup_ShaderResource(m_pShader, "g_Texture")))
		return E_FAIL;

	_float value = 1.f;
	if (FAILED(m_pShader->SetRawValue("g_FillAmount", &value, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShader->SetRawValue("g_Alpha", &value, sizeof(_float))))
		return E_FAIL;

	//마지막 줄이면 어둡게
	if (m_iCurHealthCount == 0)
		m_pShader->Begin(1);
	else
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

void CEnemyHealthBar::RenderGUI()
{
}

void CEnemyHealthBar::SetFillAmount(_float fCurHP, _float fMaxHP)
{
	_float fHealthCount = fMaxHP / 1000.f;
	m_iCurHealthCount = fCurHP / 1000.f;

	m_fFill = (fCurHP - (m_iCurHealthCount * 1000.f)) / 1000.f;
	m_fCurFill = m_fFill;
}

void CEnemyHealthBar::SetHealth(_float fCurHP, _float fMaxHP)
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

HRESULT CEnemyHealthBar::Add_Components()
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

	return S_OK;
}

HRESULT CEnemyHealthBar::Setup_ShaderResources()
{
	if (nullptr == m_pShader || nullptr == m_pTextureFront)
		return E_FAIL;

	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	return S_OK;
}

CEnemyHealthBar* CEnemyHealthBar::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEnemyHealthBar* pInstance = new CEnemyHealthBar(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		wstring message = L"Failed to Create : CEnemyHealthBar";
		MESSAGE(message);
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEnemyHealthBar::Clone(void* pArg)
{
	CEnemyHealthBar* pInstance = new CEnemyHealthBar(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		wstring message = L"Failed to Clone : CEnemyHealthBar";
		MESSAGE(message);
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEnemyHealthBar::Free()
{
	__super::Free();

	Safe_Release(m_pTexture);//Background
	Safe_Release(m_pTextureBlood);
	Safe_Release(m_pTextureFront);

	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);

	Safe_Release(m_pVIBuffer);//Background
	Safe_Release(m_pVIBufferBlood);
	Safe_Release(m_pVIBufferFront);
}
