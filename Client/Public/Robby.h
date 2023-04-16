#pragma once

#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CRobby final : public CGameObject
{
protected:
	CRobby(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRobby(const CRobby& rhs);
	virtual ~CRobby() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual void RenderGUI() override;

private:
	HRESULT Add_Components();
	HRESULT Setup_ShaderResources();

public:
	static CRobby* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

private:
	_float		m_fX, m_fY, m_fWidth, m_fHeight;
	_float4x4	m_WorldMatrix, m_ViewMatrix, m_ProjMatrix;

	_float		m_LogofX, m_LogofY, m_LogoW, m_LogoH;
	_float4x4	m_LogoMatrix;

private:
	CRenderer*		m_pRenderer = { nullptr };
	CShader*		m_pShader = { nullptr };
	CVIBuffer_Rect* m_pVIBuffer = { nullptr };
	
	CTexture*		m_pRobbyLoadingTexture = { nullptr };
	CTexture*		m_pGradientMask = { nullptr };
	CTexture*		m_pLogoTexture = nullptr;

	_float m_fAngle = 0.f;

	_bool m_bOpen = true;
	_bool m_bWait = true;
	_float m_fTimeAcc = 0.f;
	_float m_fAlphaAcc = 0.f;
	_uint m_iCurrentIndex = 0;
	_float m_fPageAcc = 0.f;
};

END