#pragma once

#include "Sprite.h"

BEGIN(Engine)
class CTexture;
END

BEGIN(Client)

class CFootstep final : public CSprite
{
public:
	explicit CFootstep(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CFootstep(const CSprite& rhs);
	virtual ~CFootstep() = default;

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
	_bool IsRender() const { return m_bRender; }

private:
	HRESULT AddComponent();

public:
	static CFootstep* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

private:
	CTexture* m_pDiffuseTexture = nullptr;

};

END