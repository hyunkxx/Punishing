#include "pch.h"
#include "..\Public\PlayerIcon.h"

#include "ApplicationManager.h"
#include "GameInstance.h"
#include "Character.h"
#include "Boss.h"

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

	//공격 대쉬 버튼
	m_fWidth = 90.f;
	m_fHeight = 90.f;
	m_fX = g_iWinSizeX;
	m_fY = g_iWinSizeY - 120.f;

	//변신 버튼
	m_fEvolutionButtonX = m_fX - 100.f;
	m_fEvolutionButtonY = m_fY;
	m_fEvolutionButtonXWidth = 90.f;
	m_fEvolutionButtonXHeight = 90.f;

	//락온 타겟
	m_fTargetWidth = 128.f;
	m_fTargetHeight = 72.f;

	//콤보 숫자
	m_fComboNumX = m_fOriginComboNumX = 150.f;
	m_fComboNumY = g_iWinSizeY >> 1;
	m_fComboNumWidth = NUM_SIZE_X;
	m_fComboNumHeight = 62.f;

	//콤보 이미지
	m_fComboImageWidth = 100.f;
	m_fComboImageHeight = 30.f;

	//콤보 게이지
	m_fComboGageX = 310.f;
	m_fComboGageY = (g_iWinSizeY >> 1) + 40.f;
	m_fComboGageWidth = 250.f;
	m_fComboGageHeight = 3.f;

	//변신 게이지
	m_fEvolutionGageWidth = 300.f;
	m_fEvolutionGageHeight = 40.f;
	m_fEvolutionGageX = g_iWinSizeX  >> 1;
	m_fEvolutionGageY = g_iWinSizeY - 100.f;

	XMStoreFloat4x4(&m_EvolutionMatrix, XMMatrixScaling(m_fEvolutionGageWidth, m_fEvolutionGageHeight, 1.f) * XMMatrixTranslation(m_fEvolutionGageX - g_iWinSizeX * 0.5f, -m_fEvolutionGageY + g_iWinSizeY * 0.5f, 0.f));
	XMStoreFloat4x4(&m_AttackMatrix, XMMatrixScaling(m_fWidth, m_fHeight, 1.f) * XMMatrixTranslation(((m_fX - g_iWinSizeX)) * 0.5f, ((-m_fY + g_iWinSizeY)) * 0.5f, 0.f));

	XMStoreFloat4x4(&m_DashBackMatrix, XMMatrixScaling(m_fWidth + 5.f, m_fHeight + 5.f, 1.f) * XMMatrixTranslation(m_fX - 70.f - g_iWinSizeX * 0.5f, -m_fY - 50.f + g_iWinSizeY * 0.5f, 0.f));
	XMStoreFloat4x4(&m_DashRotMatrix, XMMatrixScaling(m_fWidth, m_fHeight, 1.f) * XMMatrixTranslation(m_fX - 70.f - g_iWinSizeX * 0.5f, -m_fY - 50.f + g_iWinSizeY * 0.5f, 0.f));
	XMStoreFloat4x4(&m_DashMatrix, XMMatrixScaling(m_fWidth - 10.f, m_fHeight - 10.f, 1.f) * XMMatrixTranslation(m_fX - 70.f - g_iWinSizeX * 0.5f, -m_fY - 50.f + g_iWinSizeY * 0.5f, 0.f));

	XMStoreFloat4x4(&m_EvolutionBackMatrix, XMMatrixScaling(m_fWidth + 5.f, m_fHeight + 5.f, 1.f) * XMMatrixTranslation(m_fEvolutionButtonX - 80.f - g_iWinSizeX * 0.5f, -m_fY - 50.f + g_iWinSizeY * 0.5f, 0.f));
	XMStoreFloat4x4(&m_EvolutionRotMatrix, XMMatrixScaling(m_fWidth , m_fHeight , 1.f) * XMMatrixTranslation(m_fEvolutionButtonX - 80.f - g_iWinSizeX * 0.5f, -m_fY - 50.f + g_iWinSizeY * 0.5f, 0.f));
	XMStoreFloat4x4(&m_EvolutionButtonMatrix, XMMatrixScaling(m_fWidth - 10.f, m_fHeight - 10.f, 1.f) * XMMatrixTranslation(m_fEvolutionButtonX - 80.f - g_iWinSizeX * 0.5f, -m_fY - 50.f + g_iWinSizeY * 0.5f, 0.f));

	XMStoreFloat4x4(&m_ComboGageMatrix, XMMatrixScaling(m_fComboGageWidth, m_fComboGageHeight, 1.f) * XMMatrixTranslation(m_fComboGageX - g_iWinSizeX * 0.5f, -m_fComboGageY + g_iWinSizeY * 0.5f, 0.f));

	for(int i = 0 ; i < 4 ; ++i)
		XMStoreFloat4x4(&m_ComboNumberMatrix[i], XMMatrixScaling(m_fComboNumWidth, m_fComboNumHeight, 1.f) * XMMatrixTranslation(m_fOriginComboNumX - g_iWinSizeX * 0.5f, -m_fComboNumY + g_iWinSizeY * 0.5f, 0.f));

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));
	
	return S_OK;
}

