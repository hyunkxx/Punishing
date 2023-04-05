#pragma once

namespace Engine
{
	typedef struct tagKeyFrame
	{
		XMFLOAT3 vScale;
		XMFLOAT4 vRotation;
		XMFLOAT3 vPosition;
		double Time;
	}KEY_FRAME;

	typedef struct tagAnimationData
	{
		const char* szName;
		unsigned int ChannelCount;
		vector<class CChannel*>* pChannels;
	}PREV_DATA;

	typedef struct tagMaterial
	{
		class CTexture* pMaterialTexture[AI_TEXTURE_TYPE_MAX] = { nullptr };
	}MATERIAL;

	typedef struct tagLight_Desc
	{
		enum TYPE { TYPE_DIRECTIONAL, TYPE_POINT, TYPE_END };
		TYPE eLightType = TYPE_END;
		XMFLOAT4 vDirection;
		XMFLOAT4 vPosition;
		float fRange;

		XMFLOAT4 vDiffuse;
		XMFLOAT4 vAmbient;
		XMFLOAT4 vSpecular;

	}LIGHT_DESC;

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


	typedef struct tagVertex_Position_Color
	{
		XMFLOAT3		vPosition;
	}VTXPOS;

	typedef struct ENGINE_DLL tagVertex_Position_Color_Declaration
	{
		const static unsigned int						iNumElements = 1;
		static D3D11_INPUT_ELEMENT_DESC					Elements[1];
	}VTXPOS_DECLARATION;

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

	typedef struct tagVertex_Anim_Model
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexUV;
		XMFLOAT3		vTangent;

		XMUINT4			vBlendIndices;
		XMFLOAT4		vBlendWeights;

	}VTXANIMMODEL;

	typedef struct ENGINE_DLL tagVertex_Anim_Model_Declaration
	{
		const static unsigned int						ElementCount = 6;
		static D3D11_INPUT_ELEMENT_DESC					Elements[ElementCount];
	}VTXANIMMODEL_DECLARATION;

	typedef struct tagVertex_Matrix
	{
		XMFLOAT4		vRight;
		XMFLOAT4		vUp;
		XMFLOAT4		vLook;
		XMFLOAT4		vPosition;
	}VTXMATRIX;

	typedef struct ENGINE_DLL tagVertex_Instance_Declaration
	{
		const static unsigned int						ElementCount = 6;
		static D3D11_INPUT_ELEMENT_DESC					Elements[ElementCount];
	}VTXINSTANCE_DECLARATION;

	typedef struct tagVertex_Point
	{
		XMFLOAT3		vPosition;
		XMFLOAT2		vPSize;
	}VTXPOINT;

	typedef struct ENGINE_DLL tagVertex_Point_Instance_Declaration
	{
		const static unsigned int						ElementCount = 6;
		static D3D11_INPUT_ELEMENT_DESC					Elements[ElementCount];
	}VTXPOINTINSTANCE_DECLARATION;
}