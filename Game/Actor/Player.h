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
	// �÷��̾� ��ġ
	Vec2Float pos = { 0.0f, 0.0f };

	// �÷��̾ ���� ���� ����
	float angle = 0.0f;

	// �÷��̾ ���� ���⺤��
	Vec2Float dir = { 0.0f, 0.0f };

	// �÷��̾� �ӷ�
	float speed = 0.0f;

	// �÷��̾� ȸ�� �ӷ�
	float rotSpeed = 0.0f;


};