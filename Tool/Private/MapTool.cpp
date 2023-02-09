#include "pch.h"
#include "..\Public\MapTool.h"
#include "GameInstance.h"

#include "Layer.h"
#include "GameObject.h"
#include "GameObject_Tool.h"

#include "Cube.h"
#include "ToolCamera.h"
#include "Terrain.h"

#include "SaveLoader.h"

CMapTool::CMapTool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
	, m_pGameInstance { CGameInstance::GetInstance() }
	, m_pSaveLoader{ CSaveLoader::GetInstance() }
{
	ZeroMemory(&bClose, sizeof(_bool) * GUI_END);
	ZeroMemory(&m_ObjectDesc, sizeof(OBJ_DESC));
	m_ObjectDesc.vScale = { 1.f,1.f,1.f };
}

HRESULT CMapTool::Initialize()
{
	if (FAILED(Ready_Layer_BackGround(TEXT("layer_terrain"))))
		return E_FAIL;

	if (FAILED(Ready_Layre_Camera(TEXT("layer_camera"))))
		return E_FAIL;

	if (FAILED(Ready_Layre_Object(TEXT("layer_object"))))
		return E_FAIL;

	return S_OK;
}

void CMapTool::Tick(_double TimeDelta)
{
	if (m_pGameInstance->Input_KeyState_Custom(DIK_F1) == KEY_STATE::TAP)
	{
		for (_uint i = 0; i < GUI_END; ++i)
			bClose[i] = !bClose[i];
	}

	SelectCubeMovement(TimeDelta);

	if (m_pGameInstance->Input_MouseState_Custom(DIMK_LB) == KEY_STATE::TAP)
	{
		Cube_Picking();
	}

	if (m_pGameInstance->Input_KeyState_Custom(DIK_DELETE) == KEY_STATE::TAP)
	{
		DeleteSelectObject();
	}

	if (m_pGameInstance->Input_KeyState_Custom(DIK_LCONTROL) == KEY_STATE::HOLD
		&& m_pGameInstance->Input_KeyState_Custom(DIK_S) == KEY_STATE::TAP)
	{
		SaveObjects();
	}

	if (m_pGameInstance->Input_KeyState_Custom(DIK_LCONTROL) == KEY_STATE::HOLD
		&& m_pGameInstance->Input_KeyState_Custom(DIK_O) == KEY_STATE::TAP)
	{
		LoadObjects();
	}

	MoveToTerrainPickPosition();

	//Layer Clear
	if (m_pGameInstance->Input_KeyState_Custom(DIK_F11) == KEY_STATE::TAP)
	{
		m_pGameInstance->LayerClear(TOOL_MAP, L"layer_object");
		m_iObjectIndex = 0;
		m_ObjectDesc.mID = 0;
	}
}

HRESULT CMapTool::Ready_Layer_BackGround(const _tchar* pLayerTag)
{
	if(nullptr == m_pGameInstance->Add_GameObject(TOOL_MAP, TEXT("proto_obj_terrain"), pLayerTag, L"terrain"))
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

	if (nullptr == m_pGameInstance->Add_GameObject(TOOL_MAP, TEXT("proto_obj_tool_camera"), pLayerTag, L"tool_camera", &CameraDesc))
		return E_FAIL;

	return S_OK;
}

HRESULT CMapTool::Ready_Layre_Object(const _tchar * pLayerTag)
{

	return S_OK;
}

