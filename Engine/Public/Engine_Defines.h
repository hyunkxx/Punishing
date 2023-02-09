#pragma once

#define DIRECTINPUT_VERSION 0x0800

#pragma warning (disable : 4005)
#pragma warning (disable : 4251)

//ImGui
#include "../../Tool/ImGui/imgui.h"
#include "../../Tool/ImGui/imgui_impl_win32.h"
#include "../../Tool/ImGui/imgui_impl_dx11.h"

// Assimp
#include "Assimp/scene.h"
#include "Assimp/Importer.hpp"
#include "Assimp/postprocess.h"

// DirectX
#include <d3d11.h>
#include <dinput.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <DirectXCollision.h>
#include <process.h>
#include <d3dcompiler.h>
#include <typeinfo.h>
#include "Effects11/d3dx11effect.h"
#include "DirectXTK/DDSTextureLoader.h"
#include "DirectXTK/WICTextureLoader.h"

// C++ & STL
#include "commdlg.h"
#include <assert.h>
#include <cassert>
#include <list>
#include <vector>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <string>
#include <cstring>

// Utility
#include <time.h>

using namespace std;
using namespace DirectX;

// Project
#include "Engine_Macro.h"
#include "Engine_Struct.h"
#include "Engine_Template.h"
#include "Engine_Typedef.h"

namespace Engine
{
	enum MOUSE_KEYSTATE { DIMK_LB, DIMK_RB, DIMK_WHEEL, DIMK_END };
	enum MOUSE_MOVESTATE { DIMM_X, DIMM_Y, DIMM_WHEEL, DIMM_END };
	enum KEY_STATE { HOLD, TAP, AWAY, NONE };
}

#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifndef DBG_NEW 
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
#define new DBG_NEW
#endif

#endif

using namespace Engine;