void CPlayerIcon::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	m_fAngleEvolution -= TimeDelta;
	_matrix RotationMatrix = XMMatrixRotationZ(m_fAngleEvolution);
	XMStoreFloat4x4(&m_DashRotMatrix, XMMatrixScaling(m_fWidth, m_fHeight, 1.f) * RotationMatrix * XMMatrixTranslation(m_fX - 70.f - g_iWinSizeX * 0.5f, -m_fY - 50.f + g_iWinSizeY * 0.5f, 0.f));
	XMStoreFloat4x4(&m_EvolutionRotMatrix, XMMatrixScaling(m_fWidth, m_fHeight, 1.f) * RotationMatrix * XMMatrixTranslation(m_fEvolutionButtonX - 80.f - g_iWinSizeX * 0.5f, -m_fY - 50.f + g_iWinSizeY * 0.5f, 0.f));


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

			if (pBoss->LockOnRelease())
				m_bTargetImageRender = false;
			else
			{
				if (pBoss && pBoss->IsSpawned())
				{
					if (fLength >= 20.f)
						m_bTargetImageRender = false;
					else
					{
						m_bTargetImageRender = true;
						m_fTargetX = (vTargetPos.x + 1) * g_iWinSizeX * 0.5f;
						m_fTargetY = (1 - vTargetPos.y) * g_iWinSizeY * 0.5f + g_iWinSizeY * 0.5f;
						XMStoreFloat4x4(&m_TargetMatrix, XMMatrixScaling(m_fTargetWidth, m_fTargetHeight, 1.f) * XMMatrixTranslation(m_fTargetX - g_iWinSizeX * 0.5f, -m_fTargetY + g_iWinSizeY * 0.5f, 0.0f));
					}
				}
			}

		}
		else
		{
			m_bTargetImageRender = true;
			m_fTargetX = (vTargetPos.x + 1) * g_iWinSizeX * 0.5f;
			m_fTargetY = (1 - vTargetPos.y) * g_iWinSizeY * 0.5f + g_iWinSizeY * 0.5f;
			XMStoreFloat4x4(&m_TargetMatrix, XMMatrixScaling(m_fTargetWidth, m_fTargetHeight, 1.f) * XMMatrixTranslation(m_fTargetX - g_iWinSizeX * 0.5f, -m_fTargetY + g_iWinSizeY * 0.5f, 0.0f));
		}
		
	}
	else
		m_bTargetImageRender = false;

	CGameInstance* pInstance = CGameInstance::GetInstance();
}

