#pragma once
#include "Actor/Actor.h"

struct Vec2Float
{
public:
	float x = 0.0f;
	float y = 0.0f;

	operator Vector2()
	{
		Vector2 vec;
		vec.x = (int)x;
		vec.y = (int)y;

		return vec;
	}
	operator Vector2() const
	{
		Vector2 vec;
		vec.x = (int)x;
		vec.y = (int)y;

		return vec;
	}
};

class Player : public Actor
{
	RTTI_DECLARATIONS(Player, Actor)
public:
	Player();

	virtual void Tick(float _deltaTime) override;
	virtual void Render() override;

private:

private:
	// 플레이어 위치
	Vec2Float pos = { 0.0f, 0.0f };

	// 플레이어가 보는 방향 각도
	float angle = 0.0f;

	// 플레이어가 보는 방향벡터
	Vec2Float dir = { 0.0f, 0.0f };

	// 플레이어 속력
	float speed = 0.0f;

	// 플레이어 회전 속력
	float rotSpeed = 0.0f;


};