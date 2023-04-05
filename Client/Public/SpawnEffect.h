#pragma once

#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CModel;
class CShader;
class CBone;
class CModel;
class CTexture;
END

BEGIN(Client)

class CSpawnEffect final : public CGameObject
{
private:
	CSpawnEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CSpawnEffect() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual void RenderGUI() override;

	virtual _float GetLengthFromCamera() override;
public:
	void SetRender(_bool value) { m_bRender = value; m_fTimeAcc = 0.f; };
	void SetScale(_float3 vScale);
	_float3 GetScale() const;
	void SetTransfrom(_fvector vPos);


private:
	HRESULT AddComponents();
	HRESULT SetupShaderResources();

public:
	static CSpawnEffect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

private:
	CRenderer* m_pRenderer = nullptr;
	CTransform* m_pTransform = nullptr;
	CModel* m_pModel = nullptr;
	CShader* m_pShader = nullptr;

	CTexture* m_pTexture0 = nullptr; //실린더 디퓨즈
	CTexture* m_pTexture1 = nullptr; //실린더 마스크

private:
	_bool m_bRender = false;
	_float m_fTimeAcc = 0.f;

};

END