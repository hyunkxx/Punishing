#pragma once

#include "Component.h"

BEGIN(Engine)

class CShadow
{
public:
	explicit CShadow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CShadow();

public:
	virtual void SetBufferSize(int iPosX, int iPosY, int iWidth, int iHeight);
	void EffectApply(ID3D11ShaderResourceView * pMain, ID3D11ShaderResourceView * pDepth, ID3D11ShaderResourceView * pShadowDepth, class CShader * pShader);

protected:
	_float		m_fX, m_fY, m_fWidth, m_fHeight;
	_float4x4	m_WorldMatrix, m_ViewMatrix, m_ProjMatrix;

	class CVIBuffer_Rect* m_pBuffer = nullptr;

};

END