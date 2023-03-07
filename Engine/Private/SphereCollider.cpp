#include "..\Public\SphereCollider.h"
#include "DebugDraw.h"
#include "GameObject.h"
#include "OBBCollider.h"
#include "Transform.h"
#include "PipeLine.h"

CSphereCollider::CSphereCollider(ID3D11Device * device, ID3D11DeviceContext * context)
	: CCollider(device, context)
{
}

CSphereCollider::CSphereCollider(const CSphereCollider & rhs)
	: CCollider(rhs)
{
	_sphere = nullptr;
	_sphereOriginal = nullptr;
}

HRESULT CSphereCollider::InitializePrototype(ID3D11Device * device, ID3D11DeviceContext* context)
{
	_type = COLL_TYPE::COLL_SPHERE;

	_batch = new PrimitiveBatch<VertexPositionColor>(m_pContext);
	_effect = new BasicEffect(m_pDevice);
	_effect->SetVertexColorEnabled(true);

	const void* shaderByteCode = nullptr;
	size_t shaderByteColdeLength = 0;

	_effect->GetVertexShaderBytecode(&shaderByteCode, &shaderByteColdeLength);

	if (FAILED(m_pDevice->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount, shaderByteCode, shaderByteColdeLength, &_inputLayout)))
		return E_FAIL;

	return S_OK;
}

HRESULT CSphereCollider::Initialize(void * arg)
{
	//if (nullptr != arg)
	//	memcpy(&_collDesc, arg, sizeof(COLLIDER_DESC));

	if (nullptr != arg)
	{
		_collDesc.owner = static_cast<COLLIDER_DESC*>(arg)->owner;
		_collDesc.vCenter = static_cast<COLLIDER_DESC*>(arg)->vCenter;
		_collDesc.vExtants = static_cast<COLLIDER_DESC*>(arg)->vExtants;
		_collDesc.vRotaion = static_cast<COLLIDER_DESC*>(arg)->vRotaion;
	}

	SetOwner(_collDesc.owner);

	if (!_collDesc.owner)
	{
		MSG_BOX("CSphere.Owner == nullptr");
		return E_FAIL;
	}

	_matrix scaleMatrix, rotationMatrix, translationMatrix;
	scaleMatrix = XMMatrixScaling(_collDesc.vExtants.x, _collDesc.vExtants.y, _collDesc.vExtants.z);
	rotationMatrix = XMMatrixRotationX(_collDesc.vRotaion.x) * XMMatrixRotationY(_collDesc.vRotaion.y) * XMMatrixRotationZ(_collDesc.vRotaion.z);
	translationMatrix = XMMatrixTranslation(_collDesc.vCenter.x, _collDesc.vCenter.y, _collDesc.vCenter.z);

	_matrix transformMatrix = XMMatrixIdentity();

	_sphere = new BoundingSphere(_float3(0.f, 0.f, 0.f), 0.5f);
	transformMatrix = scaleMatrix * rotationMatrix * translationMatrix;
	_sphere->Transform(*_sphere, transformMatrix);
	_sphereOriginal = new BoundingSphere(*_sphere);

	return S_OK;
}

void CSphereCollider::Update(_matrix transformMatrix)
{
	if (!m_isActive)
		return;

	_isColl = false;
	_sphereOriginal->Transform(*_sphere, transformMatrix);
}

_bool CSphereCollider::Collision(CCollider * targetCollider)
{
	if (!m_isActive)
		return false;

	_bool ret = false;

	if (COLL_SPHERE == targetCollider->GetType())
		ret = _sphere->Intersects(*(static_cast<CSphereCollider*>(targetCollider)->_sphere));
	else if (COLL_AABB == targetCollider->GetType())
		ret = _sphere->Intersects(*(static_cast<CSphereCollider*>(targetCollider)->_sphere));
	else if (COLL_OBB == targetCollider->GetType())
		ret = _sphere->Intersects(*(static_cast<COBBCollider*>(targetCollider)->_obb));

	return ret;
}

void CSphereCollider::Render()
{
	if (!m_isActive)
		return;

	m_pContext->IASetInputLayout(_inputLayout);
	CPipeLine* pipeline = CPipeLine::GetInstance();

	_effect->SetWorld(XMMatrixIdentity());
	_effect->SetView(pipeline->Get_Transform_Matrix(CPipeLine::TS_VIEW));
	_effect->SetProjection(pipeline->Get_Transform_Matrix(CPipeLine::TS_PROJ));
	
	_effect->Apply(m_pContext);

	_batch->Begin();
	_vector color = _isColl == true ? XMVectorSet(1.0f, 0.f, 0.f, 1.f) : XMLoadFloat4(&_color);
	
	DX::Draw(_batch, *_sphere, color);

	_batch->End();
}

CSphereCollider * CSphereCollider::Create(ID3D11Device * device, ID3D11DeviceContext * context)
{
	CSphereCollider* instance = new CSphereCollider(device, context);
	if (FAILED(instance->InitializePrototype(device, context)))
	{
		MSG_BOX("failed to created : CSphereCollider");
		Safe_Release(instance);
	}

	return instance;
}

CComponent * CSphereCollider::Clone(void * arg)
{
	CSphereCollider* instance = new CSphereCollider(*this);
	if (FAILED(instance->Initialize(arg)))
	{
		MSG_BOX("failed to cloned : CSphereCollider");
		Safe_Release(instance);
	}

	return instance;
}

void CSphereCollider::Free()
{
	__super::Free();

	Safe_Delete(_sphere);
	Safe_Delete(_sphereOriginal);
}
