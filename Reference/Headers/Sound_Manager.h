#pragma once

#include "Base.h"

BEGIN(Engine)

class CSound_Manager final : public CBase
{
	DECLARE_SINGLETON(CSound_Manager)

private:
	CSound_Manager();
	virtual ~CSound_Manager() = default;

private:
	enum { MAX_CHANNEL = 30 };

public:
	HRESULT Initialize();

	/* �ŰԺ��� SOUND_VOLUME eVolum = CUSTOM_VOLUM ���°͵� �����غ� �� */
	HRESULT Play_Sound(
		TCHAR* pSoundKey, 
		int eChannel, 
		SOUND_VOLUME eVolum = CUSTOM_VOLUM,
		float fVolume = 0.1f
	);

	HRESULT Set_SoundVolume(
		int eChannel, 
		SOUND_VOLUME eVolum = CUSTOM_VOLUM,
		float fVolume = 0.1f
	);
	
	/*HRESULT Up_SoundVolum(SOUND_CHANNEL eChannel);
	HRESULT Down_SoundVolum(SOUND_CHANNEL eChannel);*/

	HRESULT Stop_Sound(int eChannel);
	void Stop_AllSound();

private:
	/* ���� ������ ���� �ٹڰ� �ε��� �� */
	HRESULT Load_SoundFile();

private:
	/* ���� �������� ���� ������ �ε� �� �� (�������� �ϳ��� ������ָ� ��) */
	HRESULT Load_BgmFile();

private:
	typedef unordered_map<TCHAR*, FMOD::Sound*>		UMAP_SOUND;
	UMAP_SOUND			m_umapSound;

	FMOD::System*		m_pSystem = nullptr;
	FMOD::Channel*		m_pChannel[MAX_CHANNEL];
	FMOD_RESULT			m_eResult;
	unsigned int		m_iVersion;
	void*				m_pExtradriverdata = nullptr;

public:
	virtual void Free() override;
};

END