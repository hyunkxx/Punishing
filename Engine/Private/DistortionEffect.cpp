#include "..\Public\DistortionEffect.h"
#include "GameInstance.h"

CDistortionEffect::CDistortionEffect(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	m_pBuffer = CVIBuffer_Rect::Create(pDevice, pContext);
}

CDistortionEffect::~CDistortionEffect()
{
	Safe_Release(m_pBuffer);
}

void CDistortionEffect::TimeAcc()
{
	CGameInstance* pInstance = CGameInstance::GetInstance();
	m_TimeAcc += pInstance->GetTimer(L"FPS144");
	if (m_TimeAcc >= 10.f)
		m_TimeAcc = 0.f;
}

void CDistortionEffect::SetBufferSize(int iPosX, int iPosY, int iWidth, int iHeight)
{
	m_fWidth = iWidth;
	m_fHeight = iHeight;
	m_fX = iWidth >> 1;
	m_fY = iHeight >> 1;

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixScaling(m_fWidth, m_fHeight, 1.f) * XMMatrixTranslation(m_fX - m_fWidth * 0.5f, -m_fY + m_fHeight * 0.5f, 0.f));
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)m_fWidth, (_float)m_fHeight, 0.f, 0.1f));

}

void CDistortionEffect::EffectApply(ID3D11ShaderResourceView * pMain, ID3D11ShaderResourceView * pDistortion, CShader * pShader)
{
	CGameInstance* pInstance = CGameInstance::GetInstance();
	if (FAILED(pShader->SetMatrix("g_WorldMatrix", &m_WorldMatrix)))
		return;
	if (FAILED(pShader->SetMatrix("g_ViewMatrix", &m_ViewMatrix)))
		return;
	if (FAILED(pShader->SetMatrix("g_ProjMatrix", &m_ProjMatrix)))
		return;

	if (FAILED(pShader->SetShaderResourceView("g_BufferTexture", pMain)))
		return;

	if (FAILED(pShader->SetShaderResourceView("g_DistortionTexture", pDistortion)))
		return;

	if (pShader->SetRawValue("TimeAcc", &m_TimeAcc, sizeof(float)))
		return;

	if (FAILED(pShader->Begin(1)))
		return;
	if (FAILED(m_pBuffer->Render()))
		return;

}
