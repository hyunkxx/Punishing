#pragma once

#include "Base.h"

BEGIN(Engine)
class CGameInstance;
class CRenderer;
END

BEGIN(Client)

class CApplication final: public CBase
{
private:
	CApplication();
	virtual ~CApplication() = default;

public:
	HRESULT Initialize();
	void Tick(_double TimeDelta);
	HRESULT Render();

private:
	HRESULT Open_Level(LEVEL_ID eLevelID);
	HRESULT Ready_Prototype_Static_Component();
	HRESULT Ready_Prototype_Static_GameObject();

private:
	HRESULT InitializeManager();
	void DestroyManager();

public:
	static CApplication* Create();
	virtual void Free() override;

private:
	_bool m_bIsHitFreeze = false;
	_float m_bHitFreezeLocal = 0.0f;
	const _float m_bHitFreezeTimeOut = 0.01f;

	static _uint s_TickCount;
	ID3D11Device*		 m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

	CGameInstance*	m_pGameInstance = { nullptr };
	CRenderer*		m_pRenderer = { nullptr };

	class CImGUIManager*			m_pGUIManager = { nullptr };
	class CStageCollisionManager*	m_pStageManager = { nullptr };
	class CSkillBallSystem*			m_pSkillSystem = { nullptr };
};

END