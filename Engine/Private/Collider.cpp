#include "..\Public\Collider.h"


CCollider::CCollider(ID3D11Device * device, ID3D11DeviceContext * context)
	: CComponent(device, context)
{
}

CCollider::CCollider(const CCollider & rhs)
	: CComponent(rhs)
	, _collDesc(rhs._collDesc)
	, _inputLayout(rhs._inputLayout)
	, _isColl(rhs._isColl)
	, _batch(rhs._batch)
	, _effect(rhs._effect)
{
	_owner = nullptr;

	Safe_AddRef(_inputLayout);
}

void CCollider::Free()
{
	if (false == m_isClone)
	{
		Safe_Delete(_batch);
		Safe_Delete(_effect);
	}

	Safe_Release(_inputLayout);
}

void CCollider::EraseHitCollider(CCollider * collider)
{
	for (auto iter = hitCollider.begin(); iter != hitCollider.end();)
	{
		if ((*iter) == collider)
		{
			iter = hitCollider.erase(iter);
		}
		else
			iter++;
	}
}

void CCollider::AddHitCollider(CCollider * collider)
{
	hitCollider.push_back(collider);
}

_bool CCollider::IsHitCollider(CCollider * collider)
{
	for (auto& coll : hitCollider)
	{
		if (coll == collider)
			return true;
	}

	return false;
}
