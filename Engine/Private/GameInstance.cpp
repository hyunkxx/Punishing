#include "..\Public\GameInstance.h"

#include "CollisionManager.h"
#include "Graphic_Device.h"
#include "Level_Manager.h"
#include "Object_Manager.h"
#include "Component_Manager.h"
#include "CollisionManager.h"
#include "Timer_Manager.h"
#include "Input_Device.h"
#include "LightManager.h"
#include "Layer.h"

IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()
	: m_pGraphic_Device { CGraphic_Device::GetInstance() }
	, m_pLevel_Manager { CLevel_Manager::GetInstance() }
	, m_pObject_Manager { CObject_Manager::GetInstance() }
	, m_pComponent_Manager { CComponent_Manager ::GetInstance() }
	, m_pPipeLine { CPipeLine::GetInstance() }
	, m_pTimer_Manager { CTimer_Manager::GetInstance() }
	, m_pInput_Device { CInput_Device::GetInstance() }
	, m_LightManager{ CLightManager::GetInstance() }
	, m_pCollision_Manager { CCollisionManager::GetInstance() }
{
	Safe_AddRef(m_pCollision_Manager);
	Safe_AddRef(m_pInput_Device);
	Safe_AddRef(m_pTimer_Manager);
	Safe_AddRef(m_pPipeLine);
	Safe_AddRef(m_pGraphic_Device);
	Safe_AddRef(m_pLevel_Manager);
	Safe_AddRef(m_pObject_Manager);
	Safe_AddRef(m_pComponent_Manager);
	Safe_AddRef(m_LightManager);
}

HRESULT CGameInstance::Engine_Initialize(const GRAPHIC_DESC& GraphicDesc, _uint iLevelCount, ID3D11Device** ppDevice_out, ID3D11DeviceContext** ppContext_out)
{
	if (!m_pGraphic_Device || !m_pObject_Manager || !m_pComponent_Manager || !m_pPipeLine || !m_pInput_Device)
		return E_FAIL;

	if (FAILED(m_pGraphic_Device->Ready_Graphic_Device(GraphicDesc.hWnd, GraphicDesc.eMode, GraphicDesc.iWinSizeX, GraphicDesc.iWinSizeY, ppDevice_out, ppContext_out)))
		return E_FAIL;

	if (FAILED(m_pInput_Device->Initialize_Input(GraphicDesc.hInstance, GraphicDesc.hWnd)))
		return E_FAIL;

	if (FAILED(m_pObject_Manager->Reserve_Manager(iLevelCount)))
		return E_FAIL;

	if (FAILED(m_pComponent_Manager->Reserve_Manager(iLevelCount)))
		return E_FAIL;

	if (FAILED(m_pPipeLine->Initialize()))
		return E_FAIL;

	return S_OK;
}

HRESULT CGameInstance::Engine_Tick(_double TimeDelta)
{
	if (!m_pLevel_Manager || !m_pObject_Manager || !m_pPipeLine)
		return E_FAIL;

	m_pInput_Device->Update();

	m_pLevel_Manager->Tick_Level(TimeDelta);

	m_pPipeLine->Tick();
	m_pObject_Manager->Tick(TimeDelta);
	m_pObject_Manager->LateTick(TimeDelta);
	m_pCollision_Manager->PhysicsUpdate();

	return S_OK;
}

HRESULT CGameInstance::Clear_RenderTargetView(_float4 vColor)
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	return m_pGraphic_Device->Clear_RenderTargetView(vColor);
}

HRESULT CGameInstance::Clear_DepthStencilView()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	return m_pGraphic_Device->Clear_DepthStencilView();
}

HRESULT CGameInstance::Present()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	return m_pGraphic_Device->Present();
}

HRESULT CGameInstance::Open_Level(_uint iLevelIndex, CLevel* pCurrentLevel)
{
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	return m_pLevel_Manager->Open_Level(iLevelIndex, pCurrentLevel);
}

void CGameInstance::RenderLevelUI()
{
	if (nullptr == m_pLevel_Manager)
		return;

	m_pLevel_Manager->RenderLevelUI();
}

