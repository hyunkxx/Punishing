#pragma once

#include "Base.h"

BEGIN(Engine)

class CCustomFont final : public CBase
{
private:
	CCustomFont(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CCustomFont() = default;

public:
	HRESULT Initialize(const _tchar* pFontName);
	void Render(const _tchar * pText, const _float2 & vPosition, _fvector vColor, _float fRadian, const _float2 & vOrigin, const _float2 & vScale);

private:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };

private:
	SpriteFont*			m_pFont = { nullptr };
	SpriteBatch*		m_pBatch = { nullptr };

public:
	static CCustomFont* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFontName);
	virtual void Free() override;
};

END