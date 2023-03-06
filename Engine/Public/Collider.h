#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CCollider abstract : public CComponent
{
public:
	enum COLL_TYPE { COLL_SPHERE, COLL_AABB, COLL_OBB, COLL_END };

	typedef struct tagColliderDesc
	{
		class CGameObject* owner;
		_float3 vCenter;
		_float3 vExtants;
		_float3 vRotaion;
	}COLLIDER_DESC;

protected:
	explicit CCollider(ID3D11Device* device, ID3D11DeviceContext* context);
	explicit CCollider(const CCollider& rhs);

public:
	virtual ~CCollider() = default;

public:
	virtual HRESULT InitializePrototype(ID3D11Device* device, ID3D11DeviceContext* context) = 0;
	virtual HRESULT Initialize(void* arg) = 0;

public:
	virtual void Update(_matrix transformMatrix) = 0;
	virtual _bool Collision(CCollider* targetCollider) = 0;
	virtual void Render() = 0;

public:
	_bool Compare(CCollider* collider) { return this == collider; };
	void EraseHitCollider(CCollider* collider);
	void AddHitCollider(CCollider* collider);
	_bool IsHitCollider(CCollider* collider);
	void SetColor(_float4 color) { _color = color; };
	void SetOwner(class CGameObject* owner) { _collDesc.owner = owner; }
	class CGameObject* GetOwner() const { return _collDesc.owner; }
	COLL_TYPE GetType() const {	return _type; }
	void SetCollision(_bool value) { _isColl = value; }
	_bool IsColl() const { return _isColl; }
	_float3 GetCenter() { return _collDesc.vCenter; }
	_float3 GetExtents() { return _collDesc.vExtants; }
	//void Reset(_float3 );

public:
	virtual CComponent* Clone(void* arg = nullptr) = 0;
	virtual void Free() override;

public:

protected:
	_bool _isColl = false;
	COLLIDER_DESC _collDesc;
	COLL_TYPE _type = COLL_TYPE::COLL_END;
	
	_float4 _color = { 1.f, 1.f, 1.f, 1.f };

	list<CCollider*> hitCollider;
protected:
	PrimitiveBatch<VertexPositionColor>* _batch = nullptr;
	BasicEffect* _effect = nullptr;
	ID3D11InputLayout* _inputLayout = nullptr;

};

class ENGINE_DLL IOnCollisionEnter
{
public:
	virtual void OnCollisionEnter(CCollider * src, CCollider * dest) = 0;
};

class ENGINE_DLL IOnCollisionStay
{
public:
	virtual void OnCollisionStay(CCollider * src, CCollider * dest) = 0;
};

class ENGINE_DLL IOnCollisionExit
{
public:
	virtual void OnCollisionExit(CCollider * src, CCollider * dest) = 0;
};

END