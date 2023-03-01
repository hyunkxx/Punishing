#pragma once

#define VK_MAX 0xff

#ifdef	ENGINE_EXPORTS
#define ENGINE_DLL		_declspec(dllexport)
#else
#define ENGINE_DLL		_declspec(dllimport)
#endif

#define			BEGIN(NAMESPACE)		namespace NAMESPACE {
#define			END						}

#define			USING(NAMESPACE)	using namespace NAMESPACE;

#ifndef			MESSAGE
#define			MESSAGE(_wstring)			MessageBox(NULL, _wstring.c_str(), L"Message", MB_OK)
#endif

#ifndef			MSG_BOX
#define			MSG_BOX(_message)			MessageBox(NULL, TEXT(_message), L"Message", MB_OK)
#endif

#define NO_COPY(CLASSNAME)									\
		private:											\
		CLASSNAME(const CLASSNAME&) = delete;				\
		CLASSNAME& operator = (const CLASSNAME&) = delete;		

#define DECLARE_SINGLETON(CLASSNAME)					\
		NO_COPY(CLASSNAME)								\
		private:										\
		static CLASSNAME*	m_pInstance;				\
		public:											\
		static CLASSNAME*	GetInstance( void );		\
		static unsigned long DestroyInstance( void );			

#define IMPLEMENT_SINGLETON(CLASSNAME)							\
		CLASSNAME*	CLASSNAME::m_pInstance = NULL;				\
		CLASSNAME*	CLASSNAME::GetInstance( void )	{			\
			if(NULL == m_pInstance) {							\
				m_pInstance = new CLASSNAME;					\
			}													\
			return m_pInstance;									\
		}														\
		unsigned long CLASSNAME::DestroyInstance( void ) {		\
			unsigned long dwRefCnt = 0;							\
			if(nullptr != m_pInstance)	{						\
				dwRefCnt = m_pInstance->Release();				\
				if(0 == dwRefCnt)								\
					m_pInstance = nullptr;						\
			}													\
			return dwRefCnt;									\
		}

#define IMGUI_DEFAULT  ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar

#define VECTOR_RIGHT	XMVectorSet(1.f, 0.f, 0.f, 0.f)
#define VECTOR_UP		XMVectorSet(0.f, 1.f, 0.f, 0.f)
#define VECTOR_LOOK		XMVectorSet(0.f, 0.f, 1.f, 0.f)
#define VECTOR_ZERO		XMVectorSet(0.f, 0.f, 0.f, 0.f)
#define POSITION_ZERO	XMVectorSet(0.f, 0.f, 0.f, 1.f)