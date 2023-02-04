#include "pch.h"
#include "..\Public\MapTool.h"
#include "GameInstance.h"
#include "ToolCamera.h"

CMapTool::CMapTool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CMapTool::Initialize()
{
	if (FAILED(Ready_Layer_BackGround(TEXT("layer_terrain"))))
		return E_FAIL;

	if (FAILED(Ready_Layre_Camera(TEXT("layer_camera"))))
		return E_FAIL;

	return S_OK;
}

void CMapTool::Tick(_double TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	
	if (pGameInstance->Input_MouseState_Custom(DIMK_LB) == KEY_STATE::TAP)
	{
		wstring cube = L"cube" + to_wstring(m_CubeCount);
		pGameInstance->Add_GameObject(TOOL_MAP, TEXT("proto_obj_cube"), L"layer_cube", cube.c_str());
		m_CubeCount++;
	}
}

HRESULT CMapTool::Ready_Layer_BackGround(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	
	if(FAILED(pGameInstance->Add_GameObject(TOOL_MAP, TEXT("proto_obj_terrain"), L"terrain", pLayerTag)))
		return S_OK;

	if (FAILED(pGameInstance->Add_GameObject(TOOL_MAP, TEXT("proto_obj_cube"), L"cube", pLayerTag)))
		return S_OK;

	return S_OK;
}

HRESULT CMapTool::Ready_Layre_Camera(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	CCamera::CAMERA_DESC CameraDesc;
	ZeroMemory(&CameraDesc, sizeof(CCamera::CAMERA_DESC));

	CameraDesc.TransformDesc.fMoveSpeed = 20.f;
	CameraDesc.TransformDesc.fRotationSpeed = XMConvertToRadians(90.f);

	CameraDesc.vEye = _float3(0.f, 10.f, -10.f);
	CameraDesc.vAt = _float3(0.f, 0.f, 0.f);
	CameraDesc.vAxisY = _float3(0.f, 1.f, 0.f);

	CameraDesc.fFovy = XMConvertToRadians(45.f);
	CameraDesc.fAspect = g_iWinSizeX / (_float)g_iWinSizeY;
	CameraDesc.fNear = 0.1f;
	CameraDesc.fFar = 1000.f;

	if (FAILED(pGameInstance->Add_GameObject(TOOL_MAP, TEXT("proto_obj_tool_camera"), L"tool_camera", pLayerTag, &CameraDesc)))
		return E_FAIL;

	return S_OK;
}

void CMapTool::RenderLevelUI()
{
	ImGui::Begin("Level Layout");
	ImGui::Text("Level Layour");
	ImGui::End();
}

CMapTool* CMapTool::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMapTool* pInstance = new CMapTool(pDevice, pContext);
	if (FAILED(pInstance->Initialize()))
	{
		wstring message = L"Failed to Create : CMapTool";
		MESSAGE(message);
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMapTool::Free()
{
	__super::Free();
}