HRESULT CGameInstance::Add_Prototype(const _tchar* pPrototypeTag, CGameObject* pPrototype)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_Prototype(pPrototypeTag, pPrototype);
}

CGameObject* CGameInstance::Add_GameObject(_uint iLevelIndex, const _tchar* pPrototypeTag, const _tchar* pLayerTag, wstring strObjectTag, void* pArg)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Add_GameObject(iLevelIndex, pPrototypeTag, pLayerTag, strObjectTag, pArg);
}

void CGameInstance::RenderGUI()
{
	if (nullptr == m_pObject_Manager)
		return;

	if (nullptr == m_pCollision_Manager)
		return;

	m_pObject_Manager->RenderGUI();
}

void CGameInstance::LayerClear(_uint iLevelIndex, const _tchar * pLayerTag)
{
	if (nullptr == m_pObject_Manager)
		return;

	CLayer* pLayer = m_pObject_Manager->Find_Layer(iLevelIndex, pLayerTag);
	if (pLayer == nullptr)
		return;

	pLayer->Clear();
	return;
}

CLayer* CGameInstance::Find_Layer(_uint iLevelIndex, const _tchar* pLayerTag)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Find_Layer(iLevelIndex, pLayerTag);
}

CGameObject* CGameInstance::Find_GameObject(_uint iLevelIndex, const _tchar* pLayerTag, wstring strObjectTag)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	CLayer* pLayer = m_pObject_Manager->Find_Layer(iLevelIndex, pLayerTag);
	if (nullptr == pLayer)
		return nullptr;

	CGameObject* pGameObject = nullptr;
	pGameObject = pLayer->Find_GameObject(strObjectTag);
	if (nullptr == pGameObject)
		return nullptr;

	return pGameObject;
}

CComponent* CGameInstance::Find_Component(const _tchar * pComponentTag)
{
	return nullptr;
}

HRESULT CGameInstance::Add_Prototype(_uint iLevelIndex, const _tchar * pPrototypeTag, CComponent * pPrototype)
{
	if (nullptr == m_pComponent_Manager)
		return E_FAIL;

	return m_pComponent_Manager->Add_Prototype(iLevelIndex, pPrototypeTag, pPrototype);
}

CComponent * CGameInstance::Clone_Component(_uint iLevelIndex, const _tchar * pPrototypeTag, void * pArg)
{
	if (nullptr == m_pComponent_Manager)
		return nullptr;

	return m_pComponent_Manager->Clone_Component(iLevelIndex, pPrototypeTag, pArg);
}

_float4x4 CGameInstance::Get_Transform_float4x4(CPipeLine::TRANSFORM_STATE eState)
{
	if (nullptr == m_pPipeLine)
		return _float4x4();

	return m_pPipeLine->Get_Transform_float4x4(eState);
}

_float4x4 CGameInstance::Get_Transform_float4x4_Inverse(CPipeLine::TRANSFORM_STATE eState)
{
	if (nullptr == m_pPipeLine)
		return _float4x4();

	return m_pPipeLine->Get_Transform_float4x4_Inverse(eState);
}

_matrix CGameInstance::Get_Transform_Matrix(CPipeLine::TRANSFORM_STATE eState)
{
	if (nullptr == m_pPipeLine)
		return XMMatrixIdentity();

	return m_pPipeLine->Get_Transform_Matrix(eState);
}

_matrix CGameInstance::Get_Transform_Matrix_Inverse(CPipeLine::TRANSFORM_STATE eState)
{
	if (nullptr == m_pPipeLine)
		return XMMatrixIdentity();

	return m_pPipeLine->Get_Transform_Matrix_Inverse(eState);
}

_float4 CGameInstance::Get_CamPosition()
{
	if (nullptr == m_pPipeLine)
		return _float4(0.f, 0.f, 0.f, 1.f);

	return m_pPipeLine->Get_CamPosition();
}

HRESULT CGameInstance::Initialize_Input(HINSTANCE hInst, HWND hWnd)
{
	if (nullptr == m_pInput_Device)
		return E_FAIL;

	return m_pInput_Device->Initialize_Input(hInst, hWnd);
}

