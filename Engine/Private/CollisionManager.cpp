#include "..\Public\CollisionManager.h"
#include "Collider.h"
#include "GameObject.h"

IMPLEMENT_SINGLETON(CCollisionManager)

HRESULT CCollisionManager::AddCollider(CCollider* collider)
{
	if (nullptr == collider)
		return E_FAIL;

	_collisions.push_back(collider);

	return S_OK;
}

void CCollisionManager::PhysicsUpdate()
{
	for (_uint i = 0; i < _collisions.size(); ++i)
	{
		_collisions[i]->Update();
	}

	//중복되게 호출하지 않겠다. j == i
 	for (_uint i = 0; i < _collisions.size() ; ++i)
	{
		for (_uint j = i; j < _collisions.size(); ++j)
		{
			if (nullptr == _collisions[i] || nullptr == _collisions[j])
				continue;

			CGameObject* srcObject = _collisions[i]->GetOwner();
			CGameObject* destObject = _collisions[j]->GetOwner();

			//소유주가 같으면 Continue
			if (srcObject == destObject)
				continue;

			//충돌했음
			if (_collisions[i]->Collision(_collisions[j]))
			{
				//최초 충돌
				if (!_collisions[i]->IsHitCollider(_collisions[j]))
				{
					IOnCollisionEnter* src = dynamic_cast<IOnCollisionEnter*>(srcObject);
					IOnCollisionEnter* dest = dynamic_cast<IOnCollisionEnter*>(destObject);

					if (src)
						src->OnCollisionEnter(_collisions[j]);
					if (dest)
						dest->OnCollisionEnter(_collisions[j]);

					//충돌중인 리스트에 추가
					_collisions[i]->AddHitCollider(_collisions[j]);
					_collisions[j]->AddHitCollider(_collisions[i]);
				}
				//기존에도 충돌
				else
				{
					IOnCollisionStay* src = dynamic_cast<IOnCollisionStay*>(srcObject);
					IOnCollisionStay* dest = dynamic_cast<IOnCollisionStay*>(destObject);

					if (src)
						src->OnCollisionStay(_collisions[j]);
					if (dest)
						dest->OnCollisionStay(_collisions[j]);
				}
			}
			//충돌하지 않았다면
			else
			{
				IOnCollisionExit* src = dynamic_cast<IOnCollisionExit*>(srcObject);
				IOnCollisionExit* dest = dynamic_cast<IOnCollisionExit*>(destObject);

				if (src)
					src->OnCollisionExit(_collisions[j]);
				if (dest)
					dest->OnCollisionExit(_collisions[j]);

				//충돌중인 리스트에서 삭제
				_collisions[i]->EraseHitCollider(_collisions[j]);
				_collisions[j]->EraseHitCollider(_collisions[i]);
			}
		}
	}
}

void CCollisionManager::Render()
{
	for (auto& coll : _collisions)
		coll->Render();
}

void CCollisionManager::Clear()
{
	_collisions.clear();
}

void CCollisionManager::Free()
{
	_collisions.clear();
}
