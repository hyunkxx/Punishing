#include "..\Public\VIBuffer_Rect_Instance.h"

CVIBuffer_Rect_Instance::CVIBuffer_Rect_Instance(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CVIBuffer_Rect_Instance::CVIBuffer_Rect_Instance(const CVIBuffer_Rect_Instance & rhs)
	: CVIBuffer(rhs)
	, m_pVBInstance(rhs.m_pVBInstance)
	, m_iInstanceCount(rhs.m_iInstanceCount)
	, m_iStrideInstance(rhs.m_iStrideInstance)
	, m_pSpeed(rhs.m_pSpeed)
	, m_pOldHeight(rhs.m_pOldHeight)
{
	Safe_AddRef(m_pVBInstance);
}

HRESULT CVIBuffer_Rect_Instance::Initialize_Prototype(_float fWidth, _float fHeight, _float fDepth, _float fMinSpeed, _float fMaxSpeed, _uint iInstanceCount)
{
	m_pSpeed = new _float[iInstanceCount];

	for (int i = 0; i < iInstanceCount; ++i)
		m_pSpeed[i] = rand() % ((_int)(fMaxSpeed - fMinSpeed) + 1) + fMinSpeed;

	m_iInstanceCount = iInstanceCount;
	m_iStrideInstance = sizeof(VTXMATRIX);
	m_iStride = sizeof(VTXTEX);
	m_iVertexCount = 4;
	m_iFaceIndexSize = sizeof(FACEINDICES16);
	m_iFaceCount = 2 * m_iInstanceCount;
	m_iFaceIndexCount = 3;
	m_iVertexBuffersCount = 2;
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VERTEXBUFFER_SETUP
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iStride * m_iVertexCount;

	/*D3D11_USAGE_DYNAMIC : CPU가 접근하여 이 공간에 값을 수정할 수 있도록 한다. */
	/*D3D11_USAGE_DEAFULT : CPU가 접근하여 이 공간에 값을 수정할 수 있도록 하지 않는다. */
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT /*D3D11_USAGE_DYNAMIC*/;
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

	pVertices[3].vPosition = _float3(-0.5f, -0.5f, 0.f);
	pVertices[3].vTexUV = _float2(0.0f, 1.f);

	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

#pragma endregion
#pragma region INDEXBUFFER_SETUP
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iFaceIndexSize * m_iFaceCount;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.StructureByteStride = 0;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);

	FACEINDICES16* pIndices = new FACEINDICES16[m_iFaceCount];
	ZeroMemory(pIndices, sizeof FACEINDICES16 * m_iFaceCount);

	_uint iFaceCount = 0;

	for (_uint i = 0; i < m_iInstanceCount; ++i)
	{
		pIndices[iFaceCount]._0 = 0;
		pIndices[iFaceCount]._1 = 1;
		pIndices[iFaceCount]._2 = 2;
		++iFaceCount;

		pIndices[iFaceCount]._0 = 0;
		pIndices[iFaceCount]._1 = 2;
		pIndices[iFaceCount]._2 = 3;
		++iFaceCount;
	}

	m_SubResourceData.pSysMem = pIndices;
	if (FAILED(__super::Create_IndexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

#pragma endregion
#pragma region INSTANCEBUFFER_SETUP
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iStrideInstance * m_iInstanceCount;

	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.StructureByteStride = m_iStrideInstance;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);

	VTXMATRIX* pInstanceVertices = new VTXMATRIX[m_iInstanceCount];
	ZeroMemory(pInstanceVertices, sizeof(VTXMATRIX) * m_iInstanceCount);

	m_pOldHeight = new _float[m_iInstanceCount];
	for (_uint i = 0; i < m_iInstanceCount; ++i)
	{
		pInstanceVertices[i].vRight = _float4(1.f, 0.f, 0.f, 0.f);
		pInstanceVertices[i].vUp = _float4(0.f, 1.f, 0.f, 0.f);
		pInstanceVertices[i].vLook = _float4(0.f, 0.f, 1.f, 0.f);

		m_pOldHeight[i] = rand() % (_int(fHeight) + 1) - (_int(fHeight) >> 1);
		pInstanceVertices[i].vPosition = 
			_float4(rand() % (_int(fWidth) + 1) - (_int(fWidth) >> 1),
			m_pOldHeight[i],
			rand() % (_int(fDepth) + 1) - (_int(fDepth) >> 1),
			1.f);
	}

	m_SubResourceData.pSysMem = pInstanceVertices;

	if (FAILED(m_pDevice->CreateBuffer(&m_BufferDesc, &m_SubResourceData, &m_pVBInstance)))
		return E_FAIL;

	Safe_Delete_Array(pInstanceVertices);
#pragma endregion

	return S_OK;
}

HRESULT CVIBuffer_Rect_Instance::Initialize(void * pArg)
{
	return S_OK;
}

HRESULT CVIBuffer_Rect_Instance::Render()
{
	if (nullptr == m_pContext)
		return E_FAIL;

	ID3D11Buffer* pVertexBuffers[] = {
		m_pVB,
		m_pVBInstance
	};

	_uint iStrides[] = {
		m_iStride,
		m_iStrideInstance
	};

	_uint iOffsets[] = {
		0,
		0
	};

	m_pContext->IASetVertexBuffers(0, m_iVertexBuffersCount, pVertexBuffers, iStrides, iOffsets);
	m_pContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);
	m_pContext->IASetPrimitiveTopology(m_eTopology);
	m_pContext->DrawIndexedInstanced(m_iFaceIndexCount * 2,	m_iInstanceCount, 0, 0, 0);

	return S_OK;
}

void CVIBuffer_Rect_Instance::Update(_double TimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE SubResource;
	ZeroMemory(&SubResource, sizeof SubResource);

	if (FAILED(m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource)))
		return;

	for (_uint i = 0; i < m_iInstanceCount; ++i)
	{
		((VTXMATRIX*)SubResource.pData)[i].vPosition.y -= m_pSpeed[i] * TimeDelta;

		if (((VTXMATRIX*)SubResource.pData)[i].vPosition.y < -30.f)
			((VTXMATRIX*)SubResource.pData)[i].vPosition.y = m_pOldHeight[i];
	}

	m_pContext->Unmap(m_pVBInstance, 0);
}

CVIBuffer_Rect_Instance * CVIBuffer_Rect_Instance::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, _float fWidth, _float fHeight, _float fDepth, _float fMinSpeed, _float fMaxSpeed, _uint iInstanceCount)
{
	CVIBuffer_Rect_Instance* pInstance = new CVIBuffer_Rect_Instance(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(fWidth, fHeight, fDepth, fMinSpeed, fMaxSpeed, iInstanceCount)))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Rect_Instance");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CVIBuffer_Rect_Instance::Clone(void * pArg)
{
	CVIBuffer_Rect_Instance* pInstance = new CVIBuffer_Rect_Instance(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_Rect_Instance");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Rect_Instance::Free()
{
	__super::Free();

	if (false == m_isClone)
	{
		Safe_Delete_Array(m_pSpeed);
		Safe_Delete_Array(m_pOldHeight);
	}

	Safe_Release(m_pVBInstance);
}
