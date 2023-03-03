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
	//�ߺ��ǰ� ȣ������ �ʰڴ�.
 	for (_uint i = 0; i < _collisions.size() ; ++i)
	{
		for (_uint j = i; j < _collisions.size(); ++j)
		{
			if (nullptr == _collisions[i] || nullptr == _collisions[j])
				continue;

			CGameObject* srcObject = _collisions[i]->GetOwner();
			CGameObject* destObject = _collisions[j]->GetOwner();

			//�����ְ� ������ Continue
			if (srcObject == destObject)
				continue;

			//�浹����
			if (_collisions[i]->Collision(_collisions[j]))
			{
				_collisions[i]->SetCollision(true);
				_collisions[j]->SetCollision(true);

				//�����浹 Enter
				if (!_collisions[i]->IsHitCollider(_collisions[j]))
				{
					IOnCollisionEnter* src = dynamic_cast<IOnCollisionEnter*>(srcObject);
					IOnCollisionEnter* dest = dynamic_cast<IOnCollisionEnter*>(destObject);

					if (src)
						src->OnCollisionEnter(_collisions[i], _collisions[j]);
					if (dest)
						dest->OnCollisionEnter(_collisions[j], _collisions[i]);

					//�浹���� ����Ʈ�� �߰�
					_collisions[i]->AddHitCollider(_collisions[j]);
					_collisions[j]->AddHitCollider(_collisions[i]);

				}
				//�浹�� Stay
				else
				{
					IOnCollisionStay* src = dynamic_cast<IOnCollisionStay*>(srcObject);
					IOnCollisionStay* dest = dynamic_cast<IOnCollisionStay*>(destObject);

					if (src)
						src->OnCollisionStay(_collisions[i], _collisions[j]);
					if (dest)
						dest->OnCollisionStay(_collisions[j], _collisions[i]);
				}
			}
			//�浹���� ����
			else
			{
				//�����浹 ����X Exit
				if (_collisions[i]->IsHitCollider(_collisions[j]))
				{
					IOnCollisionExit* src = dynamic_cast<IOnCollisionExit*>(srcObject);
					IOnCollisionExit* dest = dynamic_cast<IOnCollisionExit*>(destObject);

					if (src)
						src->OnCollisionExit(_collisions[i], _collisions[j]);
					if (dest)
						dest->OnCollisionExit(_collisions[j], _collisions[i]);

					//�浹���� ����Ʈ���� ����
					_collisions[i]->EraseHitCollider(_collisions[j]);
					_collisions[j]->EraseHitCollider(_collisions[i]);

				}

				_collisions[i]->SetCollision(false);
				_collisions[j]->SetCollision(false);
			}
		}
	}
}

void CCollisionManager::Render()
{
#ifdef _DEBUG
	for (auto& coll : _collisions)
		coll->Render();
#endif

	//Gameobject�� null���� null�̸� ����
	Clear();
}

void CCollisionManager::Clear()
{
	_collisions.clear();
}

void CCollisionManager::Free()
{
	_collisions.clear();
}
