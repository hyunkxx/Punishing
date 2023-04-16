#include "pch.h"
#include "..\Public\Sprite.h"

#include "GameInstance.h"

void CSprite::SetSpeed(float fSpeed)
{
	if (fSpeed <= 0.f)
		fSpeed = 0.f;
	else
		m_fSpriteSpeed = fSpeed;

}

void CSprite::SetupSprite(CTexture* pTexture, _float2 spriteSize, SPRITE_PLAY ePlayType)
{
	m_ePlayType = ePlayType;
	m_pSpriteTexture = pTexture;
	m_SpriteSize = spriteSize;
}

HRESULT CSprite::SetupSpriteShaderResource(SPRITE eType)
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	assert(m_pSpriteTexture);

	CPipeLine* pPipeline = CPipeLine::GetInstance();

	switch (eType)
	{
	case Client::CSprite::SPRITE::NONE:
		if (FAILED(m_pSpriteShader->SetMatrix("g_WorldMatrix", &m_pTransform->Get_WorldMatrix())))
			return E_FAIL;
		break;
	case Client::CSprite::SPRITE::BILLBOARD:
	{		
		_float4x4 InversViewMatrix = pPipeline->Get_Transform_float4x4_Inverse(CPipeLine::TS_VIEW);
		_float3 vScale = m_pTransform->Get_Scale();
		_float4x4 worldMatrix = m_pTransform->Get_WorldMatrix();
		InversViewMatrix._41 = worldMatrix._41;
		InversViewMatrix._42 = worldMatrix._42;
		InversViewMatrix._43 = worldMatrix._43;
		m_pTransform->Set_WorldMatrix(InversViewMatrix);
		//m_pTransform->Set_Scale(vScale);

		m_pTransform->Set_State(CTransform::STATE_RIGHT, m_pTransform->Get_State(CTransform::STATE_RIGHT) * vScale.x);
		m_pTransform->Set_State(CTransform::STATE_UP, m_pTransform->Get_State(CTransform::STATE_UP) * vScale.y);
		m_pTransform->Set_State(CTransform::STATE_LOOK, m_pTransform->Get_State(CTransform::STATE_LOOK) * vScale.z);

		if (FAILED(m_pSpriteShader->SetMatrix("g_WorldMatrix", &m_pTransform->Get_WorldMatrix())))
			return E_FAIL;
		break;
	}
	default:
		break;
	}

	if (FAILED(m_pSpriteShader->SetMatrix("g_ViewMatrix", &pPipeline->Get_Transform_float4x4(CPipeLine::TS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pSpriteShader->SetMatrix("g_ProjMatrix", &pPipeline->Get_Transform_float4x4(CPipeLine::TS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pSpriteShader->SetRawValue("g_SpriteXY", &m_SpriteSize, sizeof(_float2))))
		return E_FAIL;
	if (FAILED(m_pSpriteShader->SetRawValue("g_CurrentCount", &m_fCurrentSpriteIndex, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pSpriteTexture->Setup_ShaderResource(m_pSpriteShader, "g_DiffuseTexture")))
		return E_FAIL;

	return S_OK;
}

CSprite::CSprite(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CSprite::CSprite(const CSprite & rhs)
	: CGameObject(rhs)
{
}

HRESULT CSprite::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSprite::Initialize(void * pArg)
{
	if (FAILED(AddComponents()))
		return E_FAIL;
	
	memset(&m_SpriteSize, 1.f, sizeof(_float2));

	return S_OK;
}

void CSprite::Tick(_double TimeDelta)
{
	if (!m_bRender)
		return;

	CGameObject::Tick(TimeDelta);

	m_fSpriteAcc += TimeDelta;
	if (m_fSpriteAcc >= m_fSpriteSpeed)
	{
		m_fSpriteAcc = 0.f;
		if (m_fCurrentSpriteIndex < (m_SpriteSize.x * m_SpriteSize.y) - 1)
		{
			m_fCurrentSpriteIndex += 1.f;
		}
		else
		{
			switch (m_ePlayType)
			{
			case Client::CSprite::SPRITE_PLAY::ONE:
				m_bRender = false;
				m_fCurrentSpriteIndex = 0.f;
				break;
			case Client::CSprite::SPRITE_PLAY::LOOP:
				m_fCurrentSpriteIndex = 0.f;
				break;
			default:
				break;
			}
			
		}
	}
}

void CSprite::LateTick(_double TimeDelta)
{
	if (!m_bRender)
		return;

	CGameObject::LateTick(TimeDelta);

	if(m_bRender)
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_EFFECT, this);
}

HRESULT CSprite::Render()
{
	if (FAILED(CGameObject::Render()))
		return E_FAIL;

	m_pVIBufferRect->Render();

	return S_OK;
}

void CSprite::RenderGUI()
{
}

HRESULT CSprite::AddComponents()
{
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, L"proto_com_renderer", L"com_renderer", (CComponent**)&m_pRenderer)))
		return E_FAIL;
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, L"proto_com_transform", L"com_transform", (CComponent**)&m_pTransform)))
		return E_FAIL;
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, L"proto_com_shader_sprite", L"com_sprite", (CComponent**)&m_pSpriteShader)))
		return E_FAIL;
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, L"proto_com_vibuffer_rect", L"com_vibuffer_rect", (CComponent**)&m_pVIBufferRect)))
		return E_FAIL;

	return S_OK;
}

CSprite * CSprite::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CSprite*pInstance = new CSprite(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CSprite");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CSprite::Clone(void * pArg)
{
	CSprite* pInstance = new CSprite(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CSprite");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSprite::Free()
{
	CGameObject::Free();

	Safe_Release(m_pRenderer);
	Safe_Release(m_pTransform);
	Safe_Release(m_pSpriteShader);
}
