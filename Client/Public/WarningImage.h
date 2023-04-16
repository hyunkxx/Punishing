#pragma once

#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CWarningImage final : public CGameObject
{
public:
	explicit CWarningImage(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CWarningImage(const CWarningImage& rhs);
	virtual ~CWarningImage() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual void RenderGUI() override;

public:
	void SetStartEffect(CTransform* pOwnerTransform, float fHeight);

private:
	HRESULT Add_Components();
	HRESULT Setup_ShaderResources();

public:
	static CWarningImage* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

	CTransform* m_pTransform = nullptr;
	CRenderer* m_pRenderer = nullptr;
	CShader* m_pShader = nullptr;
	CTexture* m_pTexture = nullptr;
	CVIBuffer_Rect* m_pVIBuffer = nullptr;
		
	CTransform* m_pOwnerTransform = nullptr;

	_float m_fAlpha = 1.f;
	_float m_fOriginScaleY = 1.f;
	_float m_fCurrentScaleY = 1.f;
	_bool m_bActive = false;

	_float m_fHeight = 1.3f;
};

END