#pragma once

#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
END

BEGIN(Tool)

class CGameObject_Tool abstract : public CGameObject
{
protected:
	CGameObject_Tool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject_Tool(const CGameObject_Tool& rhs);
	virtual ~CGameObject_Tool() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg = nullptr) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual void RenderGUI() override;

	virtual _float PickObject() = 0;
public:
	virtual void ApplyObjectDesc(const OBJ_DESC& objDesc);
	void SetObjectDesc(const OBJ_DESC& objDesc) { memcpy(&m_ObjectDesc, &objDesc, sizeof(OBJ_DESC)); }
	const OBJ_DESC& GetObjectDesc() { return m_ObjectDesc; }

	_bool IsSelect() { return m_IsSelect; }
	void Select() { m_IsSelect = true; };
	void UnSelect() { m_IsSelect = false; };

public:
	virtual void Free() override;

protected:
	bool m_IsSelect = false;
	OBJ_DESC m_ObjectDesc;

	CRenderer*	m_pRenderer = { nullptr };
	CTransform * m_pTransform = { nullptr };
};

END