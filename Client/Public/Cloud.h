#pragma once

#include "Sprite.h"

BEGIN(Engine)
class CTexture;
END

BEGIN(Client)

class CCloud final : public CSprite
{
public:
	explicit CCloud(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CCloud(const CSprite& rhs);
	virtual ~CCloud() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual void RenderGUI() override;
	//virtual _float GetLengthFromCamera() override;
public:
	void StartEffect(_fvector vPosition);
	void SetPosition(_fvector vPosition);

private:
	HRESULT AddComponent();

public:
	static CCloud* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

private:
	CTexture* m_pDiffuseTexture = nullptr;

	_float4 m_vOriginPos = { 0.f, 0.f, 0.f, 0.f };
	_float4 m_vPos[5];
};

END