void CPlayerIcon::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	CGameInstance* pInstance = CGameInstance::GetInstance();

	if (m_isShake)
	{
		m_fShakeAcc += TimeDelta * 10.f;

		if (m_fShakeAcc >= m_fShakeTimeOut)
		{
			m_isShake = false;
			m_fShakeAcc = 0.f;
			m_fShakeX = 0.f;
			m_fShakeY = 0.f;
		}

		m_fShakeX = 4.5f * sin(m_fShakeAcc * 10.f) * powf(0.5f, m_fShakeAcc);
		m_fShakeY = 4.5f * sin(m_fShakeAcc * 10.f) * powf(0.5f, m_fShakeAcc);
	}

	string strCombo = to_string(m_pPlayer->GetComboCount());
	if (m_pPlayer->GetComboCount() == 0)
	{
		m_fFill = 1.f;
		m_bComboRender = false;
	}
	else
	{
		m_bComboRender = true;
		switch (strCombo.size())
		{
		case 0:
			m_fComboNumX = m_fOriginComboNumX + 205;
			break;
		case 1:
			m_fComboNumX = m_fOriginComboNumX + 205 - NUM_SIZE_X;
			break;
		case 2:
			m_fComboNumX = m_fOriginComboNumX + 205 - NUM_SIZE_X * 2.f;
			break;
		case 3:
			m_fComboNumX = m_fOriginComboNumX + 205 - NUM_SIZE_X * 3.f;
			break;
		case 4:
			m_fComboNumX = m_fOriginComboNumX + 205 - NUM_SIZE_X * 4.f;
			break;
		default:
			break;
		}

		for (int i = 0; i < 4; ++i)
			XMStoreFloat4x4(&m_ComboNumberMatrix[i], XMMatrixScaling(m_fComboNumWidth, m_fComboNumHeight, 1.f) * XMMatrixTranslation(m_fComboNumX + m_fShakeX + (i * NUM_SIZE_X) - g_iWinSizeX * 0.5f, -m_fComboNumY + m_fShakeY + g_iWinSizeY * 0.5f, 0.f));

		m_fComboImageX = m_fOriginComboNumX + 205 + 30.f;
		XMStoreFloat4x4(&m_ComboMatrix, XMMatrixScaling(m_fComboImageWidth, m_fComboImageHeight, 1.f) * XMMatrixTranslation(m_fComboImageX + m_fShakeX - g_iWinSizeX * 0.5f, -m_fComboNumY + m_fShakeY - 20.f + g_iWinSizeY * 0.5f, 0.f));

		m_fFill = (m_fComboTimeOut - m_fComboCurTime) / m_fComboTimeOut;
	}

	_float fCurEvolutionTimeAcc = m_pPlayer->GetCurEvolutionTime();
	_float fEvolutionTimeOut = m_pPlayer->GetEvolutionTime();

	m_fEvolutionGageFill = (fEvolutionTimeOut - fCurEvolutionTimeAcc) / fEvolutionTimeOut;
	
	//이미지 넘기기
	m_fImageAcc += TimeDelta;
	if (m_fImageAcc >= 0.08f)
	{
		if (m_iCurrentIndex >= 137)
			m_iCurrentIndex = 0;
		else
			m_iCurrentIndex++;
	
		m_fImageAcc = 0.f;
	}

	//깜빡깜빡
	if (m_pPlayer->IsEvolutionReady())
	{
		if (m_bMaxAlpha)
		{
			m_fEvolutionAlpha -= TimeDelta * 2.f;
			if (m_fEvolutionAlpha <= 0.f)
			{
				m_bMaxAlpha = false;
				m_fEvolutionAlpha = 0.f;
			}
		}
		else
		{
			m_fEvolutionAlpha += TimeDelta * 2.f;
			if (m_fEvolutionAlpha >= 1.f)
			{
				m_bMaxAlpha = true;
				m_fEvolutionAlpha = 1.f;
			}
		}
	}

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

	////공격 버튼 세팅
	//if(m_pPlayer->IsAttackalbe())
	//	m_pShader->Begin(0);
	//else
	//	m_pShader->Begin(2);

	//m_pBackVIBuffer->Render();

	//if (FAILED(m_pAttackTexture->Setup_ShaderResource(m_pShader, "g_Texture")))
	//	return E_FAIL;

	//if (m_pPlayer->IsAttackalbe())
	//	m_pShader->Begin(0);
	//else
	//	m_pShader->Begin(2);
	//m_pAttackVIBuffer->Render();

	//대쉬 버튼 세팅
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_DashBackMatrix)))
		return E_FAIL;
	if (FAILED(m_pBackTexture->Setup_ShaderResource(m_pShader, "g_Texture")))
		return E_FAIL;

	if (m_pPlayer->IsDashable() && m_pPlayer->IsDashGageFull())
		m_pShader->Begin(0);
	else
		m_pShader->Begin(2);

	//초간공간 사용불가시 이거
	//if (!CApplicationManager::GetInstance()->IsFreezeReady())
	//	m_pShader->Begin(3);

	m_pBackVIBuffer->Render();

	if (FAILED(m_pRotationTexture->Setup_ShaderResource(m_pShader, "g_Texture")))
		return E_FAIL;

	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_DashRotMatrix)))
		return E_FAIL;
	m_pShader->Begin(0);
	m_pDashVIBuffer->Render();

	if (FAILED(m_pDashTexture->Setup_ShaderResource(m_pShader, "g_Texture")))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_DashMatrix)))
		return E_FAIL;
	m_pShader->Begin(0);
	m_pDashVIBuffer->Render();

	//타겟 락인
	if (m_bTargetImageRender)
	{
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_TargetMatrix)))
			return E_FAIL;

		if (FAILED(m_pTargetTexture->Setup_ShaderResource(m_pShader, "g_Texture", m_iCurrentIndex)))
			return E_FAIL;

		m_pShader->Begin(4);
		m_pTargetVIBuffer->Render();
	}

	//콤보 숫자
	if (m_bComboRender)
	{
		for (int i = 0; i < to_string(m_pPlayer->GetComboCount()).size(); ++i)
		{
			if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_ComboNumberMatrix[i])))
				return E_FAIL;

			if (FAILED(ComputeComboToTexture(i)->Setup_ShaderResource(m_pShader, "g_Texture")))
				return E_FAIL;

			m_pShader->Begin(0);
			m_pComboNumberBuffer->Render();
		}
		//콤보 이미지
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_ComboMatrix)))
			return E_FAIL;

		if (FAILED(m_pComboTexture->Setup_ShaderResource(m_pShader, "g_Texture")))
			return E_FAIL;

		m_pShader->Begin(0);
		m_pComboBuffer->Render();

		//콤보 게이지
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_ComboGageMatrix)))
			return E_FAIL;

		if (FAILED(m_pComboGageTexture->Setup_ShaderResource(m_pShader, "g_Texture")))
			return E_FAIL;

		if (FAILED(m_pShader->SetRawValue("g_FillAmount", &m_fFill, sizeof(_float))))
			return E_FAIL;

		m_pShader->Begin(0);
		m_pComboGageBuffer->Render();
	}

	//변신 게이지
	if (m_pPlayer->IsEvolution())
	{
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_EvolutionMatrix)))
			return E_FAIL;
		if (FAILED(m_pEvolutionGageBackTexture->Setup_ShaderResource(m_pShader, "g_Texture")))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_FillAmount", &fValue, sizeof(_float))))
			return E_FAIL;

		m_pShader->Begin(0);
		m_pEvolutionGageBuffer->Render();

		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_EvolutionMatrix)))
			return E_FAIL;
		if (FAILED(m_pEvolutionGageFrontTexture->Setup_ShaderResource(m_pShader, "g_Texture")))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_FillAmount", &m_fEvolutionGageFill, sizeof(_float))))
			return E_FAIL;

		m_pShader->Begin(0);
		m_pEvolutionGageBuffer->Render();
	}

	if (m_pPlayer->IsEvolutionReady())
	{
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_EvolutionBackMatrix)))
			return E_FAIL;
		if (FAILED(m_pBackTexture->Setup_ShaderResource(m_pShader, "g_Texture")))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_FillAmount", &fValue, sizeof(_float))))
			return E_FAIL;

		m_pShader->Begin(0);
		m_pEvolutionButtonBuffer->Render();

		//회전
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_EvolutionRotMatrix)))
			return E_FAIL;
		if (FAILED(m_pRotationTexture->Setup_ShaderResource(m_pShader, "g_Texture")))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_FillAmount", &fValue, sizeof(_float))))
			return E_FAIL;

		m_pShader->Begin(0);
		m_pEvolutionButtonBuffer->Render();

		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_EvolutionButtonMatrix)))
			return E_FAIL;
		if (FAILED(m_pEvolutionButtonTexture->Setup_ShaderResource(m_pShader, "g_Texture")))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_FillAmount", &m_fEvolutionGageFill, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_Alpha", &m_fEvolutionAlpha, sizeof(_float))))
			return E_FAIL;

		m_pShader->Begin(0);
		m_pEvolutionButtonBuffer->Render();
	}

	return S_OK;
}

