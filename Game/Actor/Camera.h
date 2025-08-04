#pragma once
#include "Actor/Actor.h"

// �÷��̾� ������ ��ũ������ ǥ���� ����

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

	// Pos ��ġ�� ���� ����
	void SetCharinScreen(const char _c, Vector2 _pos);

	virtual void Tick(float _deltaTime) override;
	virtual void Render() override;

private:
	int screenWidth = 140;
	int screenHeight = 50;

	// ȭ���� ��Ÿ�� 2���� �迭
	pixelInfo** screen = nullptr;

	Player* ownerPlayer = nullptr;

	// 1�������� �� ��ġ�� ���̸� ����� ����
	float* depthBuffer = nullptr;

	// �ִ� ������ �Ÿ�
	float dist = 15.f;

	// �þ߰�
	float fov = 90.0f;	// 90��

};