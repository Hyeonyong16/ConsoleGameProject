#include "Player.h"
#include "Actor/Actor.h"
#include "Level/Level.h"
#include "Level/GameLevel.h"
#include "Actor/Wall.h"
#include "Actor/Item.h"
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

	cam = new Camera(this);
	owner->AddActor(cam);

	// GameLevel 에서 wall 배치 받아오기
	if (GetOwner()->As<GameLevel>())
	{
		wallMap = dynamic_cast<GameLevel*>(GetOwner())->getWallMap();
	}
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
		Engine::Get().Quit();
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

				tempItem->SetPlayerCollision(true);
			}
			int a = 0;
		}
	}
}

void Player::Render()
{
	//if(dynamic_cast<GameLevel*>(owner)->isFPS == false)
	//{
	super::Render();

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
	//}
}
