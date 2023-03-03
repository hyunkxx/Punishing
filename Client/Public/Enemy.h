#pragma once

#include "GameObject.h"
#include "Animation.h"

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

class CEnemy final : public CGameObject
{
public:
	explicit CEnemy(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CEnemy(const CEnemy& rhs);
	virtual ~CEnemy() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual void RenderGUI() override;

public:
	_float4 GetPosition();

private:
	HRESULT AddComponents();
	HRESULT SetupShaderResources();

public:
	static CEnemy* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

private:
	CRenderer* renderer = nullptr;
	CTransform* transform = nullptr;
	CModel* model = nullptr;
	CShader* shader = nullptr;
	CCollider* collider = nullptr;

private:
	class CBone* bone = nullptr;

	_uint animation = 0;
};

END