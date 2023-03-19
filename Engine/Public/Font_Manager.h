#pragma once

#include "Base.h"

BEGIN(Engine)

class CFont_Manager final : public CBase
{
	DECLARE_SINGLETON(CFont_Manager)
private:
	CFont_Manager();
	virtual ~CFont_Manager() = default;

public:
	HRESULT Add_Font(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFontTag, const _tchar* pFontName);
	void Render_Font(const _tchar* pFontTag, const _tchar* pText, const _float2 & vPosition, _fvector vColor = XMVectorSet(1.f, 1.f, 1.f, 1.f), const _float2 & vScale = _float2(1.f, 1.f), _float fRadian = 0.f, const _float2 & vOrigin = _float2(0.f, 0.f));

private:
	unordered_map<const _tchar*, class CCustomFont*>			m_Fonts;

private:
	class CCustomFont* Find_Font(const _tchar* pFontTag);

public:
	virtual void Free() override;
};

END