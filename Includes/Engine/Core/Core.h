#pragma once

// dll 외부로 템플릿 내보낼 때 발생하는 경고 끄기
#pragma warning(disable: 4251)	
// 지역 변수의 주소를 반환할 때 발생하는 경고 비활성화
#pragma warning(disable: 4172)	


#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifdef _DEBUG
#define new new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
// Replace _NORMAL_BLOCK with _CLIENT_BLOCK if you want the
// allocations to be of _CLIENT_BLOCK type
#else
#define new new
#endif

#if BuildEngineDLL
#define Engine_API __declspec(dllexport)
#else
#define Engine_API __declspec(dllimport)
#endif

// 메모리 정리 함수
template<typename T>
void SafeDelete(T*& _target)
{
	if (nullptr != _target)
	{
		delete _target;
		_target = nullptr;
	}
}

template<typename T>
void SafeDeleteArray(T*& _target)
{
	if (nullptr != _target)
	{
		delete[] _target;
		_target = nullptr;
	}
}