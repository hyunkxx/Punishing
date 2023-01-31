#pragma once

#include "Base.h"

BEGIN(Engine)
class CGameInstance;
class CRenderer;
END

BEGIN(Tool)

class CToolMain : public CBase
{
private:
	explicit CToolMain();
	virtual ~CToolMain() = default;

public:
	HRESULT Initialize();
	void Tick(_double TimeDelta);
	void LateTick(_double TimeDelta);
	void Render();

public:
	HRESULT Open_Level(TOOL_TYPE eType);

private:
	HRESULT Initialize_Components();
	HRESULT Initialize_GameObjects();

public:
	static CToolMain* Create();
	virtual void Free() override;

private:
	class CGUIManager*		m_pGUIManager{ nullptr };
	class CGameInstance*	m_pGameInstance{ nullptr };

	class CRenderer*		m_pRenderer{ nullptr };
	ID3D11Device*			m_pDevice{ nullptr };
	ID3D11DeviceContext*	m_pContext{ nullptr };
};

END