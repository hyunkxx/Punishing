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
class CVIBuffer_Rect;
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
	void SetTransfrom(_float4x4 vPos);


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

	CTransform* m_pSpawnStartTransform = nullptr;
	CShader* m_pTextureShader = nullptr;
	CTexture* m_pTextureSpawnStart = false;
	CVIBuffer_Rect* m_pVIBuffer = false;

	_float m_fSpriteAcc = 0.f;
	_float m_fCurrentIndex = 0.0f;
	const _float m_fWidth = 4.0f;

};

END