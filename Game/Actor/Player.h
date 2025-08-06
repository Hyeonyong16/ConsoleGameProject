#pragma once
#include "Actor/Actor.h"
#include "Utils/Timer.h"

class Camera;
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
	friend class Camera;
	RTTI_DECLARATIONS(Player, Actor)
public:
	Player();
	Player(const Vector2& _position);

	~Player();

	virtual void BeginPlay() override;
	virtual void Tick(float _deltaTime) override;
	virtual void Render() override;

public:
	inline void SetCamera(Camera* _camera) { cam = _camera; }

	inline int GetBullet() const { return bullet; }
	inline void IncreaseBullet(int _num) { bullet += _num; }

	inline Vec2Float GetPosFloat() const { return pos; }
	inline Vec2Float GetDir() const { return dir; }
	inline float GetAngle() const { return angle; }

	// Vec2Float ��ġ�� Vector2 ��ġ�� ����ȭ
	void ResyncPos()
	{
		pos.x = (float)position.x;
		pos.y = (float)position.y;
	}

private:
	// ��� ���
	void Fire();

private:
	// �÷��̾� ��ġ
	Vec2Float pos = { 0.0f, 0.0f };

	// �÷��̾ ���� ���� ����
	float angle = 10.f;

	// �÷��̾ ���� ���⺤��
	Vec2Float dir = { 0.0f, 0.0f };

	// �÷��̾� �ӷ�
	float speed = 0.0f;

	// �÷��̾� ȸ�� �ӷ�
	float rotSpeed = 0.0f;

	// �÷��̾� ũ��
	float scale = 0.0f;

	// �Ѿ� ����
	int bullet = 5;

	// ��� ����
	bool isFire = false;
	
	// ��� Ÿ�̸�
	Timer shotTimer;

	
	// ������ �� �浹 üũ�� ���� ���� �� Ŭ���� ����
	int** wallMap = nullptr;
	Camera* cam = nullptr;
};