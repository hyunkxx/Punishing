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
	HRESULT AddCollider(class CCollider* collider, _uint iLayerIndex = 0);
	void PhysicsUpdate();
	void Render();
	int GetHasCollisionCount();

private:
	void Clear();

public:
	virtual void Free() override;

private:
	vector<class CCollider*> m_BaseCollisions;
	vector<class CCollider*> m_BackgroundCollisions;

};

END