#pragma once

#include "Base.h"
#include "GameObject.h"

BEGIN(Engine)

class CLayer final : public CBase
{
public:
	CLayer();
	virtual ~CLayer() = default;

public:
	HRESULT Add_GameObject(CGameObject* pGameObject);
	void Tick(_double TimeDelta);
	void LateTick(_double TimeDelta);

public:
	static CLayer* Create();
	virtual void Free() override;

private:
	list<class CGameObject*>				m_GameObjects;
	typedef list<class CGameObject*>		GAMEOBJECTS;

};

END