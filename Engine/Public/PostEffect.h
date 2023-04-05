#pragma once

#include "Component.h"

BEGIN(Engine)

class CPostEffect
{
public:
	explicit CPostEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CPostEffect();

public:
	virtual void SetBufferSize(int iPosX, int iPosY, int iWidth, int iHeight);
	virtual void EffectApply(ID3D11ShaderResourceView * pSRV, class CShader* pShader);

private:
	void SetShaderResourceView(ID3D11ShaderResourceView* pMainSRV, ID3D11ShaderResourceView* pBloomSRV);

protected:
	_float		m_fX, m_fY, m_fWidth, m_fHeight;
	_float4x4	m_WorldMatrix, m_ViewMatrix, m_ProjMatrix;

	class CVIBuffer_Rect* m_pBuffer = nullptr;

	ID3D11ShaderResourceView* m_pMainResource = nullptr;
	ID3D11ShaderResourceView* m_pBloomResource = nullptr;
};

END