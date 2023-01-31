#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer abstract : public CComponent
{
protected:
	CVIBuffer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer(const CVIBuffer& rhs);
	virtual ~CVIBuffer() = default;

public:
	virtual HRESULT Initialize_Protorype();
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT Render();

protected:
	HRESULT Create_VertexBuffer();
	HRESULT Create_IndexBuffer();

public:
	virtual CComponent* Clone(void* pArg = nullptr) = 0;
	virtual void Free() override;

protected:
	ID3D11Buffer*				m_pVB = { nullptr };
	ID3D11Buffer*				m_pIB = { nullptr };

	_uint						m_iStride = { 0 };				//정점 하나의 바이트 크기
	_uint						m_iVerticesCount = { 0 };		//기본도형(삼각형)의 정점 갯수
	_uint						m_iPrimitiveIndexSize = { 0 };	//기본도형(삼각형)의 인덱스 사이즈 16 or 32
	_uint						m_iPrimitiveCount = { 0 };		//그려지는 기본도형(삼각형) 갯수
	_uint						m_iPrimitiveIndexCount = { 0 }; //그려지는 기본도형(삼각형) 인덱스 갯수
	_uint						m_iVertexBuffersCount = { 0 };	//정점 버퍼 갯수
	DXGI_FORMAT					m_eIndexFormat;
	D3D11_PRIMITIVE_TOPOLOGY	m_eTopology;

	D3D11_BUFFER_DESC			m_BufferDesc;
	D3D11_SUBRESOURCE_DATA		m_SubResourceData;

};

END