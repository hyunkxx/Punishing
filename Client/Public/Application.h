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
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };

	CGameInstance*				m_pGameInstance = { nullptr };

	CRenderer*					m_pRenderer = { nullptr };
};

END