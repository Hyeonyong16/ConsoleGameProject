#pragma once

#include <Windows.h>
#include "Math/Vector2.h"
#include "Math/Color.h"
//#include <iostream>

// ������Ʈ���� �پ��ϰ� ����� ��ƿ��Ƽ �Լ� ����
namespace Utils
{
	// �� ���� ���� �ٲ� �� ����ϴ� �Լ�
	template<typename T>
	void Swap(T& _a, T& _b)
	{
		T temp = _a;
		_a = _b;
		_b = temp;
	}

	// �ܼ� ��� �ڵ� �������� �Լ�
	inline HANDLE GetConsoleHandle()
	{
		return GetStdHandle(STD_OUTPUT_HANDLE);
	}

	// �ܼ� Ŀ�� ��ġ �̵� �Լ�
	inline void SetConsolePosition(COORD _coord)
	{
		static HANDLE handle = GetConsoleHandle();
		SetConsoleCursorPosition(handle, _coord);
	}

	inline void SetConsolePosition(const Vector2& _position)
	{
		SetConsolePosition(static_cast<COORD>(_position));
	}

	// �ܼ� �ؽ�Ʈ ���� ���� �Լ�
	inline void SetConsoleTextColor(WORD _color)
	{
		static HANDLE handle = GetConsoleHandle();
		SetConsoleTextAttribute(handle, _color);
	}

	inline void SetConsoleTextColor(Color _color)
	{
		SetConsoleTextColor(static_cast<WORD>(_color));
	}

	// ���� ���� �Լ�
	inline int Random(int _min, int _max)
	{
		// max �� min �� ���� ���ϱ�
		int diff = (_max - _min) + 1;
		return ((diff * rand()) / (RAND_MAX + 1)) + _min;
	}

	inline float RandomFloat(float _min, float _max)
	{
		// 0 ~ 1 ������ ���� ���ϱ�
		float random = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		return random * (_max - _min) + _min;
	}
}

