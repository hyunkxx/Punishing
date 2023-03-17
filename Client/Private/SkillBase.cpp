#include "pch.h"
#include "..\Public\SkillBase.h"

#include "GameInstance.h"

_uint CSkillBase::s_iID = 0;

CSkillBase::CSkillBase(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CSkillBase::CSkillBase(const CSkillBase& rhs)
	: CGameObject(rhs)
{
	s_iID++;
}

HRESULT CSkillBase::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}


HRESULT CSkillBase::Initialize(void * pArg)
{
	if (pArg != nullptr)
		memcpy(&m_eType, (TYPE*)pArg, sizeof(TYPE));

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_fWidth = 50.f;
	m_fHeight = 50.f;
	m_fX = g_iWinSizeX >> 1;
	m_fY = g_iWinSizeY - 100.f;

	m_fGoalX = m_fX;

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixScaling(m_fWidth, m_fHeight, 1.f) * XMMatrixTranslation(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f));
	XMStoreFloat4x4(&m_KeyWorldMatrix, XMMatrixScaling(m_fWidth * 0.35f, m_fHeight * 0.35f, 1.f) * XMMatrixTranslation(m_fX - g_iWinSizeX * 0.5f - (m_fWidth * 0.4f), -m_fY + g_iWinSizeY * 0.5f - (m_fHeight * 0.4f), 0.f));

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));

	return S_OK;
}


void CSkillBase::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	if (m_bAlign)
		return;

	CGameInstance* pInstance = CGameInstance::GetInstance();
		
	if (m_fX > m_fGoalX)
	{
		m_fX -= 1300.f * (_float)TimeDelta;
		XMStoreFloat4x4(&m_WorldMatrix, XMMatrixScaling(m_fWidth, m_fHeight, 1.f) * XMMatrixTranslation(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f));
		XMStoreFloat4x4(&m_KeyWorldMatrix, XMMatrixScaling(m_fWidth * 0.35f, m_fHeight * 0.35f, 1.f) * XMMatrixTranslation(m_fX - g_iWinSizeX * 0.5f - (m_fWidth * 0.4f), -m_fY + g_iWinSizeY * 0.5f - (m_fHeight * 0.4f), 0.f));
	}
	else
	{
		m_bAlign = true;
		m_fX = m_fGoalX;
		XMStoreFloat4x4(&m_WorldMatrix, XMMatrixScaling(m_fWidth, m_fHeight, 1.f) * XMMatrixTranslation(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f));
		XMStoreFloat4x4(&m_KeyWorldMatrix, XMMatrixScaling(m_fWidth * 0.35f, m_fHeight * 0.35f, 1.f) * XMMatrixTranslation(m_fX - g_iWinSizeX * 0.5f - (m_fWidth * 0.4f), -m_fY + g_iWinSizeY * 0.5f - (m_fHeight * 0.4f), 0.f));
	}
}

void CSkillBase::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	if (nullptr != m_pRenderer && m_bRender)
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CSkillBase::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(Setup_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pTexture->Setup_ShaderResource(m_pShader, "g_Texture")))
		return E_FAIL;

	//스킬 텍스쳐
	_float fFill = 1.f;
	if (FAILED(m_pShader->SetRawValue("g_FillAmount", &fFill, sizeof(_float))))
		return E_FAIL;

	m_pShader->Begin(0);
	m_pVIBuffer->Render();

	//키 텍스쳐
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_KeyWorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pKeyTexture[m_iIndex]->Setup_ShaderResource(m_pShader, "g_Texture")))
		return E_FAIL;

	m_pShader->Begin(0);
	m_pKeyBackgroundBuffer->Render();


	return S_OK;
}

void CSkillBase::RenderGUI()
{
	ImGui::Begin("Skill");
	string index = "index " + to_string(m_iIndex);

	int random = rand() % 10;
	string pos = "pos X " + to_string(random);
	//ImGui::Text(index.c_str());
	ImGui::Text(pos.c_str());
	ImGui::End();
}

void CSkillBase::SetIndex(_uint iIndex)
{
	m_bAlign = false;
	m_iIndex = iIndex;
	m_fGoalX = (iIndex + 1) * 50.f;
}

HRESULT CSkillBase::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("proto_com_renderer"),
		TEXT("com_renderer"), (CComponent**)&m_pRenderer)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("proto_com_vibuffer_rect"),
		TEXT("com_vibuffer"), (CComponent**)&m_pVIBuffer)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("proto_com_shader_ui"),
		TEXT("com_shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	switch (m_eType)
	{
	case Client::CSkillBase::TYPE::RED:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("proto_com_texture_red"),
			TEXT("com_texture"), (CComponent**)&m_pTexture)))
			return E_FAIL;

		break;
	case Client::CSkillBase::TYPE::BLUE:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("proto_com_texture_blue"),
			TEXT("com_texture"), (CComponent**)&m_pTexture)))
			return E_FAIL;

		break;
	case Client::CSkillBase::TYPE::YELLOW:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("proto_com_texture_yellow"),
			TEXT("com_texture"), (CComponent**)&m_pTexture)))
			return E_FAIL;

		break;
	case Client::CSkillBase::TYPE::INVALID:
	default:
		break;
	}

	//qwerasdf 세팅
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("proto_com_vibuffer_rect"),
		TEXT("com_vibuffer_key_back"), (CComponent**)&m_pKeyBackgroundBuffer)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("proto_com_texture_a"),
		TEXT("com_texture_a"), (CComponent**)&m_pKeyTexture[(_uint)KEY::A])))
		return E_FAIL;
	
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("proto_com_texture_s"),
		TEXT("com_texture_s"), (CComponent**)&m_pKeyTexture[(_uint)KEY::S])))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("proto_com_texture_d"),
		TEXT("com_texture_d"), (CComponent**)&m_pKeyTexture[(_uint)KEY::D])))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("proto_com_texture_f"),
		TEXT("com_texture_f"), (CComponent**)&m_pKeyTexture[(_uint)KEY::F])))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("proto_com_texture_q"),
		TEXT("com_texture_q"), (CComponent**)&m_pKeyTexture[(_uint)KEY::Q])))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("proto_com_texture_w"),
		TEXT("com_texture_w"), (CComponent**)&m_pKeyTexture[(_uint)KEY::W])))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("proto_com_texture_e"),
		TEXT("com_texture_e"), (CComponent**)&m_pKeyTexture[(_uint)KEY::E])))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("proto_com_texture_r"),
		TEXT("com_texture_r"), (CComponent**)&m_pKeyTexture[(_uint)KEY::R])))
		return E_FAIL;


	return S_OK;
}

HRESULT CSkillBase::Setup_ShaderResources()
{
	if (nullptr == m_pShader || nullptr == m_pTexture)
		return E_FAIL;

	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	return S_OK;
}

CSkillBase* CSkillBase::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSkillBase* pInstance = new CSkillBase(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		wstring message = L"Failed to Create : CSkillBase";
		MESSAGE(message);
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSkillBase::Clone(void* pArg)
{
	CSkillBase* pInstance = new CSkillBase(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		wstring message = L"Failed to Clone : CSkillBase";
		MESSAGE(message);
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSkillBase::Free()
{
	__super::Free();

	Safe_Release(m_pKeyBackgroundBuffer);
	Safe_Release(m_pTexture);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);
	Safe_Release(m_pVIBuffer);
}
