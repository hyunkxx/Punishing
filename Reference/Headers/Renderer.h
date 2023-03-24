#pragma once

#include "Component.h"


BEGIN(Engine)
class CGameObject;

class ENGINE_DLL CRenderer final : public CComponent
{
public:
	enum RENDER_GROUP { RENDER_PRIORITY, RENDER_NONALPHA, RENDER_NONLIGHT, RENDER_ALPHABLEND, RENDER_UI, RENDER_ENDING, RENDER_END };
private:
	CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CRenderer() = default;

public:
	virtual HRESULT Initialize_Prototype() override;

public:
	HRESULT Add_RenderGroup(RENDER_GROUP eRenderGroup, class CGameObject* pGameObject);
	void Draw();
	void ZSort();

public:
	static CRenderer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

public:
	list<class CGameObject*> m_RenderObject[RENDER_END];
	list<class CGameObject*> m_AlphaRenderObject;

	typedef list<class CGameObject*> RENDER_OBJECT;

};

END

bool Compute(Engine::CGameObject* pSourObject, Engine::CGameObject* pDestObject);
