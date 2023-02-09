#pragma once

namespace Engine
{
	typedef struct tagGraphic_Desc
	{
		enum WIN_MODE { MODE_FULL, MODE_WINDOW, MODE_END };
		WIN_MODE eMode;
		unsigned int iWinSizeX;
		unsigned int iWinSizeY;
		HINSTANCE hInstance;
		HWND hWnd;
	}GRAPHIC_DESC;

	typedef struct tagFaceIndices16
	{
		unsigned short _0, _1, _2;
	}FACEINDICES16;

	typedef struct tagFaceIndices32
	{
		unsigned long _0, _1, _2;
	}FACEINDICES32;

	// 내가 어떤 모델을 그리기위한 정점 구성요소
	typedef struct tagVertex_Position_TexCoord
	{
		XMFLOAT3 vPosition;
		XMFLOAT2 vTexUV;
	}VTXTEX;
	// 정점 요소에대한 구체적인 정보
	typedef struct ENGINE_DLL tagVertex_Position_TexCoord_Declaration
	{
		const static unsigned int						ElementCount = 2;
		static D3D11_INPUT_ELEMENT_DESC					Elements[2];
	}VTXTEX_DECLARATION;

	typedef struct tagVertex_Position_Normal_TexCoord
	{
		XMFLOAT3 vPosition;
		XMFLOAT3 vNormal;
		XMFLOAT2 vTexUV;
	}VTXNORTEX;

	typedef struct ENGINE_DLL tagVertex_Position_Normal_TexCoord_Declaration
	{
		const static unsigned int						ElementCount = 3;
		static D3D11_INPUT_ELEMENT_DESC					Elements[ElementCount];
	}VTXNORTEX_DECLARATION;

	// Cube
	typedef struct tagVertex_Position_Normal_TexCoord_CUBE
	{
		XMFLOAT3 vPosition;
		XMFLOAT3 vNormal;
		XMFLOAT3 vTexUV;
	}VTXCUBE;

	typedef struct ENGINE_DLL tagVertex_Position_Normal_Cube_Declaration
	{
		const static unsigned int						ElementCount = 2;
		static D3D11_INPUT_ELEMENT_DESC					Elements[ElementCount];
	}VTXCUBE_DECLARATION;
	
	// Model
	typedef struct tagVertex_Model
	{
		XMFLOAT3 vPosition;
		XMFLOAT3 vNormal;
		XMFLOAT2 vTexUV;
		XMFLOAT3 vTangent;
	}VTXMODEL;

	typedef struct ENGINE_DLL tagVertex_Model_Declaration
	{
		const static unsigned int						ElementCount = 4;
		static D3D11_INPUT_ELEMENT_DESC					Elements[ElementCount];
	}VTXMODEL_DECLARATION;
}