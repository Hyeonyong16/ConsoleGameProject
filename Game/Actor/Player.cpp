#include "Player.h"
#include "Actor/Actor.h"
#include "Level/Level.h"
#include "Level/GameLevel.h"
#include "Actor/Wall.h"
#include "Actor/Monster.h"
#include "Actor/Item.h"
#include "Actor/Portal.h"
#include "Actor/Camera.h"

#include "Game/Game.h"

#include "Engine.h"
#include "Core.h"

#include "Utils/Utils.h"

#include <iostream>
#include <math.h>

Player::Player()
	: Actor("P", Color::Red),
	speed(0.02f), rotSpeed(0.5f)
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
		Game::Get().GoMenu();
		//Engine::Get().Quit();
		return;
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
		if (!isFire && bullet > 0)
		{
			Fire();
		}
	}

	CollisionCheck();
}

void Player::Render()
{
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

	if (dynamic_cast<GameLevel*>(owner)->isFPS == false)
	{
		super::Render();
	}
}

void Player::IncreaseBullet()
{
	{ bullet += 3; }
}

//�浹 üũ
void Player::CollisionCheck()
{
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
				IncreaseBullet();

				tempItem->SetPlayerCollision(true);
			}
		}
	}

	Portal* tempPortal = owner->FindActorByID(gameLevel->GetPortalID())->As<Portal>();
	Vector2 portalPos = tempPortal->GetPosition();
	if (position.x == portalPos.x && position.y == portalPos.y)
	{
		if (tempPortal->GetPlayerCollision() == false)
		{
			tempPortal->SetPlayerCollision(true);
		}
	}
}

void Player::Fire()
{
	isFire = true;
	--bullet;
	// ���� �÷��̾� ���� ������ Ray �� Angle
	// �ִ븦 ����, �ּҸ� ������ ������ ��� �ܼ� �� ĭ ����ŭ �þ߰��� ������ ����

	float curDist = 0.f;

	// �浹 Ȯ�ο� ���� ray 
	int rayCheckNum = 1;
	float rayCheckDist = 0.1f;

	// DDA��ǥȮ�ο�
	float xPos = position.x;
	float yPos = position.y;

	Vec2Float curPos;
	curPos.x = position.x;
	curPos.y = position.y;

	GameLevel* gameLevel = owner->As<GameLevel>();

	while (curDist <= cam->GetDist())
	{
		curDist = rayCheckDist * rayCheckNum;
		++rayCheckNum;
		xPos = curPos.x + dir.x * curDist;
		yPos = curPos.y + dir.y * curDist;

		// ���� ���� ���� ���� ������(ID ���̴ϱ� �� �ƴϿ��� �����ϱ� ��)
		if (xPos < 0 || xPos >= (dynamic_cast<GameLevel*>(owner)->GetMapWidth())) return;
		if (yPos < 0 || yPos >= (dynamic_cast<GameLevel*>(owner)->GetMapHeight())) return;

		// ���� �������
		//if (wallMap[(int)yPos][(int)xPos] != -1
		//	&& owner->FindActorByID(wallMap[(int)yPos][(int)xPos])->As<Wall>())
		//{
		//	//owner->FindActorByID(wallMap[(int)yPos][(int)xPos])->As<Wall>()->SetColor(Color::Magenta);
		//}
		else
		{
			for (int id : gameLevel->GetMonsterIDs())
			{
				// �������� ��ġ (�簢�� ���� ���� �������� 0.5 ������
				Vec2Float monsterPos;
				monsterPos.x = (float)owner->FindActorByID(id)->GetPosition().x + 0.5f;
				monsterPos.y = (float)owner->FindActorByID(id)->GetPosition().y + 0.5f;

				float monsterDist = sqrt(
					(monsterPos.x - xPos) * (monsterPos.x - xPos) +
					(monsterPos.y - yPos) * (monsterPos.y - yPos)
				);

				if (monsterDist <= 0.5f)
				{
					owner->FindActorByID(id)->As<Monster>()->SetIsKilled(true);
					return;
				}
			}
		}

	}
}
