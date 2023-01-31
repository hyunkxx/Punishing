#pragma once

#include "Camera.h"

BEGIN(Client)

class CDynamicCamera final : public CCamera
{
private:
	CDynamicCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDynamicCamera(const CDynamicCamera& rhs);
	virtual ~CDynamicCamera() = default;

public:
	virtual	HRESULT Initialize_Prototype() override;
	virtual	HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render();

public:
	HRESULT Add_Components();
	HRESULT Setup_ShaderResources();

public:
	static CDynamicCamera* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

};

END