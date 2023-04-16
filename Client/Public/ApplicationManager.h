#pragma once

#include "Base.h"

BEGIN(Client)

class CApplicationManager final : public CBase
{
	DECLARE_SINGLETON(CApplicationManager)

public:
	enum class LEVEL { GAMEPLAY, BOSS, MAX };

private:
	CApplicationManager();
	virtual ~CApplicationManager() = default;

public:
	void SetTitle(wstring strTitle);

	//초산공간 오픈
	_bool IsFreezeReady() const { return m_bFreezeReady; }
	void SetFreezeReady(_bool value) { m_bFreezeReady = value; }
	void SetFreeze(_bool value) { m_bFreeze = value; }
	_bool IsFreeze() const { return m_bFreeze; }
	void SetPlayerPrevPosition(_float3 value) { m_vPrevPosition = value; }
	_vector GetPlayerPrevPosition() { return XMLoadFloat3(&m_vPrevPosition); }

	void SetHitFreeze(_bool value) { m_bHitFreeze = value; }
	_bool IsHitFreeze() const { return m_bHitFreeze; }

	//레벨이 끝났다는 설정 및 체크
	void SetLevelFinish(LEVEL eLevel) { m_bLevelEnd[(_uint)eLevel] = true; }
	_bool IsLevelFinish(LEVEL eLevel) { return m_bLevelEnd[(_uint)eLevel]; }

	void SetWinMotion(_bool value) { m_isWinMotionPlay = value; }
	_bool IsWinMotionPlay() { return m_isWinMotionPlay; }
	//이걸로 여기저기서 체크할래

	//몬스터 스폰체크
	void SetSpawned(_bool value) { m_bEnemySpawned = value; }
	_bool IsSpawned() { return m_bEnemySpawned; }

	//보스 가시 사운드 채널관리
public:
	virtual void Free() override;

private:
	std::wstring m_strTitleText;
	_bool m_bFreeze = false;
	_bool m_bFreezeReady = true;

	_float3 m_vPrevPosition = { 0.f, 0.f, 0.f };

	_bool m_bHitFreeze = false;

	//레벨 끝났다
	_bool m_bLevelEnd[(_uint)LEVEL::MAX] = { false, false };
	//레벨 승리 모션중
	_bool m_isWinMotionPlay = false;

	//스폰되기전 이펙트 false
	_bool m_bEnemySpawned = false;

	//_bool m_bUsedSound[8] = { false, };
};

END