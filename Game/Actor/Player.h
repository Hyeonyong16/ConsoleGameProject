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

	// Vec2Float 위치랑 Vector2 위치를 동기화
	void ResyncPos()
	{
		pos.x = (float)position.x;
		pos.y = (float)position.y;
	}

private:
	// 사격 기능
	void Fire();

private:
	// 플레이어 위치
	Vec2Float pos = { 0.0f, 0.0f };

	// 플레이어가 보는 방향 각도
	float angle = 10.f;

	// 플레이어가 보는 방향벡터
	Vec2Float dir = { 0.0f, 0.0f };

	// 플레이어 속력
	float speed = 0.0f;

	// 플레이어 회전 속력
	float rotSpeed = 0.0f;

	// 플레이어 크기
	float scale = 0.0f;

	// 총알 갯수
	int bullet = 5;

	// 사격 여부
	bool isFire = false;
	
	// 사격 타이머
	Timer shotTimer;

	
	// 렌더링 및 충돌 체크를 위한 레벨 내 클래스 저장
	int** wallMap = nullptr;
	Camera* cam = nullptr;
};