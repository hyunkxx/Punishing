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
	HRESULT Ready_Layre_Object(const _tchar * pLayerTag);

private: //Creator
	void CreatorUI();
	void EditStateUI();
	void CreateObject(const OBJ_DESC& objectDesc);
	void DeleteSelectObject();

public: //Picking
	void Cube_Picking();
	void SelectCubeMovement(_double TimeDelta);
	void MoveToTerrainPickPosition();

public: //SaveLoad
	void SaveObjects();
	void LoadObjects();
public:
	static CMapTool* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;

public:

private:
	_uint m_iObjectIndex = { 0 }; //오브젝트 인덱스 겸 오브젝트 카운트
	CGameInstance* m_pGameInstance = { nullptr };

private: //ImGui Value
	enum INDEX { GUI_CREATOR, GUI_STATE, GUI_END};
	_bool bClose[GUI_END];

	OBJ_DESC m_ObjectDesc;

	class CGameObject_Tool* m_pSelectObject = { nullptr };
	class CSaveLoader*		m_pSaveLoader = { nullptr };
};

END