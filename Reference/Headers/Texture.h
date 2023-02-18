#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CTexture : public CComponent
{
private:
	CTexture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTexture(const CTexture& rhs);
	virtual ~CTexture() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pTexturePath, _uint iTextureCount);
	virtual HRESULT Initialize(void* pArg = nullptr) override;

public:
	HRESULT Setup_ShaderResource(class CShader* pShaderComponent, const char* pConstantName, _uint iTextureIndex = 0);
	HRESULT Setup_ShaderResourceArray(class CShader* pShaderComponent, const char* pConstantName);

public:
	static CTexture* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pTexturePath, _uint iTextureCount = 1);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

private: // 쉐이더 전역변수에 전달될 수 있는 타입
	vector<ID3D11ShaderResourceView*> m_Textures;
	typedef vector<ID3D11ShaderResourceView*> TEXTURES;

};

END