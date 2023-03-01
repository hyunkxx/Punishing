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
	virtual void Update() = 0;
	virtual _bool Collision(CCollider* targetCollider) = 0;
	virtual void Render() = 0;

public:
	void EraseHitCollider(CCollider* collider);
	void AddHitCollider(CCollider* collider);
	_bool IsHitCollider(CCollider* collider);
	void SetOwner(class CGameObject* owner) { owner = owner; }
	class CGameObject* GetOwner() const { return _owner; }
	COLL_TYPE GetType() const {	return _type; }
	_bool IsColl() const { return _isColl; }
	//void Reset(_float3 );

public:
	virtual CComponent* Clone(void* arg = nullptr) = 0;
	virtual void Free() override;

public:

protected:
	class CGameObject* _owner;
	COLLIDER_DESC _collDesc;
	_bool _isColl = false;
	COLL_TYPE _type = COLL_TYPE::COLL_END;

	list<CCollider*> hitCollider;
protected:
	PrimitiveBatch<VertexPositionColor>* _batch = nullptr;
	BasicEffect* _effect = nullptr;
	ID3D11InputLayout* _inputLayout = nullptr;
	

};

class ENGINE_DLL IOnCollisionEnter abstract
{
public:
	virtual void OnCollisionEnter(CCollider* coll) = 0;
};

class ENGINE_DLL IOnCollisionStay abstract
{
public:
	virtual  void OnCollisionStay(CCollider* coll) = 0;
};

class ENGINE_DLL IOnCollisionExit abstract
{
public:
	virtual  void OnCollisionExit(CCollider* coll) = 0;
};

END