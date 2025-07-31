#include "Player.h"
#include "Actor/Actor.h"
#include "Engine.h"
#include "Core.h"

#include "Utils/Utils.h"

#include <iostream>

// Todo
// �÷��̾� ȸ��
//	- 

Player::Player()
	: Actor("P", Color::Red),
	speed(0.05f), rotSpeed(0.5f)
{
	int xPosition = Engine::Get().GetWidth() / 2 - width / 2;
	int yPosition = Engine::Get().GetHeight() - 1;
	SetPosition(Vector2(xPosition, yPosition));
	pos.x = (float)xPosition;
	pos.y = (float)yPosition;
}

void Player::Tick(float _deltaTime)
{
	super::Tick(_deltaTime);

	// ��ġ �̵��� �̹� �����ӿ� �־����� ���� : ������ ��ġ ���� �ٽ� ����
	bool isPosChange = false;

	// �ﰢ�Լ��� radian ���� : degree > radian
	float angleRadian = angle * PI / 180;

	// Player �� ���� ���� ����
	dir.x = cos(angleRadian);
	dir.y = sin(angleRadian);

	// ESC = ����
	if (Input::Get().GetKey(VK_ESCAPE))
	{
		Engine::Get().Quit();
	}

	// ����Ű �Է�
	// W �� ������ ���⺤�������� �̵� (cos, sin)
	// S �� ������ �ݴ��	(-cos, -sin)
	// A ������ ���� ���� ( -sin, cos)
	// D ������ ���� ������ (sin, -cos)
	// y�� + ������ �Ʒ��� �̹Ƿ� y ���⿡ - �ٿ���
	if (Input::Get().GetKey('A'))
	{
		pos.x -= dir.y * speed;
		pos.y += -dir.x * speed;
		isPosChange = true;
	}
	if (Input::Get().GetKey('D'))
	{
		pos.x += dir.y * speed;
		pos.y -= -dir.x * speed;
		isPosChange = true;
	}
	if (Input::Get().GetKey('W'))
	{
		pos.x += dir.x * speed;
		pos.y += -dir.y * speed;
		isPosChange = true;
	}
	if (Input::Get().GetKey('S'))
	{
		pos.x -= dir.x * speed;
		pos.y -= -dir.y * speed;
		isPosChange = true;
	}

	// ������ �� ��ġ�ϱ� ��ǥ ������ ����� �ѹ���
	if (isPosChange)
	{
		// ȭ�� ��Ż ����
		if (pos.y < 0.0f) pos.y = 0.0f;
		if (pos.y > (float)(Engine::Get().GetHeight())) pos.y = (float)Engine::Get().GetHeight();
		if (pos.x < 0.0f) pos.x = 0.0f;
		if (pos.x > (float)(Engine::Get().GetWidth())) pos.x = (float)Engine::Get().GetWidth();

		// ��ġ ����
		SetPosition(pos);
	}

	// �÷��̾� ȸ��
	if (Input::Get().GetKey('Q'))
	{ 
		angle += rotSpeed;
		if (angle > 360.0f) angle -= 360.0f;	// 360�� ������ ���� ����
	}
	if (Input::Get().GetKey('E'))
	{
		angle -= rotSpeed;
		if (angle < 0.0f) angle += 360.0f;		// 360�� ������ ���� ����
	}
}

void Player::Render()
{
	super::Render();

	// ����׿� �÷��̾� ��ǥ
	{
		Utils::SetConsoleTextColor(Color::Red);
		Utils::SetConsolePosition(Vector2(70, 3));
		std::cout << "x: " << position.x << " y: " << position.y << "            ";
		Utils::SetConsolePosition(Vector2(70, 4));
		std::cout << "pos x: " << pos.x << " y: " << pos.y << "            ";
		Utils::SetConsolePosition(Vector2(70, 5));
		std::cout << "angle: " << angle << "            ";
		Utils::SetConsolePosition(Vector2(70, 6));
		std::cout << "dir: " << dir.x << ", " << dir.y << "            ";
	}
}
