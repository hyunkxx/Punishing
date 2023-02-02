#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Terrain final : public CVIBuffer
{
public:
	typedef struct tagTerrainSize
	{
		_ulong mX;
		_ulong mZ;
	}TERRAIN_SIZE;

protected:
	CVIBuffer_Terrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Terrain(const CVIBuffer_Terrain& rhs);
	virtual ~CVIBuffer_Terrain() = default;

public:
	virtual	HRESULT Initialize_Prototype(const _tchar* pHeightMapFilePath);
	virtual	HRESULT Initialize(void* pArg);
	_float3* GetVertexPosition() { return m_VertexPos; };

	TERRAIN_SIZE GetTerrainSize()
	{
		TERRAIN_SIZE size;
		ZeroMemory(&size, sizeof(TERRAIN_SIZE));

		size.mX = m_dwVertexCountX;
		size.mZ = m_dwVertexCountZ;
		return size;
	};

public:
	static CVIBuffer_Terrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pHeightMapFilePath);
	virtual CComponent* Clone(void* pArg = nullptr);
	virtual void Free() override;

private:
	_ulong m_dwVertexCountX = { 0 };
	_ulong m_dwVertexCountZ = { 0 };

	_float3* m_VertexPos = { nullptr };
};

END