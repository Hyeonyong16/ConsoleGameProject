#include "Player.h"
#include "Actor/Actor.h"
#include "Engine.h"
#include "Core.h"

#include "Utils/Utils.h"

#include <iostream>

// Todo
// 플레이어 회전
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

	// 위치 이동이 이번 프레임에 있었는지 여부 : 있으면 위치 설정 다시 해줌
	bool isPosChange = false;

	// 삼각함수가 radian 기준 : degree > radian
	float angleRadian = angle * PI / 180;

	// Player 의 방향 벡터 구함
	dir.x = cos(angleRadian);
	dir.y = sin(angleRadian);

	// ESC = 종료
	if (Input::Get().GetKey(VK_ESCAPE))
	{
		Engine::Get().Quit();
	}

	// 방향키 입력
	// W 를 누르면 방향벡터쪽으로 이동 (cos, sin)
	// S 를 누르면 반대로	(-cos, -sin)
	// A 수직인 벡터 왼쪽 ( -sin, cos)
	// D 수직인 벡터 오른쪽 (sin, -cos)
	// y축 + 방향이 아래쪽 이므로 y 방향에 - 붙여줌
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

	// 조건이 다 겹치니까 좌표 변경이 생기면 한번에
	if (isPosChange)
	{
		// 화면 이탈 방지
		if (pos.y < 0.0f) pos.y = 0.0f;
		if (pos.y > (float)(Engine::Get().GetHeight())) pos.y = (float)Engine::Get().GetHeight();
		if (pos.x < 0.0f) pos.x = 0.0f;
		if (pos.x > (float)(Engine::Get().GetWidth())) pos.x = (float)Engine::Get().GetWidth();

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
}

void Player::Render()
{
	super::Render();

	// 디버그용 플레이어 좌표
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
