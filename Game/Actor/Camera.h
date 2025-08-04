#pragma once
#include "Actor/Actor.h"

// 플레이어 시점을 스크린으로 표시할 예정

struct pixelInfo
{
	char text = '\0';
	Color color = Color::White;
};

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
	int screenWidth = 140;
	int screenHeight = 50;

	// 화면을 나타낼 2차원 배열
	pixelInfo** screen = nullptr;

	Player* ownerPlayer = nullptr;

	// 1차원으로 각 위치의 깊이를 기록할 버퍼
	float* depthBuffer = nullptr;

	// 최대 렌더링 거리
	float dist = 15.f;

	// 시야각
	float fov = 90.0f;	// 90도

};