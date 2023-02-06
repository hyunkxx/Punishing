#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CGameObject abstract : public CBase
{
public:
	enum STATE { ACTIVE, DISABLE, DESTROY };

protected:
	CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject(const CGameObject& rhs);
	virtual ~CGameObject() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_double TimeDelta);
	virtual void LateTick(_double TimeDelta);
	virtual HRESULT Render();
	virtual void RenderGUI();

	class CComponent* Find_Component(const _tchar* pComponentTag);
	unordered_map<const _tchar*, class CComponent*> m_Components;

protected: // ÄÄÆ÷³ÍÆ® ¸â¹öº¯¼ö¿Í ¸Ê¿¡ ¸ðµÎ º¸°üÇÔ , ¸â¹ö + ¸Ê
	HRESULT Add_Component(_uint iLevelIndex, const _tchar* pPrototypeTag, const _tchar* pComponentTag, class CComponent** ppOut, void* pArg = nullptr);

public:
	void SetState(STATE eState) { m_eState = eState; }
	void Destroy() { m_eState = STATE::DESTROY; }
	_bool IsActive() { return m_eState == STATE::ACTIVE; }
	_bool IsDisable() { return m_eState == STATE::DISABLE; }
	_bool IsDestroy() { return m_eState == STATE::DESTROY; }

public:
	virtual CGameObject* Clone(void* pArg = nullptr) = 0;
	virtual void Free() override;

protected:
	ID3D11Device*				m_pDevice;
	ID3D11DeviceContext*		m_pContext;

protected:
	STATE m_eState = { ACTIVE };

};

END