#pragma once

#include "Core.h"
#include <Windows.h>

class Engine_API Vector2
{
public:
	Vector2(int _x = 0, int _y = 0);
	~Vector2();	// 기본 소멸자 생성

	Vector2 operator+(const Vector2& _other) const;
	Vector2 operator-(const Vector2& _other) const;

	bool operator==(const Vector2& _other) const;

	// 형변환 연산자 오버로딩
	operator COORD();
	operator COORD() const;

	// 문자열로 출력
	const char* ToString();

	static Vector2 Zero;
	static Vector2 One;
	static Vector2 Up;
	static Vector2 Right;


public:
	int x = 0;
	int y = 0;

private:
	char* value = nullptr;
};