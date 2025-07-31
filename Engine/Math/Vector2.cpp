#include "Vector2.h"

#include <iostream>

Vector2 Vector2::Zero = Vector2(0, 0);
Vector2 Vector2::One = Vector2(1, 1);
Vector2 Vector2::Up = Vector2(0, 1);
Vector2 Vector2::Right = Vector2(1, 0);

Vector2::Vector2(int _x, int _y)
	: x(_x), y(_y)
{
}

Vector2::~Vector2()
{
	if (nullptr != value)
	{
		delete[] value;
		value = nullptr;
	}
}

Vector2 Vector2::operator+(const Vector2& _other) const
{
	return Vector2(x + _other.x, y + _other.y);
}

Vector2 Vector2::operator-(const Vector2& _other) const
{
	return Vector2(x - _other.x, y - _other.y);
}

bool Vector2::operator==(const Vector2& _other) const
{
	return x == _other.x && y == _other.y;
}

Vector2::operator COORD()
{
	COORD coord;
	coord.X = (short)x;
	coord.Y = (short)y;

	return coord;
}

Vector2::operator COORD() const
{
	COORD coord;
	coord.X = (short)x;
	coord.Y = (short)y;

	return coord;
}

// Vector 를 문자열로 출력
const char* Vector2::ToString()
{
	// "(x, y)"
	if (nullptr != value)
	{
		delete[] value;
	}

	value = new char[1024];
	sprintf_s(value, 1024, "(%d, %d)", x, y);
	return value;
}
