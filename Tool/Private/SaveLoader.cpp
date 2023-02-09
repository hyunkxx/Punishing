#include "pch.h"
#include "..\Public\SaveLoader.h"

#include "GameInstance.h"

#include "Layer.h"
#include "GameObject.h"
#include "GameObject_Tool.h"

IMPLEMENT_SINGLETON(CSaveLoader)

CSaveLoader::CSaveLoader()
{
}

HRESULT CSaveLoader::SaveObjects(CLayer* pLayer)
{
	_uint ObjectCount = static_cast<_uint>(pLayer->m_GameObjects.size());

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

		HANDLE hOpen = CreateFile(fileName.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL,	NULL);
		if (hOpen == INVALID_HANDLE_VALUE)
		{
			MSG_BOX("Failed Opend Handle");
			return E_FAIL;
		}

		WriteFile(hOpen, &ObjectCount, sizeof(_uint), &dwRead, NULL);
		
		for (auto& pGameObject : pLayer->m_GameObjects)
		{
			OBJ_DESC objDesc = static_cast<CGameObject_Tool*>(pGameObject.second)->GetObjectDesc();
			WriteFile(hOpen, &objDesc, sizeof(OBJ_DESC), &dwRead, NULL);
		}

		CloseHandle(hOpen);
	}

	return S_OK;
}

HRESULT CSaveLoader::LoadObjects(OBJ_DESC** pObjectDesc, _uint& iObjectCount_out)
{
	OPENFILENAME OFN;
	_tchar filePathName[100] = L"";
	_tchar lpstrFile[100] = L"";
	static _tchar filter[] = L"All file\0*.*\0Text file\0*.txt\0FBX file\0*.fbx";

	ZeroMemory(&OFN, sizeof(OPENFILENAME));
	OFN.lStructSize = sizeof(OPENFILENAME);
	OFN.hwndOwner = g_hWnd;
	OFN.lpstrFilter = filter;
	OFN.lpstrFile = lpstrFile;
	OFN.nMaxFile = 100;
	OFN.lpstrInitialDir = L"";

	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (GetOpenFileName(&OFN) != 0)
	{
		HANDLE hRead = CreateFile(OFN.lpstrFile, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
		if (hRead == INVALID_HANDLE_VALUE)
		{
			MSG_BOX("failed hRead open");
			return E_FAIL;
		}

		DWORD dwRead = 0;
		ReadFile(hRead, &iObjectCount_out, sizeof(_uint), &dwRead, NULL);

		*pObjectDesc = new OBJ_DESC[iObjectCount_out];
		for (_uint i = 0; i < iObjectCount_out; ++i)
		{
			//(*pObjectDesc)[i].mID = i;
			ReadFile(hRead, &(*pObjectDesc)[i], sizeof(OBJ_DESC), &dwRead, NULL);
		}

		CloseHandle(hRead);
	}
	return S_OK;
}

void CSaveLoader::Free()
{
}
