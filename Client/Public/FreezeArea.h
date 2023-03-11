
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CModel;
class CShader;
class CBone;
class CModel;
END

BEGIN(Client)

class CFreezeArea final : public CGameObject
{
private:
	CFreezeArea(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CFreezeArea() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual void RenderGUI() override;

public:
	void SetPosition(_vector vPosition);
	void SetActive(_bool value) { m_bActive = true; }

private:
	HRESULT AddComponents();
	HRESULT SetupShaderResources();

public:
	static CFreezeArea* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

private:
	CRenderer* m_pRenderer = nullptr;
	CTransform* m_pTransform = nullptr;
	CModel* m_pModel = nullptr;
	CShader* m_pShader = nullptr;

private:
	class CTransform* m_pPlayerTransform = nullptr;

	_bool m_bActive = false;
	_bool m_bFinish = false;
	_float3 m_vPosition = { 0.f, 0.f, 0.f };
	_float3 m_vScale = { 0.f, 0.f, 0.f };
};

END