void CGameInstance::Update(void)
{
	if (nullptr == m_pInput_Device)
		return;

	m_pInput_Device->Update();
}

_byte CGameInstance::Input_KeyState(_ubyte byKeyID)
{
	if (nullptr == m_pInput_Device)
		return _byte();

	return m_pInput_Device->Get_DIKeyState(byKeyID);
}

KEY_STATE CGameInstance::Input_KeyState_Custom(_int _key)
{
	if (nullptr == m_pInput_Device)
		return KEY_STATE::NONE;

	return m_pInput_Device->Get_DIKeyState_Custom(_key);
}

KEY_STATE CGameInstance::Input_MouseState_Custom(MOUSE_KEYSTATE _key)
{
	if (nullptr == m_pInput_Device)
		return KEY_STATE::NONE;

	return m_pInput_Device->Get_DIMouseState_Custom(_key);
}

_byte CGameInstance::Input_MouseState(MOUSE_KEYSTATE byMouseID)
{
	if (nullptr == m_pInput_Device)
		return _byte();

	return m_pInput_Device->Get_DIMouseState(byMouseID);
}

_long CGameInstance::Input_MouseMove(MOUSE_MOVESTATE eMoveState)
{
	if (nullptr == m_pInput_Device)
		return _long();

	return m_pInput_Device->Get_DIMouseMove(eMoveState);
}

HRESULT CGameInstance::CreateTimer(const _tchar* pTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;

	return m_pTimer_Manager->CreateTimer(pTimerTag);
}

_float CGameInstance::GetTimer(const _tchar* pTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return 0.f;

	return m_pTimer_Manager->GetTimer(pTimerTag);
}

void CGameInstance::SetTimer(const _tchar* pTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return;

	m_pTimer_Manager->SetTimer(pTimerTag);
}

HRESULT CGameInstance::AddLight(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const LIGHT_DESC& LightDesc)
{
	if (nullptr == m_LightManager)
		return E_FAIL;

	return m_LightManager->AddLight(pDevice, pContext, LightDesc);
}

const LIGHT_DESC* CGameInstance::GetLightDesc(_uint Index)
{
	if (nullptr == m_LightManager)
		return nullptr;

	return m_LightManager->GetLightDesc(Index);
}

void CGameInstance::SetCollisionDebugRender(_bool value)
{
	if (m_pCollision_Manager == nullptr)
		return;

	return m_pCollision_Manager->SetDebugRender(value);
}

HRESULT CGameInstance::AddCollider(CCollider * collider, _uint iLayerIndex)
{
	if (m_pCollision_Manager == nullptr)
		return E_FAIL;

	return m_pCollision_Manager->AddCollider(collider, iLayerIndex);
}

void CGameInstance::CollisionRender()
{
	if (m_pCollision_Manager == nullptr)
		return;

	return m_pCollision_Manager->Render();
}

//void CGameInstance::PhysicsUpdate()
//{
//	if (m_pCollision_Manager == nullptr)
//		return;
//
//	return m_pCollision_Manager->PhysicsUpdate();
//}

void CGameInstance::Engine_Release()
{
	CCollisionManager::DestroyInstance();
	CGameInstance::DestroyInstance();
	CInput_Device::DestroyInstance();
	CLightManager::DestroyInstance();
	CTimer_Manager::DestroyInstance();
	CComponent_Manager::DestroyInstance();
	CObject_Manager::DestroyInstance();
	CLevel_Manager::DestroyInstance();
	CPipeLine::DestroyInstance();
	CGraphic_Device::DestroyInstance();
}

void CGameInstance::Free()
{
	Safe_Release(m_pCollision_Manager);
	Safe_Release(m_LightManager);
	Safe_Release(m_pInput_Device);
	Safe_Release(m_pTimer_Manager);
	Safe_Release(m_pPipeLine);
	Safe_Release(m_pComponent_Manager);
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pLevel_Manager);
	Safe_Release(m_pGraphic_Device);
}
