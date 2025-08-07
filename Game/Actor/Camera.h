#pragma once
#include "Actor/Actor.h"
#include <vector>

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

	inline int GetScreenWidth() const { return screenWidth; }
	inline int GetScreenHeight() const { return screenHeight; }

	inline int GetDist() const { return dist; }

	void DrawActorByDDA(std::vector<int> _actorIDs, float _renderScale);
	void DrawActorByDDA(int _actorId, float _renderScale);

	virtual void BeginPlay() override;
	virtual void Tick(float _deltaTime) override;
	virtual void Render() override;

private:
	int screenWidth = 180;
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