#pragma once

#include "Component_Manager.h"
#include "PipeLine.h"

BEGIN(Engine)

class ENGINE_DLL CGameInstance final: public CBase
{
	DECLARE_SINGLETON(CGameInstance)

private:
	CGameInstance();
	virtual ~CGameInstance() = default;

public:
	HRESULT Engine_Initialize(const GRAPHIC_DESC& GraphicDesc, _uint iLevelCount, ID3D11Device** ppDevice_out, ID3D11DeviceContext** ppContext_out);
	HRESULT Engine_Tick(_double TimeDelta);

public: //Graphic_Device
	HRESULT Clear_RenderTargetView(_float4 vColor);
	HRESULT Clear_DepthStencilView();
	HRESULT Present();

public: //Level_Manager
	HRESULT Open_Level(_uint iLevelIndex, class CLevel* pCurrentLevel);
	void RenderLevelUI();

public: //Object_Manager
	HRESULT Add_Prototype(const _tchar* pPrototypeTag, class CGameObject* pPrototype);
	class CGameObject* Add_GameObject(_uint iLevelIndex, const _tchar* pPrototypeTag, const _tchar* pLayerTag, wstring strObjectTag, void* pArg = nullptr);
	void RenderGUI();

	void LayerClear(_uint iLevelIndex, const _tchar* pLayerTag);
	class CLayer* Find_Layer(_uint iLevelIndex, const _tchar* pLayerTag);
	class CGameObject* Find_GameObject(_uint iLevelIndex, const _tchar* pLayerTag, wstring strObjectTag);
	class CComponent* Find_Component(const _tchar* pComponentTag);

public: //Component_Manager
	HRESULT Add_Prototype(_uint iLevelIndex, const _tchar* pPrototypeTag, class CComponent* pPrototype);
	class CComponent* Clone_Component(_uint iLevelIndex, const _tchar* pPrototypeTag, void* pArg = nullptr);

public: //PipeLine
	_float4x4 Get_Transform_float4x4(CPipeLine::TRANSFORM_STATE eState);
	_float4x4 Get_Transform_float4x4_Inverse(CPipeLine::TRANSFORM_STATE eState);
	_matrix Get_Transform_Matrix(CPipeLine::TRANSFORM_STATE eState);
	_matrix Get_Transform_Matrix_Inverse(CPipeLine::TRANSFORM_STATE eState);
	_float4 Get_CamPosition();

public: //Input_Device
	HRESULT	Initialize_Input(HINSTANCE hInst, HWND hWnd);
	void	Update(void);

	_byte		Input_KeyState(_ubyte byKeyID);
	KEY_STATE	Input_KeyState_Custom(_int _key);
	KEY_STATE	Input_MouseState_Custom(MOUSE_KEYSTATE _key);
	_byte		Input_MouseState(MOUSE_KEYSTATE byMouseID);
	_long		Input_MouseMove(MOUSE_MOVESTATE eMoveState);

public: //Timer_Manager
	HRESULT	CreateTimer(const _tchar* pTimerTag);
	_float	GetTimer(const _tchar* pTimerTag);
	void	SetTimer(const _tchar* pTimerTag);

public: //LightManager
	HRESULT AddLight(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const LIGHT_DESC& LightDesc);
	const LIGHT_DESC* GetLightDesc(_uint Index);

public: //CollisionManager
	HRESULT AddCollider(class CCollider* collider, _uint iLayerIndex = 0);
	//void PhysicsUpdate();
	void CollisionRender();

public:
	static void Engine_Release();
	virtual void Free() override;

private:
	class CCollisionManager*		m_pCollision_Manager = { nullptr };
	class CGraphic_Device*			m_pGraphic_Device = { nullptr };
	class CLevel_Manager*			m_pLevel_Manager = { nullptr };
	class CObject_Manager*			m_pObject_Manager = { nullptr };
	class CComponent_Manager*		m_pComponent_Manager = { nullptr };
	class CPipeLine*				m_pPipeLine = { nullptr };
	class CTimer_Manager*			m_pTimer_Manager = { nullptr };
	class CInput_Device*			m_pInput_Device = { nullptr };
	class CLightManager*			m_LightManager = { nullptr };
};

END