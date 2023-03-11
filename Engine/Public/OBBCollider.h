#pragma once

#include "Collider.h"

BEGIN(Engine)

class ENGINE_DLL COBBCollider final : public CCollider
{
protected:
	explicit COBBCollider(ID3D11Device* device, ID3D11DeviceContext* context);
	explicit COBBCollider(const COBBCollider& rhs);
	virtual ~COBBCollider() = default;

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

	_float3 GetCorners();

public:
	static COBBCollider* Create(ID3D11Device* device, ID3D11DeviceContext* context);
	virtual CComponent* Clone(void* arg) override;
	virtual void Free() override;

public:
	BoundingOrientedBox* GetObb() const { return _obb; }

public:
	BoundingOrientedBox* _obb = nullptr;
	BoundingOrientedBox* _obbOriginal = nullptr;

};

END