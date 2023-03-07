#pragma once

#include "GameObject.h"
#include "Animation.h"
#include "Collider.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CModel;
class CBone;
class CShader;
class CTimer;
class CCollider;

END

BEGIN(Client)
class CCharacter;

class CBoss final : public CGameObject
{
public:
	explicit CBoss(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CBoss(const CBoss& rhs);
	virtual ~CBoss() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual void RenderGUI() override;

private:
	HRESULT AddComponents();
	HRESULT SetupShaderResources();

public:
	static CBoss* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

private:
	CRenderer* renderer = nullptr;
	CTransform* transform = nullptr;
	CModel* model = nullptr;
	CShader* shader = nullptr;
	CCollider* collider = nullptr;

};

END