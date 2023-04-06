#pragma once

#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CShader;
class CModel;
END

BEGIN(Client)

class CFloorCircle final : public CGameObject
{
public:
	enum CIRCLE { CIRCLE_PLAYER, CIRCLE_ENEMY, CIRCLE_END };

public:
	explicit CFloorCircle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CFloorCircle(const CFloorCircle& rhs);
	virtual ~CFloorCircle() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual void RenderGUI() override;

public:
	void SetType(CIRCLE eCircle) { m_eCircleType = eCircle; };
	void SetScale(_float3 vScale);
private:
	HRESULT Add_Components();
	HRESULT Setup_ShaderResources();

public:
	static CFloorCircle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

public:
	CTransform* m_pTransform = nullptr;
	CRenderer* m_pRenderer = nullptr;
	CShader* m_pShader = nullptr;
	CModel* m_pPlayerCircle = nullptr;
	CModel* m_pEnemyCircle = nullptr;

	CGameObject* m_pOwner = nullptr;
	CTransform* m_pOwnerTransform = nullptr;
	CIRCLE m_eCircleType = CIRCLE_END;
};

END