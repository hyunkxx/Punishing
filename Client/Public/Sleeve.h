#pragma once

#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CModel;
class CShader;
END

BEGIN(Client)

//Kalienina Weapon
class CSleeve final : public CGameObject
{
private:
	CSleeve(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CSleeve() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual void RenderGUI() override;

private:
	HRESULT AddComponents();
	HRESULT SetupShaderResources();

public:
	static CSleeve* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

private:
	CRenderer* mRenderer = nullptr;
	CTransform* mTransform = nullptr;
	CModel* mModel = nullptr;
	CShader* mShader = nullptr;

private:
	CGameObject* mOwner = nullptr;

};

END