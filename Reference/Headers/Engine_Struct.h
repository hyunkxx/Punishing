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

	// ���� � ���� �׸������� ���� �������
	typedef struct tagVertex_Position_TexCoord
	{
		XMFLOAT3		vPosition;
		XMFLOAT2		vTexUV;
	}VTXTEX;
	// ���� ��ҿ����� ��ü���� ����
	typedef struct ENGINE_DLL tagVertex_Position_TexCoord_Declaration
	{
		const static unsigned int						ElementCount = 2;
		static D3D11_INPUT_ELEMENT_DESC					Elements[2];
	}VTXTEX_DECLARATION;

	typedef struct tagVertex_Position_Normal_TexCoord
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexUV;
	}VTXNORTEX;

	typedef struct ENGINE_DLL tagVertex_Position_Normal_TexCoord_Declaration
	{
		const static unsigned int						ElementCount = 3;
		static D3D11_INPUT_ELEMENT_DESC					Elements[3];
	}VTXNORTEX_DECLARATION;

}