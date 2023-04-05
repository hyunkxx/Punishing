#include "..\Public\BlurEffect.h"
#include "GameInstance.h"

CBlurEffect::CBlurEffect(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	m_pBuffer = CVIBuffer_Rect::Create(pDevice, pContext);

	if (FAILED(m_pDistortionTexture = CTexture::Create(pDevice, pContext, TEXT("../../Resource/Texture/MaskImage/MO.png"))))
		MSG_BOX("텍스쳐 로드 실패");
}

CBlurEffect::~CBlurEffect()
{
	Safe_Release(m_pBuffer);
	Safe_Release(m_pDistortionTexture);
}

void CBlurEffect::TimeAcc(_double TimeDelta)
{
	m_TimeAcc += TimeDelta;
}

void CBlurEffect::SetBufferSize(int iPosX, int iPosY, int iWidth, int iHeight)
{
	m_fWidth = iWidth;
	m_fHeight = iHeight;
	m_fX = iWidth >> 1;
	m_fY = iHeight >> 1;

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixScaling(m_fWidth, m_fHeight, 1.f) * XMMatrixTranslation(m_fX - m_fWidth * 0.5f, -m_fY + m_fHeight * 0.5f, 0.f));
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)m_fWidth, (_float)m_fHeight, 0.f, 1.f));

}

void CBlurEffect::EffectApply(ID3D11ShaderResourceView * pSRV, CShader * pShader)
{
	if (FAILED(pShader->SetMatrix("g_WorldMatrix", &m_WorldMatrix)))
		return;
	if (FAILED(pShader->SetMatrix("g_ViewMatrix", &m_ViewMatrix)))
		return;
	if (FAILED(pShader->SetMatrix("g_ProjMatrix", &m_ProjMatrix)))
		return;

	if (FAILED(pShader->SetShaderResourceView("g_BufferTexture", pSRV)))
		return;

	m_pDistortionTexture->Setup_ShaderResource(pShader, "g_DistortionTexture");

	if (pShader->SetRawValue("TimeAcc", &m_TimeAcc, sizeof(float)))
		return;

	if (FAILED(pShader->Begin(0)))
		return;
	if (FAILED(m_pBuffer->Render()))
		return;

}
