#pragma once

#include "VIBuffer_Triangle.h"
#include "VIBuffer_Terrain.h"
#include "VIBuffer_Cube.h"
#include "VIBuffer_Rect.h"
#include "Transform.h"
#include "Renderer.h"
#include "Texture.h"
#include "Shader.h"

BEGIN(Engine)
/* 컴포넌트들의 원형을 레벨별로 나누어 보관한다. */
class ENGINE_DLL CComponent_Manager : public CBase
{
	DECLARE_SINGLETON(CComponent_Manager)

private:
	CComponent_Manager();
	virtual ~CComponent_Manager() = default;

public:
	HRESULT Reserve_Manager(_uint iLevelMaxCount);
	HRESULT Add_Prototype(_uint iLevelIndex, const _tchar* pPrototypeTag, class CComponent* pPrototype);
	class CComponent* Clone_Component(_uint iLevelIndex, const _tchar* pPrototypeTag, void* pArg = nullptr);

private:
	class CComponent* Find_Prototype(_uint iLevelIndex, const _tchar* pPrototypeTag);

public:
	virtual void Free() override;

private:
	_uint m_iLevelMaxCount = { 0 };
	unordered_map<const _tchar*, class CComponent*>* m_pPrototypes = { nullptr };
	typedef unordered_map<const _tchar*, class CComponent*> PROTOTYPES;

};

END