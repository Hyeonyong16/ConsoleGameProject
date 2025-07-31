#include "Player.h"
#include "Actor/Actor.h"
#include "Engine.h"
#include "Utils/Utils.h"

#include <iostream>

// Todo
// 플레이어 이동, 회전
//

Player::Player()
	: Actor("P", Color::Red),
	speed(0.05f), rotSpeed(0.1f)
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

	// ESC = 종료
	if (Input::Get().GetKey(VK_ESCAPE))
	{
		Engine::Get().Quit();
	}

	// 방향키 입력
	if (Input::Get().GetKey('A'))
	{
		pos.x -= speed;
		if (pos.x < 0.0f) pos.x = 0.0f;
		SetPosition(pos);
	}
	if (Input::Get().GetKey('D'))
	{
		pos.x += speed;
		if (pos.x > (float)(Engine::Get().GetWidth())) pos.x = (float)Engine::Get().GetWidth();
		SetPosition(pos);
	}
	if (Input::Get().GetKey('W'))
	{
		pos.y -= speed;
		if (pos.y < 0.0f) pos.y = 0.0f;
		SetPosition(pos);
	}
	if (Input::Get().GetKey('S'))
	{
		pos.y += speed;
		if (pos.y > (float)(Engine::Get().GetHeight())) pos.y = (float)Engine::Get().GetHeight();
		SetPosition(pos);
	}
}

void Player::Render()
{
	super::Render();

	// 디버그용 플레이어 좌표
	{
		Utils::SetConsoleTextColor(Color::Red);
		Utils::SetConsolePosition(Vector2(70, 3));
		std::cout << "                             ";
		Utils::SetConsolePosition(Vector2(70, 3));
		std::cout << "position x: " << position.x << " y: " << position.y;
		Utils::SetConsolePosition(Vector2(70, 4));
		std::cout << "                             ";
		Utils::SetConsolePosition(Vector2(70, 4));
		std::cout << "pos x: " << pos.x << " y: " << pos.y;
	}
}
