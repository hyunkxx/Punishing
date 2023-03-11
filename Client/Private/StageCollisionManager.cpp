#include "pch.h"
#include "..\Public\StageCollisionManager.h"

#include "GameInstance.h"

IMPLEMENT_SINGLETON(CStageCollisionManager)

void CStageCollisionManager::AddWall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	if (m_isActive == false)
		return;

	CGameInstance* pInstance = CGameInstance::GetInstance();
	if (pInstance == nullptr)
		return;

	wstring strObjectName = L"collision" + to_wstring(m_Walls.size());
	CWall* pWall = static_cast<CWall*>(pInstance->Add_GameObject(LEVEL_GAMEPLAY, L"proto_obj_wall", L"layer_collision", strObjectName.c_str()));

	if(pWall)
		m_Walls.push_back(pWall);
}

void CStageCollisionManager::DeleteObject()
{
}

void CStageCollisionManager::Tick(_double TimeDelta)
{
	if (m_isActive == false)
		return;

	CGameInstance* pInstance = CGameInstance::GetInstance();
	if (pInstance != nullptr)
	{
		if (pInstance->Input_KeyState_Custom(DIK_PGUP) == KEY_STATE::TAP)
		{
			if (m_iCurrentIndex < m_Walls.size() - 1)
			{
				CCollider* pCollider = static_cast<CCollider*>(m_Walls[m_iCurrentIndex]->Find_Component(L"com_collider"));
				pCollider->SetColor(_float4(1.f, 1.f, 1.f, 1.f));
				m_iCurrentIndex++;				
			}
		}

		else if (pInstance->Input_KeyState_Custom(DIK_PGDN) == KEY_STATE::TAP)
		{
			if (m_iCurrentIndex > 0)
			{
				CCollider* pCollider = static_cast<CCollider*>(m_Walls[m_iCurrentIndex]->Find_Component(L"com_collider"));
				pCollider->SetColor(_float4(1.f, 1.f, 1.f, 1.f));
				m_iCurrentIndex--;
			}
		}
	}

	if (pInstance->Input_KeyState_Custom(DIK_LCONTROL) == KEY_STATE::HOLD &&
		pInstance->Input_KeyState_Custom(DIK_S) == KEY_STATE::TAP)
	{
		SaveCollisionData();
	}

	if (!m_Walls.empty())
	{
		CCollider* pCollider = static_cast<CCollider*>(m_Walls[m_iCurrentIndex]->Find_Component(L"com_collider"));
		//pCollider->IsColl() ? pCollider->SetColor(_float4(1.f, 0.f, 0.f, 1.f)) : pCollider->SetColor(_float4(1.f, 1.f, 0.f, 1.f));
		pCollider->SetColor(_float4(1.f, 1.f, 0.f, 1.f));
	}

	for (auto& pWall : m_Walls)
	{
		if (pWall)
		{
			pWall->Tick(TimeDelta);
			pWall->LateTick(TimeDelta);
		}
	}
}

void CStageCollisionManager::RenderGUI()
{
	if (m_isActive == false)
		return;

	if (m_Walls.empty()) 
		return;

	ImGui::Begin("stage");
	
	//const char* ObjectType[LEVEL_END] = { "GamePlay", "BossRoom" };
	//static const char* current_item = nullptr;
	//bool is_selected = false;
	//if (ImGui::BeginCombo("Object Type", current_item))
	//{
	//	for (int n = 0; n < IM_ARRAYSIZE(ObjectType); n++)
	//	{
	//		bool is_selected = (current_item == ObjectType[n]);
	//		if (ImGui::Selectable(ObjectType[n], is_selected))
	//			current_item = ObjectType[n];
	//		if (is_selected)
	//			ImGui::SetItemDefaultFocus();
	//	}
	//	ImGui::EndCombo();
	//}

	string strIndex = "Current Collision Index  "+ to_string(m_iCurrentIndex);
	ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f),strIndex.c_str());

	CWall::CUBE_DESC& CubeDesc = m_Walls[m_iCurrentIndex]->GetDesc();
	ImGui::DragFloat("Scale",(_float*)&CubeDesc.vScale.x, 0.01f, 1.f, 100.f);
	CubeDesc.vScale.z = CubeDesc.vScale.x;
	CubeDesc.vScale.y = 1.f;

	ImGui::DragFloat3("vRotaion", (_float*)&CubeDesc.vRotation, 0.01f, 0.f, 360.f);
	ImGui::DragFloat3("vPosition", (_float*)&CubeDesc.vPosition, 0.01f, -1000.f, 1000.f);
	
	//if (ImGui::Button("Apply"))
	//{
		m_Walls[m_iCurrentIndex]->SetScale(CubeDesc.vScale);
		m_Walls[m_iCurrentIndex]->SetRotation(CubeDesc.vRotation);
		m_Walls[m_iCurrentIndex]->SetPosition(CubeDesc.vPosition);
	//}

	ImGui::End();
}

