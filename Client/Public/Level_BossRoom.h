#pragma once

#include "Level.h"

BEGIN(Engine)
class CGameObject;
END

BEGIN(Client)

class CLevel_BossRoom final : public CLevel
{
private:
	CLevel_BossRoom(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_BossRoom() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Tick(_double TimeDelta) override;

private:
	HRESULT Ready_Light();
	HRESULT Ready_Layer_BackGround(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Wall(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Camera(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Player(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Enemy(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Effect(const _tchar* pLayerTag);
	HRESULT Ready_Layer_UI(const _tchar* pLayerTag);

public:
	static CLevel_BossRoom* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;

	CGameObject* mPlayer = nullptr;
	CGameObject* m_pHealthBar = nullptr;
	class CPlayerCamera* mCamera = nullptr;

	_float m_fBossBgmStart = 0.15f;
};

END