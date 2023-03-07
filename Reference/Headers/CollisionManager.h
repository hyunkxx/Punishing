#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CCollisionManager final : public CBase
{
	DECLARE_SINGLETON(CCollisionManager)

public:
	explicit CCollisionManager();
	virtual ~CCollisionManager() = default;

public:
	HRESULT AddCollider(class CCollider* collider);
	void PhysicsUpdate();
	void Render();

private:
	void Clear();

public:
	virtual void Free() override;

private:
	vector<class CCollider*> _collisions;

};

END