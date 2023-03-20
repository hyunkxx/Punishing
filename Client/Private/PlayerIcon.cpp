#include "pch.h"
#include "..\Public\PlayerIcon.h"

#include "GameInstance.h"
#include "Character.h"

CPlayerIcon::CPlayerIcon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CPlayerIcon::CPlayerIcon(const CPlayerIcon& rhs)
	: CGameObject(rhs)
{
}

HRESULT CPlayerIcon::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayerIcon::Initialize(void * pArg)
{
 	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_fWidth = 90.f;
	m_fHeight = 90.f;

	m_fX = g_iWinSizeX - 60.f;
	m_fY = g_iWinSizeY - 120.f;

	m_fTargetWidth = 50.f;
	m_fTargetHeight = 50.f;

	XMStoreFloat4x4(&m_AttackMatrix, XMMatrixScaling(m_fWidth, m_fHeight, 1.f) * XMMatrixTranslation(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f));
	XMStoreFloat4x4(&m_DashMatrix, XMMatrixScaling(m_fWidth - 10.f, m_fHeight - 10.f, 1.f) * XMMatrixTranslation(m_fX - 80.f - g_iWinSizeX * 0.5f, -m_fY - 50.f + g_iWinSizeY * 0.5f, 0.f));

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));
	
	return S_OK;
}

void CPlayerIcon::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	_float2 vTargetPos = m_pPlayer->GetTargetWindowPos();
	if (vTargetPos.x != FLT_MAX && vTargetPos.y != FLT_MAX)
	{
		CGameInstance* pInstance = CGameInstance::GetInstance();

		m_bTargetImageRender = true;
		m_fTargetX = (vTargetPos.x + 1) * g_iWinSizeX * 0.5f + 0.f;
		m_fTargetY = (1 - vTargetPos.y) * g_iWinSizeY * 0.5f + g_iWinSizeY * 0.5f;

		//이미 스크린 스페이스임
		XMStoreFloat4x4(&m_TargetMatrix, XMMatrixScaling(m_fTargetWidth, m_fTargetHeight, 1.f) * XMMatrixTranslation(m_fTargetX - g_iWinSizeX * 0.5f, -m_fTargetY + g_iWinSizeY * 0.5f, 0.0f));

		//스크린 좌표
		/*_vector vScreenPos = (XMMatrixScaling(m_fTargetWidth, m_fTargetHeight, 1.f) * XMMatrixTranslation(m_fTargetX - g_iWinSizeX * 0.5f, -m_fTargetY + g_iWinSizeY * 0.5f, 0.f)).r[3];
		_vector vPos = XMVector3Unproject(vScreenPos, 0, 0, g_iWinSizeX, g_iWinSizeY, 0.f, 1000.f,
			pInstance->Get_Transform_Matrix(CPipeLine::TS_PROJ),
			pInstance->Get_Transform_Matrix(CPipeLine::TS_VIEW),
			m_pPlayer->GetTargetMatrix());

		_vector vTargetUIPos = XMVector3TransformCoord(vPos, m_pPlayer->GetTargetMatrix());
		XMStoreFloat4x4(&m_TargetMatrix, XMMatrixScaling(m_fTargetWidth, m_fTargetHeight, 1.f) * XMMatrixTranslation(XMVectorGetX(vTargetUIPos), XMVectorGetY(vTargetUIPos), XMVectorGetZ(vTargetUIPos)));*/
	}
	else
		m_bTargetImageRender = false;

	CGameInstance* pInstance = CGameInstance::GetInstance();
}

