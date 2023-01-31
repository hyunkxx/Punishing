#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CInput_Device : public CBase
{
	DECLARE_SINGLETON(CInput_Device)

	struct tagKey
	{
		KEY_STATE eKeyState;
		bool bPrevPush;
		float Tap_Count;
		float Double_Count;
	};

private:
	explicit CInput_Device();
	virtual ~CInput_Device() = default;

public:
	_byte		Get_DIKeyState(_ubyte byKeyID) { return m_byKeyState[byKeyID]; }
	KEY_STATE	Get_DIKeyState_Custom(_int _key)
	{
		if (256 < _key || 0 > _key)
			return KEY_STATE::AWAY;

		return m_strKeyList[_key].eKeyState;
	}
	KEY_STATE	Get_DIMouseState_Custom(MOUSE_KEYSTATE _key)
	{
		if (DIMK_END <= _key || 0 > _key)
			return KEY_STATE::AWAY;

		return m_strMouseList[_key].eKeyState;
	}

	_byte		Get_DIMouseState(MOUSE_KEYSTATE byMouseID)
	{
		return m_MouseState.rgbButtons[byMouseID];
	}

	_long		Get_DIMouseMove(MOUSE_MOVESTATE eMoveState)
	{
		return *(((long*)&m_MouseState) + eMoveState);
	}

public:
	HRESULT			Initialize_Input(HINSTANCE hInst, HWND hWnd);
	void			Update(void);

private:
	void			CheckKeyState(void);

private:
	_byte			m_byKeyState[256];		// 모든 키보드의 입력 값을 저장하기 위한 배열
	tagKey			m_strKeyList[256];
	tagKey			m_strMouseList[DIMK_END];
	DIMOUSESTATE	m_MouseState;			// 마우스 메세지가 발생했을 때 마우스 정보를 보관하는 구조체

	LPDIRECTINPUT8				m_pInputSDK;
	LPDIRECTINPUTDEVICE8		m_pKeyBoard;
	LPDIRECTINPUTDEVICE8		m_pMouse;

public:
	virtual void		Free(void);
};

END