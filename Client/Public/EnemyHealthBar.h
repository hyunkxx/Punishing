#pragma once

#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CEnemyHealthBar final : public CGameObject
{
protected:
	CEnemyHealthBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEnemyHealthBar(const CEnemyHealthBar& rhs);
	virtual ~CEnemyHealthBar() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual void RenderGUI() override;

public:
	void StartShake() { m_fShakeAcc = 0.f; m_isShake = true; }
	void SetFillAmount(_float fCurHP, _float fMaxHP);
	void SetHealth(_float fCurHP, _float fMaxHP);
	void SetRender(_bool value) { m_bRender = value; }

private:
	HRESULT Add_Components();
	HRESULT Setup_ShaderResources();

public:
	static CEnemyHealthBar* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

private:
	_float		m_fX, m_fY, m_fWidth, m_fHeight;
	_float4x4	m_WorldMatrix, m_ViewMatrix, m_ProjMatrix;

	_float		m_fLineX, m_fLineY, m_fLineWidth, m_fLineHeight;
	_float4x4	m_LineMatrix[3];//0 : x  1 && 2 : 카운트

private:
	CRenderer*		m_pRenderer = { nullptr };
	CShader*		m_pShader = { nullptr };
	CVIBuffer_Rect* m_pVIBuffer = { nullptr };
	CTexture*		m_pTexture = { nullptr };

	CVIBuffer_Rect* m_pVIBufferBlood = { nullptr };
	CVIBuffer_Rect* m_pVIBufferFront = { nullptr };

	CTexture*		m_pTextureBlood = { nullptr };
	CTexture*		m_pTextureFront = { nullptr };

	//체력 라인 수
	CVIBuffer_Rect* m_pHpCountBuffer = { nullptr };
	CTexture*		m_pHpCountTexture[11];

private:
	_bool m_bRender = false;

	_float m_fFill = 1.f;
	_float m_fCurFill = 1.f;

	_int m_iCurHealthCount = 1.f;

	//쉐이크
	_bool m_isShake = false;
	_float m_fShakeX = 0.f;
	_float m_fShakeY = 0.f;
	_float m_fShakeAcc = 0.f;
	const _float m_fShakeTimeOut = 2.5f;
};

END