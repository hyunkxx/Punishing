#pragma once

#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CBackGround final : public CGameObject
{
protected:
	CBackGround(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBackGround(const CBackGround& rhs);
	virtual ~CBackGround() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual void RenderGUI() override;

public:
	void SetEnd() { m_bEnd = true; };
	_bool IsEnd() { return m_fEndAcc >= 1.f; };

private:
	HRESULT Add_Components();
	HRESULT Setup_ShaderResources();

public:
	static CBackGround* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

private:
	_float		m_fX, m_fY, m_fWidth, m_fHeight;
	_float4x4	m_WorldMatrix, m_ViewMatrix, m_ProjMatrix;

	_float		m_fRotX, m_fRotY, m_fRotWidth, m_fRotHeight;
	_float4x4	m_RotMatrix;

private:
	CRenderer*		m_pRenderer = { nullptr };
	CShader*		m_pShader = { nullptr };
	CVIBuffer_Rect* m_pVIBuffer = { nullptr };

	CTexture*		m_pGradientMask = nullptr;
	CTexture*		m_pTexture1 = { nullptr };
	CTexture*		m_pTexture2 = { nullptr };
	CTexture*		m_pRotationTexture = { nullptr };//·Îµù

	_float m_fAngle = 0.f;
	_float m_fAlpha = 0.f;
	_bool m_bToggle = true;

	_bool m_bEnd = false;
	_float m_fEndAcc = 0.f;

	_bool bWarningSound = false;
};

END