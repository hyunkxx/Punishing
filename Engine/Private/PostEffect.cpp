#include "..\Public\PostEffect.h"
#include "GameInstance.h"

CPostEffect::CPostEffect(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	m_pBuffer = CVIBuffer_Rect::Create(pDevice, pContext);
}

CPostEffect::~CPostEffect()
{
	Safe_Release(m_pBuffer);
}

void CPostEffect::SetBufferSize(int iPosX, int iPosY, int iWidth, int iHeight)
{
	m_fWidth = iWidth;
	m_fHeight = iHeight;
	m_fX = iWidth >> 1;
	m_fY = iHeight >> 1;

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixScaling(m_fWidth, m_fHeight, 1.f) * XMMatrixTranslation(m_fX - m_fWidth * 0.5f, -m_fY + m_fHeight * 0.5f, 0.f));
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)m_fWidth, (_float)m_fHeight, 0.f, 1.f));

}

void CPostEffect::EffectApply(ID3D11ShaderResourceView * pSRV, CShader * pShader)
{
	if (FAILED(pShader->SetMatrix("g_WorldMatrix", &m_WorldMatrix)))
		return;
	if (FAILED(pShader->SetMatrix("g_ViewMatrix", &m_ViewMatrix)))
		return;
	if (FAILED(pShader->SetMatrix("g_ProjMatrix", &m_ProjMatrix)))
		return;

	if (FAILED(pShader->SetShaderResourceView("g_Buffer", pSRV)))
		return;
	
	if (FAILED(pShader->Begin(0)))
		return;

	if (FAILED(m_pBuffer->Render()))
		return;
}

void CPostEffect::SetShaderResourceView(ID3D11ShaderResourceView * pMainSRV, ID3D11ShaderResourceView * pBloomSRV)
{
	m_pMainResource = pMainSRV;
	m_pBloomResource = pBloomSRV;
}

//void CPostEffect::SetShaderResourceView(PRE_RENDERTARGET eTarget, ID3D11ShaderResourceView * pSRV)
//{
//	switch (eTarget)
//	{
//	case PRE_RENDERTARGET::MAIN:
//		m_pMainResource = pSRV;
//		break;
//	case PRE_RENDERTARGET::BLOOM:
//		m_pBloomResource = pSRV;
//		break;
//	}
//}
