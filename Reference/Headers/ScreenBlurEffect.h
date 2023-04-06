#pragma once

#include "Component.h"

BEGIN(Engine)

class CScreenBlurEffect
{
public:
	explicit CScreenBlurEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CScreenBlurEffect();

public:
	void SetBlurAmount(_float fPower) { m_TimeAcc = fPower; };
	virtual void SetBufferSize(int iPosX, int iPosY, int iWidth, int iHeight);
	virtual void EffectApply(ID3D11ShaderResourceView* pMainSRV, class CShader* pShader, _int iPass = 0);

protected:
	_float		m_fX, m_fY, m_fWidth, m_fHeight;
	_float4x4	m_WorldMatrix, m_ViewMatrix, m_ProjMatrix;

	class CVIBuffer_Rect* m_pBuffer = nullptr;

private:
	_float m_TimeAcc = 0.f;
};

END