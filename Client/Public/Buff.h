#pragma once

#include "GameObject.h"

BEGIN(Engine)
class CTexture;
class CRenderer;
class CTransform;
class CShader;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CBuff final : public CGameObject
{
public:
	explicit CBuff(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CBuff(const CBuff& rhs);
	virtual ~CBuff() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual void RenderGUI() override;
	//virtual _float GetLengthFromCamera() override;
public:
	void StartEffect(_fvector vPosition, _float fSpeed, _float4 vDir);
	void SetPosition(_fvector vPosition);
	void SetPlayerTransform(CTransform* pTransform) { m_pPlayerTransform = pTransform; };

private:
	HRESULT AddComponent();

public:
	static CBuff* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

private:
	CRenderer* m_pRenderer = nullptr;
	CTransform* m_pTransform = nullptr;
	CShader* m_pShader = nullptr;
	CTexture* m_pDiffuseTexture = nullptr;
	CVIBuffer_Rect* m_pVIBufferRect = nullptr;

	CTransform* m_pPlayerTransform = nullptr;
	_bool m_bRender = true;
	_float m_fAcc = 1.5f;
	_float m_fSpeed = 1.5f;
	_float m_fHeight = 0.0f;
	_float4 m_vDir;
};

END