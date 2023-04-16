#include "..\Public\Sound_Manager.h"

IMPLEMENT_SINGLETON(CSound_Manager)

/* FMOD_LOOP_OFF: 무한재생 끔 */
/*  FMOD_LOOP_NORMAL : 무한재생 켬 */

CSound_Manager::CSound_Manager()
{
}

HRESULT CSound_Manager::Initialize()
{
	m_eResult = FMOD::System_Create(&m_pSystem);
	if (m_eResult != FMOD_OK)
		return E_FAIL;

	m_eResult = m_pSystem->getVersion(&m_iVersion);
	if (m_eResult != FMOD_OK)
		return E_FAIL;

	else
		printf("FMOD version %08x\n", m_iVersion);

	m_eResult = m_pSystem->init(32, FMOD_INIT_NORMAL, m_pExtradriverdata);  // 시스템 초기화 : chaneel 32개까지 사용하겠다.
	if (m_eResult != FMOD_OK)
		return E_FAIL;

	if (FAILED(Load_BgmFile()))
		return E_FAIL;
		
	/*if (FAILED(Load_SoundFile()))
		return E_FAIL;*/

	return S_OK;
}

HRESULT CSound_Manager::Play_Sound(TCHAR * pSoundKey, SOUND_CHANNEL eChannel, SOUND_VOLUME eVolum, float fVolume)
{
	if (MAX_CHANNEL <= eChannel)
		return E_FAIL;

	UMAP_SOUND::iterator iter;

	iter = find_if(
		m_umapSound.begin(),
		m_umapSound.end(),
		[&](auto& iter)->bool { return !lstrcmp(pSoundKey, iter.first); }
	);

	if (iter == m_umapSound.end())
		return E_FAIL;

	m_pSystem->playSound(
		iter->second,
		0,
		false,
		&m_pChannel[eChannel]
	);

	Set_SoundVolume(eChannel, eVolum, fVolume);

	return S_OK;
}

HRESULT CSound_Manager::Set_SoundVolume(SOUND_CHANNEL eChannel, SOUND_VOLUME eVolum, float fVolume)
{
	if (MAX_CHANNEL <= eChannel)
		return E_FAIL;

	switch (eVolum)
	{
	case CUSTOM_VOLUM:
		m_pChannel[eChannel]->setVolume(fVolume);
		break;
	case MIN_VOLUME:
		m_pChannel[eChannel]->setVolume(0.1f);
		break;
	case DEFAULT_VOLUME:
		m_pChannel[eChannel]->setVolume(0.5f);
		break;
	case MAX_VOLUME:
		m_pChannel[eChannel]->setVolume(1.f);
		break;
	default:
		break;
	}

	m_pSystem->update();

	return S_OK;
}

HRESULT CSound_Manager::Stop_Sound(SOUND_CHANNEL eChannel)
{
	if (MAX_CHANNEL <= eChannel)
		return E_FAIL;

	m_pChannel[eChannel]->setPaused(true);

	return S_OK;
}

void CSound_Manager::Stop_AllSound()
{
	for (_uint i = 0; i < MAX_CHANNEL; ++i)
		m_pChannel[i]->setPaused(true);
}

HRESULT CSound_Manager::Load_SoundFile()
{
	_finddatai64_t tFD;
	intptr_t hFile;

	char szCurPath[128] = "../../Sounds/";
	char szFullPath[128] = "";
	hFile = _findfirsti64("../../Sounds/*.*", &tFD);

	if (-1 == hFile)
		return E_FAIL;

	_int iResult = 0;

	while (-1 != iResult)
	{
		strcpy_s(szFullPath, szCurPath);
		/* 사운드 파일이름 결합 (최종 경로) */
		strcat_s(szFullPath, tFD.name);

		FMOD::Sound*     pSound = nullptr;

		/* 사운드 생성 */
		m_eResult = m_pSystem->createSound(szFullPath, FMOD_LOOP_OFF, 0, &pSound);

		if (FMOD_OK == m_eResult)
		{
			/* null 문자 까지 + 1 해서 길이 책정 */
			_int iLength = strlen(tFD.name) + 1;

			TCHAR* pSoundKey = new TCHAR[iLength];
			ZeroMemory(pSoundKey, sizeof(TCHAR) * iLength);

			/* 아스키 코드 -> 유니코드 로 변환 */
			MultiByteToWideChar(CP_ACP, 0, tFD.name, iLength, pSoundKey, iLength);

			m_umapSound.emplace(pSoundKey, pSound);
		}
		/* 다음 위치의 파일을 찾는다. 더이상 없을시 -1 리턴 */
		iResult = _findnexti64(hFile, &tFD);
	} 

	_findclose(hFile);
	m_pSystem->update();

	return S_OK;
}

HRESULT CSound_Manager::Load_BgmFile()
{
	_finddatai64_t tFD;
	intptr_t hFile;

	char szCurPath[128] = "../../Sounds/Bgm/";
	char szFullPath[128] = "";
	hFile = _findfirsti64("../../Sounds/Bgm/*.*", &tFD);

	if (-1 == hFile)
		return E_FAIL;

	_int iResult = 0;

	while (-1 != iResult)
	{
		strcpy_s(szFullPath, szCurPath);
		/* 사운드 파일이름 결합 (최종 경로) */
		strcat_s(szFullPath, tFD.name);

		FMOD::Sound*     pSound = nullptr;

		/* 사운드 생성 */
		m_eResult = m_pSystem->createSound(szFullPath, FMOD_LOOP_NORMAL, 0, &pSound);

		if (FMOD_OK == m_eResult)
		{
			/* null 문자 까지 + 1 해서 길이 책정 */
			_int iLength = strlen(tFD.name) + 1;

			TCHAR* pSoundKey = new TCHAR[iLength];
			ZeroMemory(pSoundKey, sizeof(TCHAR) * iLength);

			/* 아스키 코드 -> 유니코드 로 변환 */
			MultiByteToWideChar(CP_ACP, 0, tFD.name, iLength, pSoundKey, iLength);

			m_umapSound.emplace(pSoundKey, pSound);
		}
		/* 다음 위치의 파일을 찾는다. 더이상 없을시 -1 리턴 */
		iResult = _findnexti64(hFile, &tFD);
	}

	_findclose(hFile);
	m_pSystem->update();

	return S_OK;
}

void CSound_Manager::Free()
{
	for (auto& Pair : m_umapSound)
	{
		delete[] Pair.first;
		Pair.second->release();
	}
	m_umapSound.clear();

	m_pSystem->release();
	m_pSystem->close();
}
