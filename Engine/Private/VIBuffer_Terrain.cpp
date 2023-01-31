#include "..\Public\VIBuffer_Terrain.h"

CVIBuffer_Terrain::CVIBuffer_Terrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CVIBuffer_Terrain::CVIBuffer_Terrain(const CVIBuffer_Terrain& rhs)
	: CVIBuffer(rhs)
	, m_dwVertexCountX(rhs.m_dwVertexCountX)
	, m_dwVertexCountZ(rhs.m_dwVertexCountZ)
{
}

HRESULT CVIBuffer_Terrain::Initialize_Prototype(const _tchar* pHeightMapFilePath)
{
	_ulong dwByte = 0;
	HANDLE hFile = CreateFile(pHeightMapFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (0 == hFile)
	{
		wstring message = pHeightMapFilePath;
		MESSAGE(message);
		return E_FAIL;
	}

	BITMAPFILEHEADER fh;
	BITMAPINFOHEADER ih;
	ReadFile(hFile, &fh, sizeof fh, &dwByte, nullptr);
	ReadFile(hFile, &ih, sizeof ih, &dwByte, nullptr);
	
	_ulong* pPixel = new _ulong[ih.biWidth * ih.biHeight];
	ReadFile(hFile, pPixel, sizeof(_ulong) * ih.biWidth * ih.biHeight, &dwByte, nullptr);
	CloseHandle(hFile);

	m_iStride = sizeof(VTXNORTEX);
	m_dwVertexCountX = ih.biWidth;
	m_dwVertexCountZ = ih.biHeight;
	m_iVerticesCount = m_dwVertexCountX * m_dwVertexCountZ;

	m_iPrimitiveIndexSize = sizeof(FACEINDICES32);
	m_iPrimitiveCount = (m_dwVertexCountX - 1) * (m_dwVertexCountZ - 1) * 2;
	m_iPrimitiveIndexCount = 3;
	m_iVertexBuffersCount = 1;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;


#pragma region VERTEXBUFFER
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iStride * m_iVerticesCount;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.StructureByteStride = m_iStride;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);

	VTXNORTEX* pVertices = new VTXNORTEX[m_iVerticesCount];
	ZeroMemory(pVertices, sizeof(VTXNORTEX) * m_iVerticesCount);

	for (_uint i = 0; i < m_dwVertexCountZ; ++i)
	{
		for (_uint j = 0; j < m_dwVertexCountX; ++j)
		{
			_uint iIndex = i * m_dwVertexCountX + j;

			pVertices[iIndex].vPosition = _float3((_float)j, (pPixel[iIndex] & 0x000000ff) / 15.0f, (_float)i);
			pVertices[iIndex].vNormal = _float3(0.0f, 0.f, 0.f);
			pVertices[iIndex].vTexUV = _float2((_float)j / (m_dwVertexCountX - 1.0f), i / (m_dwVertexCountZ - 1.0f));
		}
	}

	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_VertexBuffer()))
	{
		Safe_Delete_Array(pVertices);
		Safe_Delete_Array(pPixel);
		return E_FAIL;
	}

	Safe_Delete_Array(pVertices);
	Safe_Delete_Array(pPixel);

#pragma endregion

#pragma region INDEXBUFFER
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	/*내가 그릴려고하는 인덱스하나의 크기 * 인덱스의 갯수.
	== 삼각형을 그리기위한 인덱스 세개의 크기 * 삼각형의갯수 */;
	m_BufferDesc.ByteWidth = m_iPrimitiveIndexSize * m_iPrimitiveCount;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.StructureByteStride = 0;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);

	FACEINDICES32* pIndices = new FACEINDICES32[m_iPrimitiveCount];
	ZeroMemory(pIndices, sizeof(FACEINDICES32) * m_iPrimitiveCount);

	_uint		iNumFaces = 0;

	for (_uint i = 0; i < m_dwVertexCountZ - 1; ++i)
	{
		for (_uint j = 0; j < m_dwVertexCountX - 1; ++j)
		{
			/* 삼각형 두개를 정의하기위한 루프. */
			_uint		iIndex = i * m_dwVertexCountX + j;

			/* 사각혀으이 좌상, 우상, 우하, 좌하 인덱스를 의미한다. */
			_uint		iIndices[] = {
				iIndex + m_dwVertexCountX,
				iIndex + m_dwVertexCountX + 1,
				iIndex + 1,
				iIndex
			};

			/* 우상단 삼각형의 인덱스 세개를 구성하자. */
			pIndices[iNumFaces]._0 = iIndices[0];
			pIndices[iNumFaces]._1 = iIndices[1];
			pIndices[iNumFaces]._2 = iIndices[2];
			++iNumFaces;

			pIndices[iNumFaces]._0 = iIndices[0];
			pIndices[iNumFaces]._1 = iIndices[2];
			pIndices[iNumFaces]._2 = iIndices[3];
			++iNumFaces;
		}
	}

	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_IndexBuffer()))
	{
		Safe_Delete_Array(pIndices);
		return E_FAIL;
	}

	Safe_Delete_Array(pIndices);

#pragma endregion

	return S_OK;
}

HRESULT CVIBuffer_Terrain::Initialize(void* pArg)
{
	return S_OK;
}

CVIBuffer_Terrain* CVIBuffer_Terrain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pHeightMapFilePath)
{
	CVIBuffer_Terrain* pInstance = new CVIBuffer_Terrain(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pHeightMapFilePath)))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Terrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_Terrain::Clone(void* pArg)
{
	CVIBuffer_Terrain* pInstance = new CVIBuffer_Terrain(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_Terrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Terrain::Free()
{
	__super::Free();
}
