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

	virtual void Update(_matrix transformMatrix) override;
	virtual _bool Collision(CCollider* targetCollider) override;
	virtual void Render() override;

	virtual _float3 GetCenter() const { return _collDesc.vCenter; }
	virtual _float3 GetRotation() const { return _collDesc.vRotation; }
	virtual _float3 GetExtents() const { return _collDesc.vExtents; }
	virtual void SetExtents(_float3 vExtents);
	virtual void SetRotation(_float3 vRotation);

public:
	static CSphereCollider* Create(ID3D11Device* device, ID3D11DeviceContext* context);
	virtual CComponent* Clone(void* arg) override;
	virtual void Free() override;
	
public:
	BoundingSphere* GetSphere() const { return _sphere; }

public:
	BoundingSphere* _sphere = nullptr;
	BoundingSphere* _sphereOriginal = nullptr;

};

END