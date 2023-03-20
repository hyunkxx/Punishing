#pragma once

#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CPlayerHealthBar final : public CGameObject
{
protected:
	CPlayerHealthBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayerHealthBar(const CPlayerHealthBar& rhs);
	virtual ~CPlayerHealthBar() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual void RenderGUI() override;

public:
	void SetFillAmount(_float fCurHP, _float fMaxHP);
	void SetHealth(_float fCurHP, _float fMaxHP);
	
	void SetDashFillAmount(_float fCurDashGage, _float fMaxDashGage);
	void SetDash(_float fCurDashGage, _float fMaxDashGage);

	void SetRender(_bool value) { m_bRender = value; }

private:
	HRESULT Add_Components();
	HRESULT Setup_ShaderResources();

public:
	static CPlayerHealthBar* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

private:
	_float		m_fX, m_fY, m_fWidth, m_fHeight;
	_float4x4	m_WorldMatrix, m_ViewMatrix, m_ProjMatrix;
	_float4x4	m_PlayerBackWorld;
	_float4x4	m_PlayerDashMatrix;

private:
	CRenderer*		m_pRenderer = { nullptr };
	CShader*		m_pShader = { nullptr };
	CVIBuffer_Rect* m_pVIBuffer = { nullptr };
	CTexture*		m_pTexture = { nullptr };

	CTexture*		m_pDashImage = { nullptr };
	CVIBuffer_Rect* m_pDashBuffer = { nullptr };

	CTexture*		m_pPlayerBackImage = { nullptr };
	CVIBuffer_Rect* m_pPlayerBackBuffer = { nullptr };

	CVIBuffer_Rect* m_pVIBufferBlood = { nullptr };
	CVIBuffer_Rect* m_pVIBufferFront = { nullptr };

	CTexture*		m_pTextureBlood = { nullptr };
	CTexture*		m_pTextureFront = { nullptr };

private:
	_bool m_bRender = true;

	_float m_fFill = 1.f;
	_float m_fCurFill = 1.f;

	_int m_iCurHealthCount = 1.f;

	_float m_fCurDashGage = 100.f;
	_float m_fMaxDashGage = 100.f;
	_float m_fDashFill = 1.f;
	_float m_fDashCurFill = 1.f;

};

END