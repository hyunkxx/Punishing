#pragma once

#include "GameObject.h"

BEGIN(Engine)
class CTransform;
class CRenderer;
class CShader;
class CModel;
END

BEGIN(Client)

class CBossRoom final : public CGameObject
{
private:
	CBossRoom(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CBossRoom() = default;

public:
	virtual	HRESULT Initialize_Prototype() override;
	virtual	HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Add_Components();
	HRESULT Setup_ShaderResources();

public:
	static CBossRoom* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

private:
	CTransform*	m_pTransform = { nullptr };
	CRenderer*	m_pRenderer = { nullptr };
	CShader*	m_pShader = { nullptr };
	CModel*		m_pModel = { nullptr };

};

END