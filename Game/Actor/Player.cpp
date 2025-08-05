#include "Player.h"
#include "Actor/Actor.h"
#include "Level/Level.h"
#include "Level/GameLevel.h"
#include "Actor/Wall.h"
#include "Actor/Item.h"
#include "Actor/Camera.h"
#include "Engine.h"
#include "Core.h"

#include "Utils/Utils.h"

#include <iostream>
#include <math.h>

Player::Player()
	: Actor("P", Color::Red),
	speed(0.05f), rotSpeed(0.5f)
{
	int xPosition = Engine::Get().GetWidth() / 2 - width / 2;
	int yPosition = Engine::Get().GetHeight() - 1;
	SetPosition(Vector2(xPosition, yPosition));
	pos.x = (float)xPosition;
	pos.y = (float)yPosition;

	shotTimer.SetTargetTime(0.5f);

	SetSortingOrder(3);
}

Player::Player(const Vector2& _position)
	: Actor("P", Color::Red, _position),
	speed(0.05f), rotSpeed(0.5f), scale(0.3f)
{
	pos.x = (float)_position.x;
	pos.y = (float)_position.y;
}

Player::~Player()
{

}

void Player::BeginPlay()
{
	super::BeginPlay();

	wallMap = dynamic_cast<GameLevel*>(GetOwner())->getWallMap();
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
	dir.y = -sin(angleRadian);

	// ESC = ����
	if (Input::Get().GetKey(VK_ESCAPE))
	{
		Engine::Get().Quit();
	}

	float moveX = 0.f;
	float moveY = 0.f;

	// ����Ű �Է�
	// W �� ������ ���⺤�������� �̵� (cos, sin)
	// S �� ������ �ݴ��	(-cos, -sin)
	// A ������ ���� ���� ( -sin, cos)
	// D ������ ���� ������ (sin, -cos)

	// �浹 üũ�� ���� �̵� �Ÿ��� ���
	if (Input::Get().GetKey('A'))
	{
		moveX = dir.y * speed;
		moveY = -dir.x * speed;
		isPosChange = true;
	}
	if (Input::Get().GetKey('D'))
	{
		moveX = -dir.y * speed;
		moveY = dir.x * speed;
		isPosChange = true;
	}
	if (Input::Get().GetKey('W'))
	{
		moveX = dir.x * speed;
		moveY = dir.y * speed;
		isPosChange = true;
	}
	if (Input::Get().GetKey('S'))
	{
		moveX = -dir.x * speed;
		moveY = -dir.y * speed;
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

		// ������ �̵�
		if (moveX > 0)
		{
			// �÷��̾� ũ�⸸ŭ ���̶� �浹 ����
			if (wallMap[(int)(pos.y + scale)][(int)(pos.x + scale)] == -1
				&& wallMap[(int)(pos.y - scale)][(int)(pos.x + scale)] == -1)
			{
				// �浹���� ������ �̵�
				pos.x += moveX;
			}
		}
		// ���� �̵�
		else if (moveX < 0)
		{
			// �÷��̾� ũ�⸸ŭ ���̶� �浹 ����
			if (wallMap[(int)(pos.y + scale)][(int)(pos.x - scale)] == -1
				&& wallMap[(int)(pos.y - scale)][(int)(pos.x - scale)] == -1)
			{
				// �浹���� ������ �̵�
				pos.x += moveX;
			}
		}

		// �Ʒ��� �̵�
		if (moveY > 0)
		{
			// �÷��̾� ũ�⸸ŭ ���̶� �浹 ����
			if (wallMap[(int)(pos.y + scale)][(int)(pos.x + scale)] == -1
				&& wallMap[(int)(pos.y + scale)][(int)(pos.x - scale)] == -1)
			{
				// �浹���� ������ �̵�
				pos.y += moveY;
			}
		}
		// ���� �̵�
		else if (moveY < 0)
		{
			// �÷��̾� ũ�⸸ŭ ���̶� �浹 ����
			if (wallMap[(int)(pos.y - scale)][(int)(pos.x + scale)] == -1
				&& wallMap[(int)(pos.y - scale)][(int)(pos.x - scale)] == -1)
			{
				// �浹���� ������ �̵�
				pos.y += moveY;
			}
		}

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

	if (isFire)
	{
		shotTimer.Tick(_deltaTime);
		if (shotTimer.IsTimeout()) 
		{
			shotTimer.Reset();
			shotTimer.SetTargetTime(0.5f);
			isFire = false;
		}
	}
	// ���
	if (Input::Get().GetKeyDown(VK_SPACE))
	{
		// ��� ���� �ð��� �Ǹ� ���
		if (!isFire)
		{
			Fire();
		}
	}

	// Todo: ������ �浹 ó��
	GameLevel* gameLevel = owner->As<GameLevel>();
	std::vector<int>& itemIdsVector = gameLevel->GetItemIDs();

	for (int tempItemId : itemIdsVector)
	{
		Item* tempItem = owner->FindActorByID(tempItemId)->As<Item>();
		Vector2 itemPos = tempItem->GetPosition();
		if (position.x == itemPos.x && position.y == itemPos.y)
		{
			if (tempItem->GetPlayerCollision() == false)
			{
				//Todo: ������ �Ծ��� �� �ൿ �߰�

				tempItem->SetPlayerCollision(true);
			}
			int a = 0;
		}
	}
}

void Player::Render()
{
	if (dynamic_cast<GameLevel*>(owner)->isFPS == false)
	{
		super::Render();

		// ����׿� �÷��̾� ��ǥ
		{
			char buffer[30] = { };
			sprintf_s(buffer, 30, "x: %d, y: %d", position.x, position.y);
			Engine::Get().WriteToBuffer(Vector2(Engine::Get().GetWidth() - 35, 3), buffer);
			//std::cout << "x: " << position.x << " y: " << position.y << "            ";
			sprintf_s(buffer, 30, "pos x: %.2f, y: %.2f", pos.x, pos.y);
			Engine::Get().WriteToBuffer(Vector2(Engine::Get().GetWidth() - 35, 4), buffer);
			//std::cout << "pos x: " << pos.x << " y: " << pos.y << "            ";
			sprintf_s(buffer, 30, "angle: %.2f", angle);
			Engine::Get().WriteToBuffer(Vector2(Engine::Get().GetWidth() - 35, 5), buffer);
			//std::cout << "angle: " << angle << "            ";
			sprintf_s(buffer, 30, "dir x: %.2f, y: %.2f", dir.x, dir.y);
			Engine::Get().WriteToBuffer(Vector2(Engine::Get().GetWidth() - 35, 6), buffer);
			//std::cout << "dir: " << dir.x << ", " << dir.y << "            ";
		}
	}
}

void Player::Fire()
{
	isFire = true;

	// ���� �÷��̾� ���� ������ Ray �� Angle
		// �ִ븦 ����, �ּҸ� ������ ������ ��� �ܼ� �� ĭ ����ŭ �þ߰��� ������ ����

	float rayAngleRadian = angle * PI / 180;
	Vec2Float rayAngleDir;
	rayAngleDir.x = cos(rayAngleRadian);
	rayAngleDir.y = sin(rayAngleRadian);

	float curDist = 0.f;
	float xDelta = fabs(1 / rayAngleDir.x); // x ���� 1 �ٲ� �� ���� �Ÿ�
	float yDelta = fabs(1 / rayAngleDir.y); // y ���� 1 �ٲ� �� ���� �Ÿ�
	float xDist = xDelta * (pos.x - trunc(pos.x));	// ó�� x���� �ٲ� ���� �Ÿ�
	float yDist = yDelta * (pos.y - trunc(pos.y));	// ó�� y���� �ٲ� ���� �Ÿ�

	// DDA��ǥȮ�ο�
	int xPosDDA = position.x;
	int yPosDDA = position.y;

	while (curDist <= cam->GetDist())
	{
		// yDist �� xDist ���� ª�� -> y = n ���� ������
		if (xDist > yDist)
		{
			// �������� �Ÿ� ydist ��
			curDist = yDist;

			// yDist �� ���� ����
			yDist += yDelta;

			// ���⿡ ���� y��ǥ ����
			if (rayAngleDir.y > 0) --yPosDDA;
			else ++yPosDDA;
		}
		// xDist �� yDist ���� ª�� -> x = n ���� ������
		else
		{
			// �������� �Ÿ� xdist ��
			curDist = xDist;

			// xDist �� ���� ����
			xDist += xDelta;

			// ���⿡ ���� x��ǥ ����
			if (rayAngleDir.x > 0) ++xPosDDA;
			else --xPosDDA;
		}

		// ���� ���� ���� ���� ������(ID ���̴ϱ� �� �ƴϿ��� �����ϱ� ��)
		if (xPosDDA < 0 || xPosDDA >= (dynamic_cast<GameLevel*>(owner)->GetMapWidth())) return;
		if (yPosDDA < 0 || yPosDDA >= (dynamic_cast<GameLevel*>(owner)->GetMapHeight())) return;

		// ���� �������
		if (wallMap[yPosDDA][xPosDDA] != -1
			&& owner->FindActorByID(wallMap[yPosDDA][xPosDDA])->As<Wall>())
		{
			owner->FindActorByID(wallMap[yPosDDA][xPosDDA])->As<Wall>()->SetColor(Color::Magenta);
		}
	}
}
