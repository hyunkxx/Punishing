#include "pch.h"
#include "..\Public\MapTool.h"
#include "GameInstance.h"

#include "Layer.h"
#include "GameObject.h"

#include "Cube.h"
#include "ToolCamera.h"
#include "Terrain.h"

CMapTool::CMapTool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
	, m_pGameInstance { CGameInstance::GetInstance() }
{
}

HRESULT CMapTool::Initialize()
{
	if (FAILED(Ready_Layer_BackGround(TEXT("layer_terrain"))))
		return E_FAIL;

	if (FAILED(Ready_Layre_Camera(TEXT("layer_camera"))))
		return E_FAIL;

	if (FAILED(Ready_Layre_Cube(TEXT("layer_cube"))))
		return E_FAIL;

	return S_OK;
}

void CMapTool::Tick(_double TimeDelta)
{
	if (m_pGameInstance->Input_KeyState_Custom(DIK_INSERT) == KEY_STATE::TAP)
	{
		wstring cube = L"cube" + to_wstring(m_CubeCount);
		m_pGameInstance->Add_GameObject(TOOL_MAP, TEXT("proto_obj_cube"), L"layer_cube", cube.c_str());
		m_CubeCount++;
	}

	if (m_pGameInstance->Input_MouseState_Custom(DIMK_LB) == KEY_STATE::TAP)
	{
		Cube_Picking();
	}

	if (m_pGameInstance->Input_KeyState_Custom(DIK_DELETE) == KEY_STATE::TAP)
	{
		if (nullptr != m_pSelectCube)
		{
			m_pSelectCube->Destroy();
			m_pSelectCube = nullptr;
		}
	}

	MoveToTerrainPickPosition();
}

HRESULT CMapTool::Ready_Layer_BackGround(const _tchar* pLayerTag)
{
	if(FAILED(m_pGameInstance->Add_GameObject(TOOL_MAP, TEXT("proto_obj_terrain"), pLayerTag, L"terrain")))
		return E_FAIL;

	return S_OK;
}

HRESULT CMapTool::Ready_Layre_Camera(const _tchar* pLayerTag)
{
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

	if (FAILED(m_pGameInstance->Add_GameObject(TOOL_MAP, TEXT("proto_obj_tool_camera"), pLayerTag, L"tool_camera", &CameraDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMapTool::Ready_Layre_Cube(const _tchar * pLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject(TOOL_MAP, TEXT("proto_obj_cube"), pLayerTag, L"cube")))
		return E_FAIL;

	return S_OK;
}

void CMapTool::Cube_Picking()
{
	auto Layer = m_pGameInstance->Find_Layer(TOOL_MAP, L"layer_cube");
	
	_float fDistance = FLT_MAX;

	auto GameObjects= Layer->m_GameObjects;
	for (auto& iter = GameObjects.begin() ; iter != GameObjects.end() ; ++iter)
	{
		auto pCube = dynamic_cast<CCube*>(iter->second);
		if (pCube)
		{
			float fRayDistance = pCube->Picking();

			if (0.f < fRayDistance)
			{
				if (fDistance > fRayDistance)
				{
					if (m_pSelectCube != nullptr)
					{
						m_pSelectCube->UnSelect();
					}

					m_pSelectCube = pCube;
					fDistance = fRayDistance;
				}
			}
		}
	}

	if (m_pSelectCube != nullptr)
	{
		m_pSelectCube->Select();
	}
}

void CMapTool::MoveToTerrainPickPosition()
{
	CGameObject* pTerrain = m_pGameInstance->Find_GameObject(TOOL_MAP, L"layer_terrain", L"terrain");
	if (nullptr == pTerrain)
		return;

	if (nullptr == m_pSelectCube)
		return;

	if (m_pGameInstance->Input_KeyState_Custom(DIK_LCONTROL) == KEY_STATE::HOLD
		&& m_pGameInstance->Input_MouseState_Custom(DIMK_LB) == KEY_STATE::TAP)
	{
		static_cast<CTerrain*>(pTerrain)->Terrain_Picking();
		_float4 pickPos = static_cast<CTerrain*>(pTerrain)->GetPickPosition();

		CTransform* pTransform = static_cast<CTransform*>(m_pSelectCube->Find_Component(L"com_transform"));
		if (nullptr == pTransform)
			return;

		pTransform->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&pickPos));
	}
}

void CMapTool::RenderLevelUI()
{
	if (nullptr == m_pSelectCube)
		return;

	CComponent* pComponent = nullptr;
	CTransform* pTransform = nullptr;

	ImGui::SetNextWindowSize(ImVec2(g_fDetailWidth, (_float)g_iWinSizeY));
	ImGui::Begin("Details", 0, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

	CLayer* Layer = m_pGameInstance->Find_Layer(TOOL_MAP, TEXT("layer_cube"));

	ImGui::Text("Cube Count");
	ImGui::SameLine();
	string strCubeSize = to_string(Layer->m_GameObjects.size());
	ImGui::Text(strCubeSize.c_str());

	pComponent = m_pSelectCube->Find_Component(L"com_transform");

	if (nullptr != pComponent)
		pTransform = static_cast<CTransform*>(pComponent);
	
	if (nullptr != pTransform)
	{
		ImGui::BeginChild("Transform", ImVec2(g_fDetailWidth, 300));
		_float3 scale, rotation, position;

		_vector vPosition = pTransform->Get_State(CTransform::STATE_POSITION);
		rotation = pTransform->Get_Angle();

		scale = pTransform->Get_Scale();

		XMStoreFloat3(&position, vPosition);
		ImGui::InputFloat3("Scale", (_float*)&scale);
		ImGui::InputFloat3("Rotation", (_float*)&rotation);
		ImGui::InputFloat3("Position", (_float*)&position);

		pTransform->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&position));
		//pTransform->SetRotation(XMLoadFloat3(&position));

		ImGui::EndChild();
	}

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
