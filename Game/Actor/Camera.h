#pragma once
#include "Actor/Actor.h"

// �÷��̾� ������ ��ũ������ ǥ���� ����

class Player;

class Camera : public Actor
{
	RTTI_DECLARATIONS(Camera, Actor)

public:
	Camera(Player* _player);
	~Camera();

	// Pos ��ġ�� ���� ����
	void SetCharinScreen(const char _c, Vector2 _pos);

	virtual void Tick(float _deltaTime) override;
	virtual void Render() override;

private:
	int screenWidth = 120;
	int screenHeight = 50;

	// ȭ���� ��Ÿ�� 2���� �迭
	char** screen = nullptr;
	Player* ownerPlayer = nullptr;

	float dist = 3.f;

	float fov = 90.0f;	// 90��

};