void CMapTool::CreatorUI()
{
	if (!bClose[GUI_CREATOR])
		return;

	ImGui::SetNextWindowPos({ 0.f, 0.f });
	ImGui::SetNextWindowSize({ 300.f, 145.f });

	ImGui::Begin("Creator", &bClose[GUI_CREATOR], ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);
	ImGui::TextColored({ 0.41f, 0.90f, 0.95f, 1.f }, "Creator");
	//오브젝트 타입 지정
	const char* ObjectType[OBJ_END] = { "OBJ_CUBE", "OBJ_PROB", "OBJ_BUILDING", "OBJ_NPC", "OBJ_MONSTER", "OBJ_PLAYER" };
	static const char* current_item = nullptr;
	bool is_selected = false;
	if (ImGui::BeginCombo("Object Type", current_item))
	{
		for (int n = 0; n < IM_ARRAYSIZE(ObjectType); n++)
		{
			bool is_selected = (current_item == ObjectType[n]);
			if (ImGui::Selectable(ObjectType[n], is_selected))
				current_item = ObjectType[n];
			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

	if (m_pSelectObject)
		m_pSelectObject->SetObjectDesc(m_ObjectDesc);

	//트렌스폼 세팅
	ImGui::InputFloat3("Scale", (_float*)&m_ObjectDesc.vScale, "%.3f");
	ImGui::InputFloat3("Rotation", (_float*)&m_ObjectDesc.vRotation, "%.3f");
	ImGui::InputFloat3("Position", (_float*)&m_ObjectDesc.vPosition, "%.3f");

	//Scale Clamp -> 스케일 엄청 얇을때 피킹이 안됨
	if (m_ObjectDesc.vScale.x <= 0.005f)
		m_ObjectDesc.vScale.x = 0.005f;
	if (m_ObjectDesc.vScale.y <= 0.005f)
		m_ObjectDesc.vScale.y = 0.005f;
	if (m_ObjectDesc.vScale.z <= 0.005f)
		m_ObjectDesc.vScale.z = 0.005f;

	//오브젝트 삭제
	if (ImGui::Button("Create", { 90.f, 18.f }))
	{
		if (current_item == nullptr)
		{
			ImGui::End();
			return;
		}

		if (current_item == ObjectType[OBJ_CUBE])
		{
			m_ObjectDesc.mID = m_iObjectIndex;
			CreateObject(m_ObjectDesc);
		}
		else
		{

		}
	}

	//오브젝트 삭제
	ImGui::SameLine();
	if (ImGui::Button("Delete", { 90.f, 18.f }))
	{
		if (nullptr == m_pSelectObject)
		{
			ImGui::End();
			return;
		}
		m_pSelectObject->Destroy();
		m_pSelectObject = nullptr;
	}

	//데이터 적용
	ImGui::SameLine();
	if (ImGui::Button("Apply", { 90.f, 18.f }))
	{
		if (nullptr == m_pSelectObject)
		{
			ImGui::End();
			return;
		}

		if (m_pSelectObject)
			m_pSelectObject->ApplyObjectDesc(m_ObjectDesc);
	}

	ImGui::End();
}

void CMapTool::EditStateUI()
{
	if (!bClose[GUI_STATE])
		return;

	ImGui::SetNextWindowPos({ 300.f, 0.f });
	ImGui::SetNextWindowSize({ 300.f, 145.f });
	ImGui::Begin("EditState", &bClose[GUI_STATE], ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);
	ImGui::TextColored({ 0.41f, 0.90f, 0.95f, 1.f }, "Editor State");

	ImGui::End();
}

void CMapTool::CreateObject(const OBJ_DESC& objectDesc)
{
	wstring tag = L"level_object_" + m_iObjectIndex;

	switch (objectDesc.eType)
	{
	case OBJ_CUBE:
	{
		CGameObject_Tool* pGameObject = static_cast<CGameObject_Tool*>(m_pGameInstance->Add_GameObject(TOOL_MAP, L"proto_obj_cube", L"layer_object", tag, (OBJ_DESC*)&objectDesc));
		break;
	}
	case OBJ_PROB:
		m_pGameInstance->Add_GameObject(TOOL_MAP, L"proto_obj_cube", L"layer_object", tag);
		break;
	case OBJ_BUILDING:
		m_pGameInstance->Add_GameObject(TOOL_MAP, L"proto_obj_cube", L"layer_object", tag);
		break;
	case OBJ_NPC:
		m_pGameInstance->Add_GameObject(TOOL_MAP, L"proto_obj_cube", L"layer_object", tag);
		break;
	case OBJ_MONSTER:
		m_pGameInstance->Add_GameObject(TOOL_MAP, L"proto_obj_cube", L"layer_object", tag);
		break;
	case OBJ_PLAYER:
		m_pGameInstance->Add_GameObject(TOOL_MAP, L"proto_obj_cube", L"layer_object", tag);
		break;
	default:
		return;
	}

	++m_iObjectIndex;
}

void CMapTool::DeleteSelectObject()
{
	if (nullptr != m_pSelectObject)
	{
		m_pSelectObject->Destroy();
		m_pSelectObject = nullptr;
	}
}

void CMapTool::Cube_Picking()
{
	CLayer* pLayer = m_pGameInstance->Find_Layer(TOOL_MAP, L"layer_object");
	if (nullptr == pLayer || pLayer->m_GameObjects.size() == 0)
		return;

	_float fDistance = FLT_MAX;

	auto GameObjects= pLayer->m_GameObjects;


	for (auto& iter = GameObjects.begin() ; iter != GameObjects.end() ; ++iter)
	{
		CGameObject_Tool* pObject = dynamic_cast<CGameObject_Tool*>(iter->second);
		if (pObject)
		{
			float fRayDistance = pObject->PickObject();

			if (0.f < fRayDistance)
			{
				if (fDistance > fRayDistance)
				{
					if (m_pSelectObject != nullptr)
					{
						m_pSelectObject->UnSelect();
					}

					m_pSelectObject = pObject;
					fDistance = fRayDistance;
				}
			}
		}
	}

	if (m_pSelectObject != nullptr)
	{
		m_pSelectObject->Select();
		m_ObjectDesc = m_pSelectObject->GetObjectDesc();
	}
}

void CMapTool::SelectCubeMovement(_double TimeDelta)
{
	if (nullptr == m_pSelectObject)
		return;

	if (m_pGameInstance->Input_KeyState_Custom(DIK_Q) == KEY_STATE::HOLD)
	{
		CTransform* pTransform = static_cast<CTransform*>(m_pSelectObject->Find_Component(L"com_transform"));
		m_ObjectDesc.vRotation.y -= (_float)TimeDelta * 50.f;
		pTransform->SetRotationXYZ(m_ObjectDesc.vRotation);
		m_pSelectObject->ApplyObjectDesc(m_ObjectDesc);
	}
	
	if (m_pGameInstance->Input_KeyState_Custom(DIK_E) == KEY_STATE::HOLD)
	{
		CTransform* pTransform = static_cast<CTransform*>(m_pSelectObject->Find_Component(L"com_transform"));
		m_ObjectDesc.vRotation.y += (_float)TimeDelta * 50.f;
		pTransform->SetRotationXYZ(m_ObjectDesc.vRotation);
		m_pSelectObject->ApplyObjectDesc(m_ObjectDesc);
	}

	if (m_pGameInstance->Input_KeyState_Custom(DIK_R) == KEY_STATE::HOLD)
	{
		CTransform* pTransform = static_cast<CTransform*>(m_pSelectObject->Find_Component(L"com_transform"));
		m_ObjectDesc.vRotation.x += (_float)TimeDelta * 50.f;
		pTransform->SetRotationXYZ(m_ObjectDesc.vRotation);
		m_pSelectObject->ApplyObjectDesc(m_ObjectDesc);
	}

	if (m_pGameInstance->Input_KeyState_Custom(DIK_F) == KEY_STATE::HOLD)
	{
		CTransform* pTransform = static_cast<CTransform*>(m_pSelectObject->Find_Component(L"com_transform"));
		m_ObjectDesc.vRotation.x -= (_float)TimeDelta * 50.f;
		pTransform->SetRotationXYZ(m_ObjectDesc.vRotation);
		m_pSelectObject->ApplyObjectDesc(m_ObjectDesc);
	}
}

void CMapTool::MoveToTerrainPickPosition()
{
	CGameObject* pTerrain = m_pGameInstance->Find_GameObject(TOOL_MAP, L"layer_terrain", L"terrain");
	if (nullptr == pTerrain)
		return;

	if (nullptr == m_pSelectObject)
		return;

	if (m_pGameInstance->Input_KeyState_Custom(DIK_LCONTROL) == KEY_STATE::HOLD
		&& m_pGameInstance->Input_MouseState_Custom(DIMK_LB) == KEY_STATE::TAP)
	{
		static_cast<CTerrain*>(pTerrain)->Terrain_Picking();
		_float4 pickPos = static_cast<CTerrain*>(pTerrain)->GetPickPosition();

		CTransform* pTransform = static_cast<CTransform*>(m_pSelectObject->Find_Component(L"com_transform"));
		if (nullptr == pTransform)
			return;

		//피킹 지점으로 오브젝트 트렌스폼 수정
		pTransform->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&pickPos));
		memcpy(&m_ObjectDesc.vPosition, &pickPos, sizeof(_float3));
	}
}

void CMapTool::SaveObjects()
{
	CLayer* pLayer = m_pGameInstance->Find_Layer(TOOL_MAP, L"layer_object");
	if (nullptr == pLayer)
	{
		MSG_BOX("Failed Save Layer == nullptr");
		return;
	}
	m_pSaveLoader->SaveObjects(pLayer);
}

void CMapTool::LoadObjects()
{
	_uint ObjectCount;
	OBJ_DESC* pObjectDesc = nullptr;
	
	if (FAILED(m_pSaveLoader->LoadObjects(&pObjectDesc, ObjectCount)))
		return;

	for (_uint i = 0; i < ObjectCount; ++i)
	{
		pObjectDesc[i].mID;

		CreateObject(pObjectDesc[i]);
	}

	Safe_Delete_Array(pObjectDesc);
}

void CMapTool::RenderLevelUI()
{
	CreatorUI();
	EditStateUI();
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

	Safe_Release(m_pSaveLoader);
}
