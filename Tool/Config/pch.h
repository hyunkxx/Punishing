#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN

#include "Engine_Defines.h"

#include <limits>
#include <windows.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

namespace Tool 
{
	const static _uint g_iWinSizeX = 1280;
	const static _uint g_iWinSizeY = 720;

	enum TOOL_TYPE { TOOL_STATIC, TOOL_MAP, TOOL_END };

	const static float g_fDetailWidth = 400.f;
	const static float g_fDetailHeight = 720.f;

	enum OBJ_TYPE { OBJ_CUBE, OBJ_PROB, OBJ_BUILDING, OBJ_NPC, OBJ_MONSTER, OBJ_PLAYER, OBJ_END };

	typedef struct tagObject_Desc
	{
		OBJ_TYPE eType;			//오브젝트 타입 (실제 객체)
		_uint mID;
		_float3 vScale;
		_float3 vRotation;
		_float3 vPosition;
	}OBJ_DESC;
};

extern HINSTANCE g_hInst;
extern HWND g_hWnd;

using namespace Tool;