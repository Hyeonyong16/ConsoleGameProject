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

	// 위치 이동이 이번 프레임에 있었는지 여부 : 있으면 위치 설정 다시 해줌
	bool isPosChange = false;

	// 삼각함수가 radian 기준 : degree > radian
	float angleRadian = angle * PI / 180;

	// Player 의 방향 벡터 구함
	dir.x = cos(angleRadian);
	dir.y = -sin(angleRadian);

	// ESC = 종료
	if (Input::Get().GetKey(VK_ESCAPE))
	{
		Game::Get().GoMenu();
		//Engine::Get().Quit();
		return;
	}

	float moveX = 0.f;
	float moveY = 0.f;

	// 방향키 입력
	// W 를 누르면 방향벡터쪽으로 이동 (cos, sin)
	// S 를 누르면 반대로	(-cos, -sin)
	// A 수직인 벡터 왼쪽 ( -sin, cos)
	// D 수직인 벡터 오른쪽 (sin, -cos)

	// 충돌 체크를 위해 이동 거리를 기록
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

	// 조건이 다 겹치니까 좌표 변경이 생기면 한번에
	if (isPosChange)
	{
		// 화면 이탈 방지
		if (pos.y < 0.0f) pos.y = 0.0f;
		if (pos.y > (float)(Engine::Get().GetHeight())) pos.y = (float)Engine::Get().GetHeight();
		if (pos.x < 0.0f) pos.x = 0.0f;
		if (pos.x > (float)(Engine::Get().GetWidth())) pos.x = (float)Engine::Get().GetWidth();

		// 오른쪽 이동
		if (moveX > 0)
		{
			// 플레이어 크기만큼 벽이랑 충돌 판정
			if (wallMap[(int)(pos.y + scale)][(int)(pos.x + scale)] == -1
				&& wallMap[(int)(pos.y - scale)][(int)(pos.x + scale)] == -1)
			{
				// 충돌하지 않으면 이동
				pos.x += moveX;
			}
		}
		// 왼쪽 이동
		else if (moveX < 0)
		{
			// 플레이어 크기만큼 벽이랑 충돌 판정
			if (wallMap[(int)(pos.y + scale)][(int)(pos.x - scale)] == -1
				&& wallMap[(int)(pos.y - scale)][(int)(pos.x - scale)] == -1)
			{
				// 충돌하지 않으면 이동
				pos.x += moveX;
			}
		}

		// 아래쪽 이동
		if (moveY > 0)
		{
			// 플레이어 크기만큼 벽이랑 충돌 판정
			if (wallMap[(int)(pos.y + scale)][(int)(pos.x + scale)] == -1
				&& wallMap[(int)(pos.y + scale)][(int)(pos.x - scale)] == -1)
			{
				// 충돌하지 않으면 이동
				pos.y += moveY;
			}
		}
		// 위쪽 이동
		else if (moveY < 0)
		{
			// 플레이어 크기만큼 벽이랑 충돌 판정
			if (wallMap[(int)(pos.y - scale)][(int)(pos.x + scale)] == -1
				&& wallMap[(int)(pos.y - scale)][(int)(pos.x - scale)] == -1)
			{
				// 충돌하지 않으면 이동
				pos.y += moveY;
			}
		}

		// 위치 설정
		SetPosition(pos);
	}

	// 플레이어 회전
	if (Input::Get().GetKey('Q'))
	{
		angle += rotSpeed;
		if (angle > 360.0f) angle -= 360.0f;	// 360도 내에서 범위 설정
	}
	if (Input::Get().GetKey('E'))
	{
		angle -= rotSpeed;
		if (angle < 0.0f) angle += 360.0f;		// 360도 내에서 범위 설정
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
	// 사격
	if (Input::Get().GetKeyDown(VK_SPACE))
	{
		// 사격 가능 시간이 되면 사격
		if (!isFire && bullet > 0)
		{
			Fire();
		}
	}

	CollisionCheck();
}

void Player::Render()
{
	// 디버그용 플레이어 좌표
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

//충돌 체크
void Player::CollisionCheck()
{
	// Todo: 아이템 충돌 처리
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
				//Todo: 아이템 먹었을 때 행동 추가
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
	// 현재 플레이어 기준 나가는 Ray 의 Angle
	// 최대를 왼쪽, 최소를 오른쪽 끝으로 잡고 콘솔 각 칸 수만큼 시야각을 나눠서 진행

	float curDist = 0.f;

	// 충돌 확인용 정면 ray 
	int rayCheckNum = 1;
	float rayCheckDist = 0.1f;

	// DDA좌표확인용
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

		// 정면 범위 내에 벽이 찍히면(ID 값이니까 벽 아니여도 가능하긴 함)
		if (xPos < 0 || xPos >= (dynamic_cast<GameLevel*>(owner)->GetMapWidth())) return;
		if (yPos < 0 || yPos >= (dynamic_cast<GameLevel*>(owner)->GetMapHeight())) return;

		// 벽을 찍었으면
		//if (wallMap[(int)yPos][(int)xPos] != -1
		//	&& owner->FindActorByID(wallMap[(int)yPos][(int)xPos])->As<Wall>())
		//{
		//	//owner->FindActorByID(wallMap[(int)yPos][(int)xPos])->As<Wall>()->SetColor(Color::Magenta);
		//}
		else
		{
			for (int id : gameLevel->GetMonsterIDs())
			{
				// 아이템의 위치 (사각형 기준 원점 잡으려고 0.5 더해줌
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
