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
	_bool IsDebugRender() const { return m_bDebugRender; }
	void SetDebugRender(_bool value) { m_bDebugRender = value; }

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
	_bool m_bDebugRender = true;
	vector<class CCollider*> m_BaseCollisions;
	vector<class CCollider*> m_BackgroundCollisions;

};

END