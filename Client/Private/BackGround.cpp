#include "pch.h"
#include "..\Public\BackGround.h"

#include "GameInstance.h"
#include "ApplicationManager.h"

CBackGround::CBackGround(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CBackGround::CBackGround(const CBackGround& rhs)
	: CGameObject(rhs)
{
}

HRESULT CBackGround::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBackGround::Initialize(void * pArg)
{
 	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_fWidth = g_iWinSizeX;
	m_fHeight = g_iWinSizeY;
	m_fX = g_iWinSizeX >> 1;
	m_fY = g_iWinSizeY >> 1;

	m_fRotWidth = 60.f;
	m_fRotHeight = 60.f;
	m_fRotX = g_iWinSizeX - 55.f;
	m_fRotY = g_iWinSizeY - 55.f;

	XMStoreFloat4x4(&m_RotMatrix, XMMatrixScaling(m_fRotWidth, m_fRotHeight, 1.f) * XMMatrixTranslation(m_fRotX - g_iWinSizeX * 0.5f, -m_fRotY + g_iWinSizeY * 0.5f, 0.f));
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixScaling(m_fWidth, m_fHeight, 1.f) * XMMatrixTranslation(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f));
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));
	
	return S_OK;
}

void CBackGround::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	m_fAngle += TimeDelta * (_float)2.0;
	//if (m_fAngle > 3.141592 * 2.f)
	//	m_fAngle = 0.f;

	if (m_bToggle)
	{
		m_fAlpha += TimeDelta;
		if (m_fAlpha >= 1.f)
		{
			m_bToggle = false;
		}
	}

	if (m_bEnd)
	{
		if (!bWarningSound)
		{
			bWarningSound = true;
			CGameInstance* pGameInstance = CGameInstance::GetInstance();
			pGameInstance->PlaySoundEx(L"Warning.wav", SOUND_CHANNEL::WARNING, SOUND_VOLUME::CUSTOM_VOLUM, 0.7f);
		}

		m_fAlpha -= TimeDelta;
		m_fEndAcc += TimeDelta;
	}

	XMStoreFloat4x4(&m_RotMatrix, XMMatrixScaling(m_fRotWidth, m_fRotHeight, 1.f) * XMMatrixRotationZ(-m_fAngle) * XMMatrixTranslation(m_fRotX - g_iWinSizeX * 0.5f, -m_fRotY + g_iWinSizeY * 0.5f, 0.f));
}

void CBackGround::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	if (nullptr != m_pRenderer)
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CBackGround::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(Setup_ShaderResources()))
		return E_FAIL;
	if (FAILED(m_pGradientMask->Setup_ShaderResource(m_pShader, "g_MaskTexture")))
		return E_FAIL;
	//백그라운드
	m_pShader->Begin(8);
	m_pVIBuffer->Render();

	//뱅글뱅글 로딩 게이지
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_RotMatrix)))
		return E_FAIL;
	if (FAILED(m_pRotationTexture->Setup_ShaderResource(m_pShader, "g_Texture")))
		return E_FAIL;

	m_pShader->SetRawValue("g_TimeAcc", &m_fAlpha, sizeof(float));
	m_pShader->Begin(6);
	m_pVIBuffer->Render();

	return S_OK;
}

void CBackGround::RenderGUI()
{
}

HRESULT CBackGround::Add_Components()
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

	if (FAILED(__super::Add_Component(LEVEL_LOADING, TEXT("proto_com_texture_background"),
		TEXT("com_texture1"), (CComponent**)&m_pTexture1)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_LOADING, TEXT("proto_com_texture_background2"),
		TEXT("com_texture2"), (CComponent**)&m_pTexture2)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_LOADING, TEXT("proto_com_texture_loading"),
		TEXT("com_texture_loading"), (CComponent**)&m_pRotationTexture)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("proto_com_texture_gradmask"),
		TEXT("con_texture_gradmask"), (CComponent**)&m_pGradientMask)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBackGround::Setup_ShaderResources()
{
	if (nullptr == m_pShader || nullptr == m_pTexture1 || nullptr == m_pTexture2)
		return E_FAIL;

	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (!CApplicationManager::GetInstance()->IsLevelFinish(CApplicationManager::LEVEL::GAMEPLAY))
	{
		if (FAILED(m_pTexture1->Setup_ShaderResource(m_pShader, "g_Texture")))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pTexture2->Setup_ShaderResource(m_pShader, "g_Texture")))
			return E_FAIL;
	}

	return S_OK;
}

CBackGround* CBackGround::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBackGround* pInstance = new CBackGround(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		wstring message = L"Failed to Create : CBackGround";
		MESSAGE(message);
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBackGround::Clone(void* pArg)
{
	CBackGround* pInstance = new CBackGround(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		wstring message = L"Failed to Clone : CBackGround";
		MESSAGE(message);
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBackGround::Free()
{
	__super::Free();

	Safe_Release(m_pTexture1);
	Safe_Release(m_pTexture2);
	Safe_Release(m_pRotationTexture);

	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);
	Safe_Release(m_pVIBuffer);
}
