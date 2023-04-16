#include "pch.h"
#include "..\Public\DamageFont.h"

#include "ApplicationManager.h"
#include "GameInstance.h"
#include "Character.h"
#include "Enemy.h"
#include "Boss.h"

CDamageFont::CDamageFont(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CDamageFont::CDamageFont(const CDamageFont& rhs)
	: CGameObject(rhs)
{
}

HRESULT CDamageFont::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CDamageFont::Initialize(void * pArg)
{ 
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (pArg != nullptr)
		m_pPlayer = (CCharacter*)pArg;

	m_fDamageWidth  = 13.f;
	m_fDamageHeight = 20.f;
	m_fDamageX = g_iWinSizeX >> 1;
	m_fDamageY = g_iWinSizeY >> 1;

	for (int i = 0; i < 4; ++i)
		XMStoreFloat4x4(&m_DamageMatrix[i], XMMatrixScaling(m_fDamageWidth, m_fDamageHeight, 1.f) * XMMatrixTranslation(m_fDamageX - g_iWinSizeX * 0.5f, -m_fDamageY + g_iWinSizeY * 0.5f, 0.f));

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));

	return S_OK;
}

void CDamageFont::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
	CGameInstance* pInstance = CGameInstance::GetInstance();

	switch (m_strCombo.size())
	{
	case 0:
		m_fDamageX = m_fOriginDamageX;
		break;
	case 1:
		m_fDamageX = m_fOriginDamageX + 13.f;
		break;
	case 2:
		m_fDamageX = m_fOriginDamageX + 13.f * 2.f;
		break;
	case 3:
		m_fDamageX = m_fOriginDamageX + 13.f * 3.f;
		break;
	case 4:
		m_fDamageX = m_fOriginDamageX + 13.f * 4.f;
		break;
	default:
		break;
	}

	for (int i = 0; i < 4; ++i)
		XMStoreFloat4x4(&m_DamageMatrix[i], XMMatrixScaling(m_fDamageWidth, m_fDamageHeight, 1.f) * XMMatrixTranslation(m_fDamageX - g_iWinSizeX * 0.5f, -m_fDamageY + g_iWinSizeY * 0.5f, 0.f));

}

void CDamageFont::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	CGameInstance* pInstance = CGameInstance::GetInstance();

	if (nullptr != m_pRenderer && m_bRender)
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CDamageFont::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(Setup_ShaderResources()))
		return E_FAIL;

	_float fValue = 1.f;
	_float fDiscardValue = 0.f;

	if (FAILED(m_pShader->SetRawValue("g_FillAmount", &fValue, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShader->SetRawValue("g_Alpha", &fValue, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShader->SetRawValue("g_DiscardValue", &fDiscardValue, sizeof(_float))))
		return E_FAIL;

	//GetComboCount였음 
	for (int i = 0; i < 3; ++i)
	{
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_DamageMatrix[i])))
			return E_FAIL;

		if (FAILED(ComputeComboToTexture(i)->Setup_ShaderResource(m_pShader, "g_Texture")))
			return E_FAIL;

		m_pShader->Begin(0);
		m_pDamageBuffer->Render();
	}

	return S_OK;
}

void CDamageFont::RenderGUI()
{
}

CTexture* CDamageFont::ComputeComboToTexture(int iIndex)
{
	string strCombo = to_string(123);
	int index = strCombo[iIndex] - '0';
	return m_pDamageTexture[index];
}

void CDamageFont::SetPosition()
{
	m_bRender = true;
	_float2 vTargetPos = m_pPlayer->GetTargetWindowPos();
	if (vTargetPos.x != FLT_MAX && vTargetPos.y != FLT_MAX)
	{
		CGameInstance* pInstance = CGameInstance::GetInstance();

		//보스쪽 타겟
		if (CApplicationManager::GetInstance()->IsLevelFinish(CApplicationManager::LEVEL::GAMEPLAY))
		{
			//보스가 스폰된 상태 혹은 버로우하지 않은상태일때만 락온
			CBoss* pBoss = static_cast<CBoss*>(m_pPlayer->GetLockOnTarget());
			_vector vBossPos = XMLoadFloat4(&pBoss->GetPosition());
			_vector vPlayerPos = static_cast<CTransform*>(m_pPlayer->Find_Component(L"com_transform"))->Get_State(CTransform::STATE_POSITION);
			_float fLength = XMVectorGetX(XMVector3Length(vBossPos - vPlayerPos));

			if (pBoss && pBoss->IsSpawned())
			{
				m_fOriginDamageX = (vTargetPos.x + 1) * g_iWinSizeX * 0.5f;
				m_fDamageY = (1 - vTargetPos.y) * g_iWinSizeY * 0.5f + g_iWinSizeY * 0.5f;
			}
		}
		else
		{
			m_fOriginDamageX = (vTargetPos.x + 1) * g_iWinSizeX * 0.5f;
			m_fDamageY = (1 - vTargetPos.y) * g_iWinSizeY * 0.5f + g_iWinSizeY * 0.5f;
		}

	}
}

HRESULT CDamageFont::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("proto_com_renderer"),
		TEXT("com_renderer"), (CComponent**)&m_pRenderer)))
		return E_FAIL;

	//버퍼
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("proto_com_vibuffer_rect"),
		TEXT("com_backIcon"), (CComponent**)&m_pDamageBuffer)))
		return E_FAIL;

	//쉐이더
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("proto_com_shader_ui"),
		TEXT("com_shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	//데미지 숫자
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("proto_com_texture_0"),
		TEXT("com_texture_0"), (CComponent**)&m_pDamageTexture[0])))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("proto_com_texture_1"),
		TEXT("com_texture_1"), (CComponent**)&m_pDamageTexture[1])))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("proto_com_texture_2"),
		TEXT("com_texture_2"), (CComponent**)&m_pDamageTexture[2])))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("proto_com_texture_3"),
		TEXT("com_texture_3"), (CComponent**)&m_pDamageTexture[3])))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("proto_com_texture_4"),
		TEXT("com_texture_4"), (CComponent**)&m_pDamageTexture[4])))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("proto_com_texture_5"),
		TEXT("com_texture_5"), (CComponent**)&m_pDamageTexture[5])))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("proto_com_texture_6"),
		TEXT("com_texture_6"), (CComponent**)&m_pDamageTexture[6])))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("proto_com_texture_7"),
		TEXT("com_texture_7"), (CComponent**)&m_pDamageTexture[7])))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("proto_com_texture_8"),
		TEXT("com_texture_8"), (CComponent**)&m_pDamageTexture[8])))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("proto_com_texture_9"),
		TEXT("com_texture_9"), (CComponent**)&m_pDamageTexture[9])))
		return E_FAIL;

	return S_OK;
}

HRESULT CDamageFont::Setup_ShaderResources()
{
	if (nullptr == m_pShader)
		return E_FAIL;

	if (FAILED(m_pShader->SetMatrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	return S_OK;
}

CDamageFont* CDamageFont::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDamageFont* pInstance = new CDamageFont(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		wstring message = L"Failed to Create : CDamageFont";
		MESSAGE(message);
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDamageFont::Clone(void* pArg)
{
	CDamageFont* pInstance = new CDamageFont(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		wstring message = L"Failed to Clone : CDamageFont";
		MESSAGE(message);
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDamageFont::Free()
{
	__super::Free();

	Safe_Release(m_pDamageBuffer);
	for (int i = 0; i < 10; ++i)
		Safe_Release(m_pDamageTexture[i]);

	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);
}
