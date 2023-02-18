#pragma once

#include "VIBuffer.h"
#include "Model.h"

BEGIN(Engine)

class ENGINE_DLL CMesh final : public CVIBuffer
{
private:
	CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMesh(const CMesh& rhs);
	virtual ~CMesh() = default;

public:
	virtual	HRESULT Initialize_Prototype(CModel::MESH_TYPE eType, const aiMesh* pAIMesh, class CModel* pModel, _fmatrix LocalMatrix);
	virtual HRESULT Initialize(void* pArg);

private:
	HRESULT InitStaticMeshVertexBuffer(const aiMesh* pAIMesh, _fmatrix LocalMatrix);
	HRESULT InitSkeletalMeshVertexBuffer(const aiMesh* pAIMesh, class CModel* pModel);

public:
	_uint GetMaterialIndex() const { return m_iMaterialIndex; }
	void GetBoneMatrices(_float4x4* pMeshBoneMatrices);

public:
	static CMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::MESH_TYPE eType, const aiMesh* pAIMesh, class CModel* pModel, _fmatrix LocalMatrix);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

private:
	_uint m_iMaterialIndex = { 0 };

	// ¸Þ½¬¿¡ ¿µÇâÀ» ÁÖ´Â »ÀÀÇ °¹¼ö¿Í »À
	_uint m_iBoneCount = { 0 };
	vector<class CBone*> m_Bones;

};

END