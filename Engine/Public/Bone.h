#pragma once

#include "Base.h"

BEGIN(Engine)

class CBone final : public CBase
{
private:
	CBone() = default;
	virtual ~CBone() = default;

public:
	const char* GetName() const { return m_szName; }
	_float4x4 GetOffSetMatrix() const { return m_OffsetMatrix; }
	_float4x4 GetCombinedMatrix() const { return m_CombinedMatrix; }
	void SetOffsetMatrix(_fmatrix OffSetMatrix) { XMStoreFloat4x4(&m_OffsetMatrix, OffSetMatrix); }
	void SetTransformationMatrix(_fmatrix TransformationMatrix) { XMStoreFloat4x4(&m_TransformationMatrix, TransformationMatrix); }

public:
	HRESULT Initialize(aiNode* pAINode, CBone* pParent);
	void InvalidateCombinedMatrix();

public:
	static CBone* Create(aiNode* pAINode, CBone* pParent);
	virtual void Free() override;
	
private:
	CBone* m_pParent = { nullptr };
	char m_szName[MAX_PATH] = "";
	_float4x4 m_OffsetMatrix;
	_float4x4 m_TransformationMatrix;
	_float4x4 m_CombinedMatrix;

};

END