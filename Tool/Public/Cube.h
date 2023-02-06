#pragma once

#include "GameObject.h"

BEGIN(Engine)
class CVIBuffer_Cube;
class CTransform;
class CRenderer;
class CTexture;
class CShader;
END

BEGIN(Tool)

class CCube : public CGameObject
{
protected:
	CCube(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCube(const CCube& rhs);
	virtual ~CCube() = default;

public:
	virtual	HRESULT Initialize_Prototype() override;
	virtual	HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual void RenderGUI() override;

public:
	_uint GetID() const { return m_iID;	}
	_float Picking();
	_bool IsSelect() { return m_IsSelect; }
	void Select() { m_IsSelect = true; }
	void UnSelect() { m_IsSelect = false; }

private:
	HRESULT Add_Components();
	HRESULT Setup_ShaderResources();

public:
	static CCube* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

private:
	CShader*			m_pShader = { nullptr };
	CRenderer*			m_pRenderer = { nullptr };
	CTransform*			m_pTransform = { nullptr };
	CVIBuffer_Cube*		m_pVIBuffer = { nullptr };

private:
	static _uint m_iID;
	bool m_IsSelect = false;

};

END