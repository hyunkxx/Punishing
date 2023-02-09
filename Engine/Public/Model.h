#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CModel final : public CComponent
{
public:
	enum class MESH_TYPE{ STATIC_MESH, SKELETAK_MESH, TYPE_END };
public:
	CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModel(const CModel& rhs);
	virtual ~CModel() = default;

public:
	virtual HRESULT Initialize_Prototype(MESH_TYPE eType, const char* pPath);
	virtual HRESULT Initialize(void* pArg = nullptr);

public:
	HRESULT InitalizeMesh();
	HRESULT Render();

public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MESH_TYPE eType, const char* pPath);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

private:
	Assimp::Importer m_Importer;
	const aiScene* m_pAIScene = { nullptr };

	_uint m_iMeshCount = { 0 };
	vector<class CMesh*> m_vecMesh;

};

END