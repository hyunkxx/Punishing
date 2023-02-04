#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CGameObject abstract : public CBase
{
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
protected: // ÄÄÆ÷³ÍÆ® ¸â¹öº¯¼ö¿Í ¸Ê¿¡ ¸ðµÎ º¸°üÇÔ , ¸â¹ö + ¸Ê
	HRESULT Add_Component(_uint iLevelIndex, const _tchar* pPrototypeTag, const _tchar* pComponentTag, class CComponent** ppOut, void* pArg = nullptr);

public:
	virtual CGameObject* Clone(void* pArg = nullptr) = 0;
	virtual void Free() override;

protected:
	ID3D11Device*				m_pDevice;
	ID3D11DeviceContext*		m_pContext;

protected:
	unordered_map<const _tchar*, class CComponent*> m_Components;

};

END