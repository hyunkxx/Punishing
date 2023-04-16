#pragma once

#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CShader;
class CModel;
END

BEGIN(Client)

class CFootSmoke final : public CGameObject
{
public:
	enum class FOOT { PLAYER, ENEMY };

public:
	explicit CFootSmoke(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CFootSmoke(const CFootSmoke& rhs);
	virtual ~CFootSmoke() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual void RenderGUI() override;

public:
	void SetType(FOOT eType) { m_eFootType = eType; }
	void SetSaveScale(_float fScale) { m_vSaveScale = fScale; };
	void StartEffect();
	_bool IsActive()const { return m_bActive; }
private:
	HRESULT Add_Components();
	HRESULT Setup_ShaderResources();

public:
	static CFootSmoke* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

public:
	FOOT m_eFootType = FOOT::PLAYER;

	CTransform* m_pTransform = nullptr;
	CRenderer* m_pRenderer = nullptr;
	CShader* m_pShader = nullptr;

	CModel* m_pModel = nullptr;
	CTexture* m_pDiffuseTexture = nullptr;

	CGameObject* m_pOwner = nullptr;
	CTransform* m_pOwnerTransform = nullptr;

	_bool m_bActive = false;
	_float m_fTimeAcc = 0.f;
	_float m_vCurScale = 3.f;
	_float m_vSaveScale = 3.f;

};

END