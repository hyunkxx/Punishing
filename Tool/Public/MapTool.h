#pragma once

#include "Level.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Tool)

class CMapTool final : public CLevel
{
private:
	CMapTool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CMapTool() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Tick(_double TimeDelta) override;
	virtual void RenderLevelUI() override;

private:
	HRESULT Ready_Layer_BackGround(const _tchar* pLayerTag);
	HRESULT Ready_Layre_Camera(const _tchar * pLayerTag);
	HRESULT Ready_Layre_Cube(const _tchar * pLayerTag);

public:
	void Cube_Picking();
	void MoveToTerrainPickPosition();

public:
	static CMapTool* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;

public:
	_uint m_CubeCount = { 0 };
	class CCube* m_pSelectCube = { nullptr };

private:
	CGameInstance* m_pGameInstance = { nullptr };

};

END