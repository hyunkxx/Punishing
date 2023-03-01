#pragma once

#include "Collider.h"

BEGIN(Engine)

class ENGINE_DLL CSphereCollider final : public CCollider
{
protected:
	explicit CSphereCollider(ID3D11Device* device, ID3D11DeviceContext* context);
	explicit CSphereCollider(const CSphereCollider& rhs);
	virtual ~CSphereCollider() = default;

public:
	virtual HRESULT InitializePrototype(ID3D11Device* device, ID3D11DeviceContext* context) override;
	virtual HRESULT Initialize(void* arg = nullptr) override;

	virtual void Update() override;
	virtual _bool Collision(CCollider* targetCollider) override;
	virtual void Render() override;

public:
	static CSphereCollider* Create(ID3D11Device* device, ID3D11DeviceContext* context);
	virtual CComponent* Clone(void* arg) override;
	virtual void Free() override;

	BoundingSphere* _sphere = nullptr;
	BoundingSphere* _sphereOriginal = nullptr;

};

END