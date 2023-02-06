#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CObject_Manager final : public CBase
{
	DECLARE_SINGLETON(CObject_Manager)
public:
	typedef unordered_map<const _tchar*, class CLayer*> LAYERS;

private:
	CObject_Manager();
	virtual ~CObject_Manager() = default;

public:
	HRESULT Reserve_Manager(_uint iLevelMaxCount);
	HRESULT Add_Prototype(const _tchar* pPrototypeTag, class CGameObject* pPrototype);
	HRESULT Add_GameObject(_uint iLevelIndex, const _tchar* pPrototypeTag, const _tchar* pLayerTag, wstring pObjectTag, void* pArg = nullptr);

	void Tick(_double TimeDelta);
	void LateTick(_double TimeDelta);
	void RenderGUI();
	void Clear(_uint iLevelIndex);

	class CLayer* Find_Layer(_uint iLevelIndex, const _tchar* pLayerTag);
	unordered_map<const _tchar*, class CLayer*>* m_pLayers = { nullptr };
	//class CGameObject* Find_GameObject(wstring strObjectTag);
private:
	class CGameObject* Find_Prototype(const _tchar* pPrototypeTag);

public:
	virtual void Free() override;

private: // 원형 객체 보관
	unordered_map<const _tchar*, class CGameObject*> m_Prototypes;

private:
	_uint m_iLevelMaxCount;

};

END