void CPlayerIcon::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	if (nullptr != m_pRenderer && m_bRender)
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CPlayerIcon::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(Setup_ShaderResources()))
		return E_FAIL;

	_float fValue = 1.f;
	_float fDiscardValue = 0.f;

	//버튼 백그라운드 이미지
	if (FAILED(m_pBackTexture->Setup_ShaderResource(m_pShader, "g_Texture")))
		return E_FAIL;

	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_AttackMatrix)))
		return E_FAIL;

	if (FAILED(m_pShader->SetRawValue("g_FillAmount", &fValue, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShader->SetRawValue("g_Alpha", &fValue, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShader->SetRawValue("g_DiscardValue", &fDiscardValue, sizeof(_float))))
		return E_FAIL;

	//공격 버튼 세팅
	if(m_pPlayer->IsAttackalbe())
		m_pShader->Begin(0);
	else
		m_pShader->Begin(2);

	m_pBackVIBuffer->Render();

	if (FAILED(m_pAttackTexture->Setup_ShaderResource(m_pShader, "g_Texture")))
		return E_FAIL;

	if (m_pPlayer->IsAttackalbe())
		m_pShader->Begin(0);
	else
		m_pShader->Begin(2);
	m_pAttackVIBuffer->Render();

	//대쉬 버튼 세팅
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_DashMatrix)))
		return E_FAIL;

	if (FAILED(m_pBackTexture->Setup_ShaderResource(m_pShader, "g_Texture")))
		return E_FAIL;

	if (m_pPlayer->IsDashable() && m_pPlayer->IsDashGageFull())
		m_pShader->Begin(0);
	else
		m_pShader->Begin(2);
	m_pBackVIBuffer->Render();

	if (FAILED(m_pDashTexture->Setup_ShaderResource(m_pShader, "g_Texture")))
		return E_FAIL;

	if (m_pPlayer->IsDashable() && m_pPlayer->IsDashGageFull())
		m_pShader->Begin(0);
	else
		m_pShader->Begin(2);

	m_pDashVIBuffer->Render();

	//타겟 락인
	if (m_bTargetImageRender)
	{
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_TargetMatrix)))
			return E_FAIL;

		if (FAILED(m_pTargetTexture->Setup_ShaderResource(m_pShader, "g_Texture")))
			return E_FAIL;

		m_pShader->Begin(0);
		m_pTargetVIBuffer->Render();
	}

	return S_OK;
}

void CPlayerIcon::RenderGUI()
{
}

HRESULT CPlayerIcon::Add_Components()
{
 	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("proto_com_renderer"),
		TEXT("com_renderer"), (CComponent**)&m_pRenderer)))
		return E_FAIL;

	//버퍼
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("proto_com_vibuffer_rect"),
		TEXT("com_backIcon"), (CComponent**)&m_pBackVIBuffer)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("proto_com_vibuffer_rect"),
		TEXT("com_vibuffer_attackicon"), (CComponent**)&m_pAttackVIBuffer)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("proto_com_vibuffer_rect"),
		TEXT("com_vibuffer_dashicon"), (CComponent**)&m_pDashVIBuffer)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("proto_com_vibuffer_rect"),
		TEXT("com_vibuffer_evolutionicon"), (CComponent**)&m_pEvolutionVIBuffer)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("proto_com_vibuffer_rect"),
		TEXT("com_vibuffer_target"), (CComponent**)&m_pTargetVIBuffer)))
		return E_FAIL;

	//쉐이더
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("proto_com_shader_ui"),
		TEXT("com_shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	//텍스쳐
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("proto_com_texture_back_icon"),
		TEXT("com_texture_back"), (CComponent**)&m_pBackTexture)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("proto_com_texture_attack_icon"),
		TEXT("com_texture_attack"), (CComponent**)&m_pAttackTexture)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("proto_com_texture_dash_icon"),
		TEXT("com_texture_dash"), (CComponent**)&m_pDashTexture)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("proto_com_texture_target"),
		TEXT("com_texture_target"), (CComponent**)&m_pTargetTexture)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayerIcon::Setup_ShaderResources()
{
	if (nullptr == m_pShader)
		return E_FAIL;

	if (FAILED(m_pShader->SetMatrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	return S_OK;
}

CPlayerIcon* CPlayerIcon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlayerIcon* pInstance = new CPlayerIcon(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		wstring message = L"Failed to Create : CPlayerIcon";
		MESSAGE(message);
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPlayerIcon::Clone(void* pArg)
{
	CPlayerIcon* pInstance = new CPlayerIcon(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		wstring message = L"Failed to Clone : CPlayerIcon";
		MESSAGE(message);
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerIcon::Free()
{
	__super::Free();

	Safe_Release(m_pBackVIBuffer);
	Safe_Release(m_pBackTexture);
	Safe_Release(m_pAttackVIBuffer);
	Safe_Release(m_pAttackTexture);
	Safe_Release(m_pDashVIBuffer);
	Safe_Release(m_pDashTexture);
	Safe_Release(m_pEvolutionVIBuffer);
	Safe_Release(m_pEvolutionTexture);
	Safe_Release(m_pTargetVIBuffer);
	Safe_Release(m_pTargetTexture);

	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);
}
