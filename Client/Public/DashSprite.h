#pragma once

#include "Sprite.h"

BEGIN(Engine)
class CTexture;
END

BEGIN(Client)

class CDashSprite final : public CSprite
{
public:
	explicit CDashSprite(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CDashSprite(const CSprite& rhs);
	virtual ~CDashSprite() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual void RenderGUI() override;
	//virtual _float GetLengthFromCamera() override;
public:
	void StartEffect(CTransform* vPosition);

private:
	HRESULT AddComponent();

public:
	static CDashSprite* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

private:
	CTexture* m_pRightDustTexture = nullptr;
	CTexture* m_pLeftDustTexture = nullptr;

	CTransform* m_pRightFootTransform = nullptr;
	CTransform* m_pLeftFootTransform = nullptr;
};

END