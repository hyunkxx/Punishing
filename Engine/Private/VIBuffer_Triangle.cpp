#include "..\Public\VIBuffer_Triangle.h"

CVIBuffer_Triangle::CVIBuffer_Triangle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CVIBuffer_Triangle::CVIBuffer_Triangle(const CVIBuffer_Triangle& rhs)
	: CVIBuffer(rhs)
{
}

HRESULT CVIBuffer_Triangle::Initialize_Prototype()
{
	m_iStride = sizeof(VTXTEX);
	m_iVertexCount = 3;
	m_iFaceIndexSize = sizeof(FACEINDICES16);
	m_iFaceCount = 1;
	m_iFaceIndexCount = 3;
	m_iVertexBuffersCount = 1;
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region 버텍스 버퍼 세팅
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iStride * m_iVertexCount;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.StructureByteStride = m_iStride;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);

	VTXTEX* pVertices = new VTXTEX[m_iVertexCount];
	ZeroMemory(pVertices, sizeof(VTXTEX) * m_iVertexCount);

	pVertices[0].vPosition = _float3(-0.5f, 0.5f, 0.f);
	pVertices[0].vTexUV = _float2(0.0f, 0.f);

	pVertices[1].vPosition = _float3(0.5f, 0.5f, 0.f);
	pVertices[1].vTexUV = _float2(1.f, 0.f);

	pVertices[2].vPosition = _float3(0.5f, -0.5f, 0.f);
	pVertices[2].vTexUV = _float2(1.f, 1.f);

	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_VertexBuffer()))
	{
		Safe_Delete_Array(pVertices);
		return E_FAIL;
	}

	Safe_Delete_Array(pVertices);

#pragma endregion


#pragma region 인덱스 버퍼 세팅
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iFaceIndexSize * m_iFaceCount;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.StructureByteStride = 0;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);

	FACEINDICES16* pIndices = new FACEINDICES16[m_iFaceCount];
	ZeroMemory(pIndices, sizeof(FACEINDICES16) * m_iFaceCount);

	pIndices[0]._0 = 0;
	pIndices[0]._1 = 1;
	pIndices[0]._2 = 2;

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

HRESULT CVIBuffer_Triangle::Initialize(void* pArg)
{

	return S_OK;
}

void CVIBuffer_Triangle::SetBuffer(_float3 v1, _float3 v2, _float3 v3)
{
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iStride * m_iVertexCount;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.StructureByteStride = m_iStride;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);

	VTXTEX* pVertices = new VTXTEX[m_iVertexCount];
	ZeroMemory(pVertices, sizeof(VTXTEX) * m_iVertexCount);

	pVertices[0].vPosition = v1;
	pVertices[0].vTexUV = _float2(0.0f, 0.f);

	pVertices[1].vPosition = v2;
	pVertices[1].vTexUV = _float2(1.f, 0.f);

	pVertices[2].vPosition = v3;
	pVertices[2].vTexUV = _float2(1.f, 1.f);

	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_VertexBuffer()))
	{
		Safe_Delete_Array(pVertices);
		return;
	}

	Safe_Delete_Array(pVertices);

#pragma endregion


#pragma region 인덱스 버퍼 세팅
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iFaceIndexSize * m_iFaceCount;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.StructureByteStride = 0;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);

	FACEINDICES16* pIndices = new FACEINDICES16[m_iFaceCount];
	ZeroMemory(pIndices, sizeof(FACEINDICES16) * m_iFaceCount);

	pIndices[0]._0 = 0;
	pIndices[0]._1 = 1;
	pIndices[0]._2 = 2;

	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_IndexBuffer()))
	{
		Safe_Delete_Array(pIndices);
		return;
	}

	Safe_Delete_Array(pIndices);

#pragma endregion

	return;
}

CVIBuffer_Triangle* CVIBuffer_Triangle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CVIBuffer_Triangle*	pInstance = new CVIBuffer_Triangle(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		wstring message = L"Failed to Create : CVIBuffer_Triangle";
		MESSAGE(message);
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_Triangle::Clone(void * pArg)
{
	CVIBuffer_Triangle*	pInstance = new CVIBuffer_Triangle(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		wstring message = L"Failed to Clone : CVIBuffer_Triangle";
		MESSAGE(message);
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Triangle::Free()
{
	__super::Free();
}