HRESULT CStageCollisionManager::LoadCollisionData(const wchar_t * pPath, vector<CWall::CUBE_DESC>* CubeDescs_out, _uint* iCollisionCount_out)
{
	_tchar filePathName[100] = L"";
	_tchar lpstrFile[100] = L"";
	lstrcpy(lpstrFile, pPath);

	static _tchar filter[] = L"All file\0*.*\0Text file\0*.txt\0FBX file\0*.fbx";

	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	HANDLE hRead = CreateFile(lpstrFile, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (hRead == INVALID_HANDLE_VALUE)
	{
		MSG_BOX("failed hRead open");
		return E_FAIL;
	}

	DWORD dwRead = 0;
	ReadFile(hRead, iCollisionCount_out, sizeof(_uint), &dwRead, NULL);

	CWall::CUBE_DESC CubeDesc;
	for (_uint i = 0; i < *iCollisionCount_out; ++i)
	{
		ZeroMemory(&CubeDesc, sizeof CubeDesc);
		ReadFile(hRead, &CubeDesc, sizeof(CWall::CUBE_DESC), &dwRead, NULL);
		CubeDescs_out->push_back(CubeDesc);
	}

	CloseHandle(hRead);

	return S_OK;
}

HRESULT CStageCollisionManager::SaveCollisionData()
{
	if (m_isActive == false)
		return S_OK;

	_uint ObjectCount = static_cast<_uint>(m_Walls.size());

	OPENFILENAME OFN;
	_tchar filePathName[100] = L"";
	_tchar lpstrFile[100] = L"";
	static _tchar filter[] = L"Binary file\0*.bin";

	ZeroMemory(&OFN, sizeof(OPENFILENAME));
	OFN.lStructSize = sizeof(OPENFILENAME);
	OFN.hwndOwner = g_hWnd;
	OFN.lpstrFilter = filter;
	OFN.lpstrFile = lpstrFile;
	OFN.nMaxFile = 100;
	OFN.lpstrInitialDir = L"";

	if (GetSaveFileName(&OFN) != 0)
	{
		DWORD dwRead = 0;

		wstring fileName = OFN.lpstrFile;

		HANDLE hOpen = CreateFile(fileName.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hOpen == INVALID_HANDLE_VALUE)
		{
			MSG_BOX("Failed Opend Handle");
			return E_FAIL;
		}

		//[ ÄÝ¸®Àü °¹¼ö ] [ CUBE_DESC ]
		WriteFile(hOpen, &ObjectCount, sizeof(_uint), &dwRead, NULL);

		for (auto& pWall : m_Walls)
		{
			CWall::CUBE_DESC cubeDesc = static_cast<CWall*>(pWall)->GetDesc();
			WriteFile(hOpen, &cubeDesc, sizeof(CWall::CUBE_DESC), &dwRead, NULL);
		}

		CloseHandle(hOpen);
	}

	return S_OK;
}

void CStageCollisionManager::Free()
{
	for (auto& pWall : m_Walls)
		Safe_Release(pWall);

	m_Walls.clear();
}
