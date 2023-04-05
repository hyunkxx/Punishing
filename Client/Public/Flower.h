#pragma once

#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Rect_Instance;
END

BEGIN(Client)

class CFlower final : public CGameObject
{
public:
	explicit CFlower(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CFlower(const CFlower& rhs);
	virtual ~CFlower() = default;

public:
	virtual	HRESULT Initialize_Prototype() override;
	virtual	HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CFlower* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

private:
	CShader* m_pShader = nullptr;
	CTexture* m_pTexture = nullptr;
	CRenderer* m_pRenderer = nullptr;
	CTransform*	m_pTransform = nullptr;
	CVIBuffer_Rect_Instance* m_pVIBuffer = nullptr;

	CTransform* m_pPlayerTransform = nullptr;

};

END