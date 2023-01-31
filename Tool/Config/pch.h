#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN

#include "Engine_Defines.h"

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
};

extern HINSTANCE g_hInst;
extern HWND g_hWnd;

using namespace Tool;