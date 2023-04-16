#include "..\Public\Shadow.h"
#include "GameInstance.h"

CShadow::CShadow(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	m_pBuffer = CVIBuffer_Rect::Create(pDevice, pContext);
}

CShadow::~CShadow()
{
	Safe_Release(m_pBuffer);
}

void CShadow::SetBufferSize(int iPosX, int iPosY, int iWidth, int iHeight)
{
	m_fWidth = iWidth;
	m_fHeight = iHeight;
	m_fX = iWidth >> 1;
	m_fY = iHeight >> 1;

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixScaling(m_fWidth, m_fHeight, 1.f) * XMMatrixTranslation(m_fX - m_fWidth * 0.5f, -m_fY + m_fHeight * 0.5f, 0.f));
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)m_fWidth, (_float)m_fHeight, 0.f, 0.1f));

}

void CShadow::EffectApply(ID3D11ShaderResourceView * pMain, ID3D11ShaderResourceView * pDepth, ID3D11ShaderResourceView * pShadowDepth, CShader * pShader)
{
	CGameInstance* pInstance = CGameInstance::GetInstance();
	if (FAILED(pShader->SetMatrix("g_WorldMatrix", &m_WorldMatrix)))
		return;
	if (FAILED(pShader->SetMatrix("g_ViewMatrix", &m_ViewMatrix)))
		return;
	if (FAILED(pShader->SetMatrix("g_ProjMatrix", &m_ProjMatrix)))
		return;

	//역변환을 위한 역행렬들
	if (FAILED(pShader->SetMatrix("g_ViewInv", &pInstance->Get_Transform_float4x4_Inverse(CPipeLine::TS_VIEW))))
		return;
	if (FAILED(pShader->SetMatrix("g_ProjInv", &pInstance->Get_Transform_float4x4_Inverse(CPipeLine::TS_PROJ))))
		return;
	//if (FAILED(pShader->SetMatrix("g_ViewInv", &pInstance->GetLightInverseFloat4x4(LIGHT_MATRIX::LIGHT_VIEW))))
	//	return;
	//if (FAILED(pShader->SetMatrix("g_ProjInv", &pInstance->GetLightInverseFloat4x4(LIGHT_MATRIX::LIGHT_PROJ))))
	//	return;

	//조명기준의 뷰와 투영행렬
	if (FAILED(pShader->SetMatrix("g_LightViewMatrix", &pInstance->GetLightFloat4x4(LIGHT_MATRIX::LIGHT_VIEW))))
		return;
	if (FAILED(pShader->SetMatrix("g_LightProjMatrix", &pInstance->GetLightFloat4x4(LIGHT_MATRIX::LIGHT_PROJ))))
		return;

	//메인, 깊이, 쉐도우맵
	if (FAILED(pShader->SetShaderResourceView("g_MainTexture", pMain)))
		return;
	if (FAILED(pShader->SetShaderResourceView("g_DepthTexture", pDepth)))
		return;
	if (FAILED(pShader->SetShaderResourceView("g_ShadowDepthTexture", pShadowDepth)))
		return;

	if (FAILED(pShader->Begin(0)))
		return;
	if (FAILED(m_pBuffer->Render()))
		return;
}
