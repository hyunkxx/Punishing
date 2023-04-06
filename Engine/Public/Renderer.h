#pragma once

#include "Component.h"


BEGIN(Engine)
class CGameObject;

class ENGINE_DLL CRenderer final : public CComponent
{
public:
	enum RENDER_GROUP { RENDER_PRIORITY, RENDER_NONALPHA, RENDER_NONLIGHT, RENDER_ALPHABLEND, RENDER_UI, RENDER_ENDING, RENDER_END };
private:
	CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CRenderer() = default;

public:
	virtual HRESULT Initialize_Prototype() override;

public:
	HRESULT Add_RenderGroup(RENDER_GROUP eRenderGroup, class CGameObject* pGameObject);
	void Draw();
	void SetBlurAmount(_float fPower);

private:
	void ZSort();

public:
	static CRenderer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

private:
	void RenderPriority();
	void RenderNonAlpha();
	void RenderNonLight();
	void RenderAlphaBlend();
	void RenderUI();
	void RenderEnding();//�̰� ī�޶󿡼� ���µ� �ϴ� ����

public:
	list<class CGameObject*> m_RenderObject[RENDER_END];
	list<class CGameObject*> m_AlphaRenderObject;

	typedef list<class CGameObject*> RENDER_OBJECT;

private:
	// Main�� Bloom�� �ռ��ؼ� ����۷� �����ϴ� ���̴�
	class CPostEffect* m_pPostEffect = nullptr;
	class CBlurEffect* m_pBlurEffect = nullptr; //RGB ��
	class CScreenBlurEffect* m_pScreenBlurEffect = nullptr; //��ũ����
	class CDistortionEffect* m_pDistortionEffect = nullptr; //Distortion ����Ʈ

	class CShader* m_pPostEffectShader = nullptr;
	class CShader* m_pBlurEffectShader = nullptr;
	class CShader* m_pScreenBlurEffectShader = nullptr;
	class CShader* m_pDistortionEffectShader = nullptr;

};

END

bool Compute(Engine::CGameObject* pSourObject, Engine::CGameObject* pDestObject);
