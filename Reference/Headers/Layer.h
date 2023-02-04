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
	//HRESULT Add_GameObject(CGameObject* pGameObject);
	HRESULT Add_GameObject(wstring, CGameObject* pGameObject);
	void Tick(_double TimeDelta);
	void LateTick(_double TimeDelta);
	void RenderGUI();

	CGameObject* Find_GameObject(wstring strObjectTag);
public:
	static CLayer* Create();
	virtual void Free() override;

private:
	unordered_map<wstring ,class CGameObject*>				m_GameObjects;
	typedef unordered_map<wstring ,class CGameObject*>		GAMEOBJECTS;

};

END