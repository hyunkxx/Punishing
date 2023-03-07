#include "pch.h"
#include "..\Public\Boss.h"

CBoss::CBoss(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CBoss::CBoss(const CBoss & rhs)
	: CGameObject(rhs)
{
}

HRESULT CBoss::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBoss::Initialize(void * pArg)
{
	return S_OK;
}

void CBoss::Tick(_double TimeDelta)
{
}

void CBoss::LateTick(_double TimeDelta)
{
}

HRESULT CBoss::Render()
{
	return S_OK;
}

void CBoss::RenderGUI()
{
}

HRESULT CBoss::AddComponents()
{
	return S_OK;
}

HRESULT CBoss::SetupShaderResources()
{
	return S_OK;
}

CBoss * CBoss::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	return nullptr;
}

CGameObject * CBoss::Clone(void * pArg)
{
	return nullptr;
}

void CBoss::Free()
{
	__super::Free();
}
