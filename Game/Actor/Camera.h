#pragma once
#include "Actor/Actor.h"

// 플레이어 시점을 스크린으로 표시할 예정

class Player;

class Camera : public Actor
{
	RTTI_DECLARATIONS(Camera, Actor)

public:
	Camera(Player* _player);
	~Camera();

	// Pos 위치에 문자 설정
	void SetCharinScreen(const char _c, Vector2 _pos);

	virtual void Tick(float _deltaTime) override;
	virtual void Render() override;

private:
	int screenWidth = 120;
	int screenHeight = 50;

	// 화면을 나타낼 2차원 배열
	char** screen = nullptr;
	Player* ownerPlayer = nullptr;

	float dist = 3.f;

	float fov = 90.0f;	// 90도

};