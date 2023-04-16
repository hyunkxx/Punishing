#pragma once

#include "GameObject.h"

BEGIN(Engine)
class CTransform;
END

BEGIN(Client)

class CBuffHandler final : public CGameObject
{
public:
	explicit CBuffHandler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CBuffHandler(const CBuffHandler& rhs);
	virtual ~CBuffHandler() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual void RenderGUI() override;
	//virtual _float GetLengthFromCamera() override;
public:
	void StartEffect(_fvector vPosition, _float fSpeed);
	void SetPosition(_fvector vPosition);

public:
	static CBuffHandler* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

private:
	class CBuff* m_pBuff[9] = {nullptr, };
	CTransform* m_pPlayerTransform = nullptr;

	_float m_fTimer[9] = { 0.f, };

};

END