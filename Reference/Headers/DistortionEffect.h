#pragma once

#include "Component.h"

BEGIN(Engine)

class CDistortionEffect
{
public:
	explicit CDistortionEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CDistortionEffect();

public:
	void TimeAcc();
	virtual void SetBufferSize(int iPosX, int iPosY, int iWidth, int iHeight);
	void EffectApply(ID3D11ShaderResourceView * pMain, ID3D11ShaderResourceView * pDistortion, class CShader * pShader);

protected:
	_float		m_fX, m_fY, m_fWidth, m_fHeight;
	_float4x4	m_WorldMatrix, m_ViewMatrix, m_ProjMatrix;

	class CVIBuffer_Rect* m_pBuffer = nullptr;

private:
	_float m_TimeAcc = 0.f;
};

END