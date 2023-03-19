#include "pch.h"
#include "..\Public\SkillBallSystem.h"

#include "GameInstance.h"

IMPLEMENT_SINGLETON(CSkillBallSystem)

void CSkillBallSystem::Clear()
{
	for (auto gameObject : m_pSkills)
		gameObject->Destroy();

	m_iSkillCount = 0;
	m_pSkills.clear();

	while (!m_pReserveSkills.empty())
	{
		CSkillBase* pSkill = m_pReserveSkills.front();
		pSkill->Destroy();
		m_pReserveSkills.pop();
	}
}

void CSkillBallSystem::PushReadyTimer(_double DeltaTime)
{
	if (m_bSkillPushReady)
		return;

	m_fReadyTimer += DeltaTime;
	if (m_fReadyTimer >= m_fReadyTimeOut)
	{
		m_bSkillPushReady = true;
		m_fReadyTimer = 0.f;
	}
}

HRESULT CSkillBallSystem::PushSkill(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CSkillBase::TYPE eType)
{
	CGameInstance* pInstance = CGameInstance::GetInstance();

	if (!m_bSkillPushReady)
		return S_OK;

	if (m_pSkills.size() < 8)
	{
		m_bSkillPushReady = false;
		wstring strName = to_wstring(CSkillBase::GetSkillID());
		CSkillBase* pSkill = static_cast<CSkillBase*>(pInstance->Add_GameObject(LEVEL_STATIC, L"proto_obj_skillball", L"layer_skillball", strName.c_str(), &eType));
		pSkill->SetIndex(m_iSkillCount++);
		m_pSkills.push_back(pSkill);
	}
	else
	{
		if (m_pReserveSkills.size() < 8)
		{
			m_bSkillPushReady = false;
			wstring strName = to_wstring(CSkillBase::GetSkillID());
			CSkillBase* pSkill = static_cast<CSkillBase*>(pInstance->Add_GameObject(LEVEL_STATIC, L"proto_obj_skillball", L"layer_skillball", strName.c_str(), &eType));
			pSkill->SetRender(false);
			m_pReserveSkills.push(pSkill);
		}
	}

	return S_OK;
}

CSkillBase::SKILL_INFO CSkillBallSystem::UseSkill(_uint iIndex)
{
	_int iSkillSize = m_pSkills.size();

	//���� ��
	CSkillBase::SKILL_INFO SkillInfo;
	ZeroMemory(&SkillInfo, sizeof CSkillBase::SKILL_INFO);
	SkillInfo.eType = CSkillBase::TYPE::INVALID;

	if (iSkillSize - 1 < (_int)iIndex)
		return SkillInfo;

	CSkillBase::CHAIN_DATA ChainData = ComputeSkillChain(iIndex);

	//ü�� �Ѱ��϶� ��������
	if (ChainData.iChainCount == CHAIN_ONE)
	{
		_uint i = 0;
		for (auto& iter = m_pSkills.begin(); iter != m_pSkills.end(); )
		{
			if (i == iIndex)
			{
				m_iSkillCount--;
				SkillInfo.eType = m_pSkills[iIndex]->GetSkillType();
				m_pSkills[i]->SetDestroyWait();
				iter = m_pSkills.erase(iter);

				//���� ��ų�� �������� ���ེų���� �����ϸ� �տ��� ����
				if (!m_pReserveSkills.empty())
				{
					CSkillBase* pSkill = m_pReserveSkills.front();
					pSkill->SetRender(true);
					m_pSkills.push_back(pSkill);
					m_pReserveSkills.pop();
					m_iSkillCount++;
				}

				break;
			}
			else
			{
				iter++;
				i++;
			}
		}
	}
	else if (ChainData.iChainCount == CHAIN_TWO)
	{
		_uint i = 0;
		_uint j = ChainData.iLeftIndex;
		for (auto& iter = m_pSkills.begin(); iter != m_pSkills.end(); )
		{
			if (i >= ChainData.iLeftIndex && i <= ChainData.iRightIndex)
			{
				m_iSkillCount--;
				SkillInfo.eType = m_pSkills[j]->GetSkillType();
				m_pSkills[j]->SetDestroyWait();
				iter = m_pSkills.erase(iter);

				//���� ��ų�� �������� ���ེų���� �����ϸ� �տ��� ����
				if (!m_pReserveSkills.empty())
				{
					CSkillBase* pSkill = m_pReserveSkills.front();
					pSkill->SetRender(true);
					m_pSkills.push_back(pSkill);
					m_pReserveSkills.pop();
					m_iSkillCount++;
				}

				i++;
			}
			else
			{
				iter++;
				i++;
			}
		}
	}
	else if (ChainData.iChainCount == CHAIN_THREE)
	{
		_uint i = 0;
		_uint j = ChainData.iLeftIndex;
	
		for (auto& iter = m_pSkills.begin(); iter != m_pSkills.end(); )
		{
			if (i >= ChainData.iLeftIndex && i <= ChainData.iRightIndex)
			{
				m_iSkillCount--;
				SkillInfo.eType = m_pSkills[j]->GetSkillType();
				m_pSkills[j]->SetDestroyWait();
				iter = m_pSkills.erase(iter);

				//���� ��ų�� �������� ���ེų���� �����ϸ� �տ��� ����
				if (!m_pReserveSkills.empty())
				{
					CSkillBase* pSkill = m_pReserveSkills.front();
					pSkill->SetRender(true);
					m_pSkills.push_back(pSkill);
					m_pReserveSkills.pop();
					m_iSkillCount++;
				}

				i++;
			}
			else
			{
				iter++;
				i++;
			}
		}
	}

	for (_uint i = 0; i < m_iSkillCount; ++i)
		m_pSkills[i]->SetIndex(i);

	SkillInfo.iChainCount = ChainData.iChainCount;
	return SkillInfo;
}

CSkillBase::CHAIN_DATA CSkillBallSystem::ComputeSkillChain(_uint iIndex)
{
	CSkillBase::CHAIN_DATA ChainData;
	ZeroMemory(&ChainData, sizeof CSkillBase::CHAIN_DATA);

	ChainData.iLeftIndex = iIndex;
	ChainData.iRightIndex = iIndex;

	int iTargetIndex = (int)iIndex;
	while (iTargetIndex - 1 >= 0 && ChainData.iLeftIndex > (int)iIndex - 2)
	{
		//���õ� �ε����� ���� �ε����� Ÿ���� �ٸ��� �극��ũ
		if (!m_pSkills[iIndex]->IsSameType(m_pSkills[--iTargetIndex]->GetSkillType()))
			break;

		ChainData.iLeftIndex--;
	}

	iTargetIndex = iIndex;
	while (iTargetIndex + 1 < (int)m_iSkillCount)
	{
		//���õ� �ε����� ���� �ε����� Ÿ���� �ٸ��� �극��ũ
		if (!m_pSkills[iIndex]->IsSameType(m_pSkills[++iTargetIndex]->GetSkillType()))
			break;

		ChainData.iRightIndex++;
	}

	ChainData.iChainCount = ChainData.iRightIndex - ChainData.iLeftIndex;
	if (ChainData.iChainCount > 2)
	{
		ChainData.iChainCount = 2;
		ChainData.iRightIndex = ChainData.iLeftIndex + 2;
	}

	return ChainData;
}

void CSkillBallSystem::Free()
{
	Clear();
}
