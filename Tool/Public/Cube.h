#pragma once

#include "GameObject_Tool.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CVIBuffer_Cube;
class CShader;
END

BEGIN(Tool)

class CCube : public CGameObject_Tool
{
protected:
	CCube(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCube(const CCube& rhs);
	virtual ~CCube() = default;

public:
	virtual	HRESULT Initialize_Prototype() override;
	virtual	HRESULT Initialize(void* pArg = nullptr) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual void RenderGUI() override;

public:
	virtual _float PickObject() override;

private:
	HRESULT Add_Components();
	HRESULT Setup_ShaderResources();

public:
	static CCube* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

private:
	CShader*			m_pShader = { nullptr };
	CVIBuffer_Cube*		m_pVIBuffer = { nullptr };

};

END