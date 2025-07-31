#pragma once

#include <Windows.h>
#include "Math/Vector2.h"
#include "Math/Color.h"
//#include <iostream>

// 프로젝트에서 다양하게 사용할 유틸리티 함수 모음
namespace Utils
{
	// 두 값을 서로 바꿀 때 사용하는 함수
	template<typename T>
	void Swap(T& _a, T& _b)
	{
		T temp = _a;
		_a = _b;
		_b = temp;
	}

	// 콘솔 출력 핸들 가져오는 함수
	inline HANDLE GetConsoleHandle()
	{
		return GetStdHandle(STD_OUTPUT_HANDLE);
	}

	// 콘솔 커서 위치 이동 함수
	inline void SetConsolePosition(COORD _coord)
	{
		static HANDLE handle = GetConsoleHandle();
		SetConsoleCursorPosition(handle, _coord);
	}

	inline void SetConsolePosition(const Vector2& _position)
	{
		SetConsolePosition(static_cast<COORD>(_position));
	}

	// 콘솔 텍스트 색상 설정 함수
	inline void SetConsoleTextColor(WORD _color)
	{
		static HANDLE handle = GetConsoleHandle();
		SetConsoleTextAttribute(handle, _color);
	}

	inline void SetConsoleTextColor(Color _color)
	{
		SetConsoleTextColor(static_cast<WORD>(_color));
	}

	// 랜덤 생성 함수
	inline int Random(int _min, int _max)
	{
		// max 와 min 의 차이 구하기
		int diff = (_max - _min) + 1;
		return ((diff * rand()) / (RAND_MAX + 1)) + _min;
	}

	inline float RandomFloat(float _min, float _max)
	{
		// 0 ~ 1 사이의 난수 구하기
		float random = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		return random * (_max - _min) + _min;
	}
}