void CPlayerIcon::RenderGUI()
{
}

CTexture* CPlayerIcon::ComputeComboToTexture(int iIndex)
{
	string strCombo = to_string(m_pPlayer->GetComboCount());
	int index = strCombo[iIndex] - '0';
	return m_pComboNumberTexture[index];
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

	_tchar szImageTag[MAX_PATH] = L"";
	wsprintfW(szImageTag, L"tagetimage");
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("proto_com_targetimage"),
		szImageTag, (CComponent**)&m_pTargetTexture)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("proto_com_texture_rot_icon"), TEXT("com_texture_rot"), (CComponent**)&m_pRotationTexture)))
		return E_FAIL;

	//기존 단일 타겟이미지
	//if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("proto_com_texture_target"),
	//	TEXT("com_texture_target"), (CComponent**)&m_pTargetTexture)))
	//	return E_FAIL;

	//콤보 숫자
	wstring strComboNumBufferName = L"com_vibuffer_combo_num_" + to_wstring(0);
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("proto_com_vibuffer_rect"),
		strComboNumBufferName.c_str(), (CComponent**)&m_pComboNumberBuffer)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("proto_com_texture_0"),
		TEXT("com_texture_0"), (CComponent**)&m_pComboNumberTexture[0])))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("proto_com_texture_1"),
		TEXT("com_texture_1"), (CComponent**)&m_pComboNumberTexture[1])))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("proto_com_texture_2"),
		TEXT("com_texture_2"), (CComponent**)&m_pComboNumberTexture[2])))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("proto_com_texture_3"),
		TEXT("com_texture_3"), (CComponent**)&m_pComboNumberTexture[3])))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("proto_com_texture_4"),
		TEXT("com_texture_4"), (CComponent**)&m_pComboNumberTexture[4])))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("proto_com_texture_5"),
		TEXT("com_texture_5"), (CComponent**)&m_pComboNumberTexture[5])))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("proto_com_texture_6"),
		TEXT("com_texture_6"), (CComponent**)&m_pComboNumberTexture[6])))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("proto_com_texture_7"),
		TEXT("com_texture_7"), (CComponent**)&m_pComboNumberTexture[7])))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("proto_com_texture_8"),
		TEXT("com_texture_8"), (CComponent**)&m_pComboNumberTexture[8])))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("proto_com_texture_9"),
		TEXT("com_texture_9"), (CComponent**)&m_pComboNumberTexture[9])))
		return E_FAIL;

	//Combo 이미지
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("proto_com_vibuffer_rect"),
		TEXT("com_buffer_combo"), (CComponent**)&m_pComboBuffer)))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("proto_com_texture_combo"),
		TEXT("com_texture_combo"), (CComponent**)&m_pComboTexture)))
		return E_FAIL;

	//Combo Gage
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("proto_com_vibuffer_rect"),
		TEXT("com_buffer_combogage"), (CComponent**)&m_pComboGageBuffer)))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("proto_com_texture_combo_gage"),
		TEXT("com_texture_combogage"), (CComponent**)&m_pComboGageTexture)))
		return E_FAIL;

	//Evolution Gage
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("proto_com_vibuffer_rect"),
		TEXT("com_buffer_evolution"), (CComponent**)&m_pEvolutionGageBuffer)))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("proto_com_texture_evolution_back"),
		TEXT("com_texture_evolution_back"), (CComponent**)&m_pEvolutionGageBackTexture)))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("proto_com_texture_evolution_front"),
		TEXT("com_texture_evolution_front"), (CComponent**)&m_pEvolutionGageFrontTexture)))
		return E_FAIL;

	//Evolution Button
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("proto_com_vibuffer_rect"),
		TEXT("com_buffer_evolution_icon"), (CComponent**)&m_pEvolutionButtonBuffer)))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("proto_com_texture_evolution_icon"),
		TEXT("com_texture_evolution_icon"), (CComponent**)&m_pEvolutionButtonTexture)))
		return E_FAIL;

	//for (int i = 0; i < 10; ++i)
	//{
	//	//wstring strProtoComboNumTexture = L"proto_com_texture_" + to_wstring(i);
	//	//wstring strComboTextureName = L"com_texture_" + to_wstring(i);
	//	_tchar szProtoBuff[64] = L"";
	//	_tchar szComBuff[64] = L"";
	//	wsprintf(szProtoBuff, L"proto_com_texture_%d", i);
	//	wsprintf(szComBuff, L"com_texture_%d", i);
	//	if (FAILED(__super::Add_Component(LEVEL_STATIC, szProtoBuff,
	//		szComBuff, (CComponent**)&m_pComboNumberTexture[i])))
	//		return E_FAIL;
	//}

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

	Safe_Release(m_pComboGageBuffer);
	Safe_Release(m_pComboGageTexture);
	Safe_Release(m_pComboNumberBuffer);
	for(int i = 0 ; i < 10 ; ++i)
		Safe_Release(m_pComboNumberTexture[i]);

	Safe_Release(m_pTargetTexture);

	Safe_Release(m_pComboBuffer);
	Safe_Release(m_pComboTexture);
	Safe_Release(m_pBackVIBuffer);
	Safe_Release(m_pBackTexture);
	Safe_Release(m_pAttackVIBuffer);
	Safe_Release(m_pAttackTexture);
	Safe_Release(m_pDashVIBuffer);
	Safe_Release(m_pDashTexture);
	Safe_Release(m_pEvolutionVIBuffer);
	Safe_Release(m_pEvolutionTexture);
	Safe_Release(m_pTargetVIBuffer);
	

	Safe_Release(m_pEvolutionGageBuffer);
	Safe_Release(m_pEvolutionGageBackTexture);
	Safe_Release(m_pEvolutionGageFrontTexture);

	Safe_Release(m_pEvolutionButtonBuffer);
	Safe_Release(m_pEvolutionButtonTexture);

	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);
}
