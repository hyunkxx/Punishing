#include "pch.h"
#include "..\Public\Terrain.h"

#include "GameInstance.h"

CTerrain::CTerrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CTerrain::CTerrain(const CTerrain& rhs)
	: CGameObject(rhs)
{
}

HRESULT CTerrain::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTerrain::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

void CTerrain::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (pGameInstance->Input_MouseState_Custom(DIMK_LB) == KEY_STATE::TAP)
	{
		m_IsPick = Terrain_Picking();
	}
}

void CTerrain::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	if (nullptr != m_pRenderer)
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);
}

HRESULT CTerrain::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(Setup_ShaderResources()))
		return E_FAIL;

	m_pShader->Begin(0);
	m_pVIBuffer->Render();

	return S_OK;   
}

void CTerrain::RenderGUI()
{
}

_bool CTerrain::Terrain_Picking()
{
	_bool ret = false;

	CPipeLine* pPipeline = CPipeLine::GetInstance();

	_float4x4 matProj = pPipeline->Get_Transform_float4x4(CPipeLine::TS_PROJ);
	_float4 matView4x4;
	memcpy(&matView4x4, &(pPipeline->Get_Transform_float4x4_Inverse(CPipeLine::TS_VIEW).m[3][0]), sizeof(_float4));

	_matrix matView = pPipeline->Get_Transform_Matrix(CPipeLine::TS_VIEW);
	_matrix matProjInverse = pPipeline->Get_Transform_Matrix_Inverse(CPipeLine::TS_PROJ);
	_matrix matViewInverse = pPipeline->Get_Transform_Matrix_Inverse(CPipeLine::TS_VIEW);

	POINT mousePos;
	GetCursorPos(&mousePos);
	ScreenToClient(g_hWnd, &mousePos);

	_float3 vMouse;
	vMouse.x = mousePos.x / (g_iWinSizeX * 0.5f) - 1.f;
	vMouse.y = mousePos.y / -(g_iWinSizeY * 0.5f) + 1.f;
	vMouse.z = 0.f;

	_vector vMousePos = XMVector3TransformCoord(XMLoadFloat3(&vMouse), matProjInverse);
	
	_vector vRayPos = { 0.f, 0.f, 0.f }; // 뷰 스페이스 공간의 원점
	_vector vRayDir = XMVector3Normalize(vMousePos - vRayPos);

	vRayPos = XMVector3TransformCoord(vRayPos, matViewInverse);
	vRayDir = XMVector3TransformNormal(vRayDir, matViewInverse);

	origin, dir, vCam;
	vCam = pPipeline->Get_CamPosition(); //카메라 위치 디버그용
	XMStoreFloat4(&origin, vRayPos);
	XMStoreFloat4(&dir, vRayDir);

	CVIBuffer_Terrain::TERRAIN_SIZE terrainSize;
	ZeroMemory(&terrainSize, sizeof(terrainSize));
	terrainSize = m_pVIBuffer->GetTerrainSize();

	_float3* vertexPos = m_pVIBuffer->GetVertexPosition();

	m_fDistance = 1000.f;
	_float fDist = 1000.f;
	for (_uint i = 0; i < terrainSize.mZ - 1; ++i)
	{
		for (_uint j = 0; j < terrainSize.mX - 1; ++j)
		{
			_uint iIndex = i * terrainSize.mX + j;
			_uint iIndices[] = {
				iIndex + terrainSize.mX,
				iIndex + terrainSize.mX + 1,
				iIndex + 1,
				iIndex
			};//012 023
			
			_vector v1 = XMLoadFloat3(&vertexPos[iIndices[0]]);
			_vector v2 = XMLoadFloat3(&vertexPos[iIndices[1]]);
			_vector v3 = XMLoadFloat3(&vertexPos[iIndices[2]]);
			
			if (TriangleTests::Intersects(vRayPos, vRayDir, v1, v2, v3, fDist))
			{
				if (m_fDistance > fDist)
				{
					m_fDistance = fDist;
					ret = true;

					XMStoreFloat3(&m_v1, v1);
					XMStoreFloat3(&m_v2, v2);
					XMStoreFloat3(&m_v3, v3);

					XMStoreFloat4(&pickPos, vRayPos + vRayDir * fDist);
				}
			}

			v1 = XMLoadFloat3(&vertexPos[iIndices[0]]);
			v2 = XMLoadFloat3(&vertexPos[iIndices[2]]);
			v3 = XMLoadFloat3(&vertexPos[iIndices[3]]);

			if (TriangleTests::Intersects(vRayPos, vRayDir, v1, v2, v3, fDist))
			{
				if (m_fDistance > fDist)
				{
					m_fDistance = fDist;
					ret = true;

					XMStoreFloat3(&m_v1, v1);
					XMStoreFloat3(&m_v2, v2);
					XMStoreFloat3(&m_v3, v3);

					XMStoreFloat4(&pickPos, vRayPos + vRayDir * fDist);
				}
			}
		}
	}

	return ret;
}

HRESULT CTerrain::Add_Components()
{
	if (FAILED(__super::Add_Component(TOOL_STATIC, TEXT("proto_com_renderer"),
		TEXT("com_renderer"), (CComponent**)&m_pRenderer)))
		return E_FAIL;

	CTransform::TRANSFORM_DESC TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORM_DESC));

	TransformDesc.fMoveSpeed = 5.f;
	TransformDesc.fRotationSpeed = XMConvertToRadians(90.f);

	if (FAILED(__super::Add_Component(TOOL_STATIC, TEXT("proto_com_transform"),
		TEXT("com_transform"), (CComponent**)&m_pTransform)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(TOOL_STATIC, TEXT("proto_com_vibuffer_plane_terrain"),
		TEXT("com_vibuffer"), (CComponent**)&m_pVIBuffer)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(TOOL_STATIC, TEXT("proto_com_shader_phong"),
		TEXT("com_shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(TOOL_STATIC, TEXT("proto_com_texture_terrain"),
		TEXT("com_texture"), (CComponent**)&m_pTexture)))
		return E_FAIL;

	return S_OK;
}

HRESULT CTerrain::Setup_ShaderResources()
{
	if (nullptr == m_pShader)
		return E_FAIL;

	CPipeLine* pPipeline= CPipeLine::GetInstance();

	if (FAILED(m_pTransform->Setup_ShaderResource(m_pShader, "g_WorldMatrix")))
		return E_FAIL;
	
	if (FAILED(m_pShader->SetMatrix("g_ViewMatrix", &pPipeline->Get_Transform_float4x4(CPipeLine::TS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShader->SetMatrix("g_ProjMatrix", &pPipeline->Get_Transform_float4x4(CPipeLine::TS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShader->SetRawValue("g_vCamPosition", &pPipeline->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pTexture->Setup_ShaderResource(m_pShader, "g_DiffuseTexture", 0)))
		return E_FAIL;

	return S_OK;
}

CTerrain* CTerrain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTerrain* pInstance = new CTerrain(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTerrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTerrain::Clone(void* pArg)
{
	CTerrain* pInstance = new CTerrain(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTerrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTerrain::Free()
{
	__super::Free();

	Safe_Release(m_pTransform);
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pShader);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pTexture);
}
