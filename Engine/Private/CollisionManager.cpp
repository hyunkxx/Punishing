#include "..\Public\CollisionManager.h"
#include "Collider.h"
#include "GameObject.h"

IMPLEMENT_SINGLETON(CCollisionManager)

CCollisionManager::CCollisionManager()
{
	m_BaseCollisions.reserve(100);
	m_BaseCollisions.reserve(100);
}

HRESULT CCollisionManager::AddCollider(CCollider* collider, _uint iLayerIndex)
{
	if (nullptr == collider)
		return E_FAIL;

	switch (iLayerIndex)
	{
	case 0:
		if (collider->IsActive())
			m_BaseCollisions.emplace_back(collider);
		break;
	case 1:
		if (collider->IsActive())
			m_BackgroundCollisions.emplace_back(collider);
		break;
	default:
		MSG_BOX("AddCollider Out Length");
		break;
	}


	return S_OK;
}

void CCollisionManager::PhysicsUpdate()
{
	// ���� ���� ���� �ִ°Ű����� ���߿� �������� j = i ���ƴ϶� �� ó������ �ٽõ����� ���̾��Է¹޾Ƽ� �������� src dest �Լ��� �ΰ�ȣ�� ����
 	for (_uint i = 0; i < m_BaseCollisions.size() ; ++i)
	{
		for (_uint j = i; j < m_BaseCollisions.size(); ++j)
		{
			if (nullptr == m_BaseCollisions[i] || nullptr == m_BaseCollisions[j])
				continue;

			CGameObject* srcObject = m_BaseCollisions[i]->GetOwner();
			CGameObject* destObject = m_BaseCollisions[j]->GetOwner();

			//gameObject �ڽ��� �ݶ��̴����� �浹 ����
			if (srcObject == destObject)
				continue;

			//���� Ÿ���̰� ���� Ÿ���� ������Ʈ�ϰ�� ����
			ISameObjectNoDetection* src = dynamic_cast<ISameObjectNoDetection*>(srcObject);
			ISameObjectNoDetection* dest = dynamic_cast<ISameObjectNoDetection*>(destObject);
			if (src && dest)
			{
				if (srcObject->GetID() == destObject->GetID())
					continue;
			}

			//�浹����
			if (m_BaseCollisions[i]->Collision(m_BaseCollisions[j]))
			{
				m_BaseCollisions[i]->SetCollision(true);
				m_BaseCollisions[j]->SetCollision(true);

				//�����浹 Enter
				if (!m_BaseCollisions[i]->IsHitCollider(m_BaseCollisions[j]))
				{
					IOnCollisionEnter* src = dynamic_cast<IOnCollisionEnter*>(srcObject);
					IOnCollisionEnter* dest = dynamic_cast<IOnCollisionEnter*>(destObject);

					if (src)
						src->OnCollisionEnter(m_BaseCollisions[i], m_BaseCollisions[j]);
					if (dest)
						dest->OnCollisionEnter(m_BaseCollisions[j], m_BaseCollisions[i]);

					//�浹���� ����Ʈ�� �߰�
					m_BaseCollisions[i]->AddHitCollider(m_BaseCollisions[j]);
					m_BaseCollisions[j]->AddHitCollider(m_BaseCollisions[i]);

				}
				//�浹�� Stay
				else
				{
					IOnCollisionStay* src = dynamic_cast<IOnCollisionStay*>(srcObject);
					IOnCollisionStay* dest = dynamic_cast<IOnCollisionStay*>(destObject);

					if (src)
						src->OnCollisionStay(m_BaseCollisions[i], m_BaseCollisions[j]);
					if (dest)
						dest->OnCollisionStay(m_BaseCollisions[j], m_BaseCollisions[i]);
				}
			}
			//�浹���� ����
			else
			{
				//�����浹 ����X Exit
				if (m_BaseCollisions[i]->IsHitCollider(m_BaseCollisions[j]))
				{
					IOnCollisionExit* src = dynamic_cast<IOnCollisionExit*>(srcObject);
					IOnCollisionExit* dest = dynamic_cast<IOnCollisionExit*>(destObject);

					if (src)
						src->OnCollisionExit(m_BaseCollisions[i], m_BaseCollisions[j]);
					if (dest)
						dest->OnCollisionExit(m_BaseCollisions[j], m_BaseCollisions[i]);

					//�浹���� ����Ʈ���� ����
					m_BaseCollisions[i]->EraseHitCollider(m_BaseCollisions[j]);
					m_BaseCollisions[j]->EraseHitCollider(m_BaseCollisions[i]);
				}

				//���ڸ����� Active�� �����ϴ� �ݶ��̴� �浹�߿� ���������� �̻��ϰ� �����ϴ¾� �־ �׽�Ʈ�غ����� ����
			}
		}
	}

	//��� �浹 �ߺ��ǰ� ȣ������ �ʰڴ� ����
	for (_uint i = 0; i < m_BackgroundCollisions.size(); ++i)
	{
		for (_uint j = i; j < m_BackgroundCollisions.size(); ++j)
		{
			if (nullptr == m_BackgroundCollisions[i] || nullptr == m_BackgroundCollisions[j])
				continue;


			CGameObject* srcObject = m_BackgroundCollisions[i]->GetOwner();
			CGameObject* destObject = m_BackgroundCollisions[j]->GetOwner();

			//gameObject �ڽ��� �ݶ��̴����� �浹 ����
			if (srcObject == destObject)
				continue;

			//���� Ÿ���̰� ���� Ÿ���� ������Ʈ�ϰ�� ����
			ISameObjectNoDetection* src = dynamic_cast<ISameObjectNoDetection*>(srcObject);
			ISameObjectNoDetection* dest = dynamic_cast<ISameObjectNoDetection*>(destObject);
			if (src && dest)
			{
				if (srcObject->GetID() && destObject->GetID())
					continue;
			}

			//�浹����
			if (m_BackgroundCollisions[i]->Collision(m_BackgroundCollisions[j]))
			{
				m_BackgroundCollisions[i]->SetCollision(true);
				m_BackgroundCollisions[j]->SetCollision(true);

				//�����浹 Enter
				if (!m_BackgroundCollisions[i]->IsHitCollider(m_BackgroundCollisions[j]))
				{
					IOnCollisionEnter* src = dynamic_cast<IOnCollisionEnter*>(srcObject);
					IOnCollisionEnter* dest = dynamic_cast<IOnCollisionEnter*>(destObject);

					if (src)
						src->OnCollisionEnter(m_BackgroundCollisions[i], m_BackgroundCollisions[j]);
					if (dest)
						dest->OnCollisionEnter(m_BackgroundCollisions[j], m_BackgroundCollisions[i]);

					//�浹���� ����Ʈ�� �߰�
					m_BackgroundCollisions[i]->AddHitCollider(m_BackgroundCollisions[j]);
					m_BackgroundCollisions[j]->AddHitCollider(m_BackgroundCollisions[i]);

				}
				//�浹�� Stay
				else
				{
					IOnCollisionStay* src = dynamic_cast<IOnCollisionStay*>(srcObject);
					IOnCollisionStay* dest = dynamic_cast<IOnCollisionStay*>(destObject);

					if (src)
						src->OnCollisionStay(m_BackgroundCollisions[i], m_BackgroundCollisions[j]);
					if (dest)
						dest->OnCollisionStay(m_BackgroundCollisions[j], m_BackgroundCollisions[i]);
				}
			}
			//�浹���� ����
			else
			{
				//�����浹 ����X Exit
				if (m_BackgroundCollisions[i]->IsHitCollider(m_BackgroundCollisions[j]))
				{
					IOnCollisionExit* src = dynamic_cast<IOnCollisionExit*>(srcObject);
					IOnCollisionExit* dest = dynamic_cast<IOnCollisionExit*>(destObject);

					if (src)
						src->OnCollisionExit(m_BackgroundCollisions[i], m_BackgroundCollisions[j]);
					if (dest)
						dest->OnCollisionExit(m_BackgroundCollisions[j], m_BackgroundCollisions[i]);

					//�浹���� ����Ʈ���� ����
					m_BackgroundCollisions[i]->EraseHitCollider(m_BackgroundCollisions[j]);
					m_BackgroundCollisions[j]->EraseHitCollider(m_BackgroundCollisions[i]);
				}

			}
		}
	}
}

void CCollisionManager::Render()
{
	if (!m_bDebugRender)
	{
		Clear();
		return;
	}

#ifdef _DEBUG
	for (auto& coll : m_BaseCollisions)
	{
		if(coll->IsVisible())
			coll->Render();
	}

	for (auto& coll : m_BackgroundCollisions)
	{
		if (coll->IsVisible())
			coll->Render();
	}
#endif

	Clear();
}

int CCollisionManager::GetHasCollisionCount()
{
	return (int)m_BaseCollisions.size();
}

void CCollisionManager::Clear()
{
	m_BaseCollisions.clear();
	m_BackgroundCollisions.clear();
}

void CCollisionManager::Free()
{
	m_BaseCollisions.clear();
	m_BackgroundCollisions.clear();
}
