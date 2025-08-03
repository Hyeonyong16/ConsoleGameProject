#include "Camera.h"
#include "Level/GameLevel.h"
#include "Player.h"
#include "Actor/Wall.h"
#include "Engine.h"

#include <cmath>

Camera::Camera(Player* _player)
	: ownerPlayer(_player)
{
	screen = new char*[screenHeight];
	for (int i = 0; i < screenHeight; ++i)
	{
		screen[i] = new char[screenWidth + 1];
	}
	for (int i = 0; i < screenHeight; ++i)
	{
		for (int j = 0; j < screenWidth + 1; ++j)
		{
			screen[i][j] = {'\0'};
		}
	}
	int i = 0;
}

Camera::~Camera()
{
	for (int i = 0; i < screenHeight; ++i)
	{
		if (nullptr != screen[i])
		{
			delete[] screen[i];
			screen[i] = nullptr;
		}
	}

	delete[] screen;
}

void Camera::SetCharinScreen(const char _c, Vector2 _pos)
{
	// screen 범위 밖이면 실행 X
	if (_pos.x < 0 || _pos.x > screenWidth - 1) return;
	if (_pos.y < 0 || _pos.y > screenHeight - 1) return;

	// 범위 안이면 해당 위치에 설정
	screen[_pos.y][_pos.x] = _c;
}

void Camera::Tick(float _deltaTime)
{
	// 초기화
	for (int i = 0; i < screenHeight; ++i)
	{
		for (int j = 0; j < screenWidth; ++j)
		{
			screen[i][j] = { ' ' };
		}
	}


	// 플레이어 위치를 기준으로 진행해야함
	// 가로 픽셀만큼 레이캐스트 진행
	for (int i = 0; i < screenWidth; ++i)
	{
		// 각 콘솔 왼쪽 ~ 오른쪽은 시야각임
		// - 왼쪽 : 플레이어 Angle - (fov / 2)
		// - 오른쪽 : 플레이어 Angle + (fov / 2);
		// - 콘솔 n번째 칸 시야각 : (Angle - (fov / 2)) + (fov / screenWidth) * n
		//		- 왼쪽부터 계산
		// 
		// - dist 보다 curDist 가 크기 전까지 DDA 방식 체크
		//		- 벽이 찍히면 해당 부분 (ScreenHeight / 2) 위치에 체크
		//		- Screen 중앙부터 ScreenHeight / dist 해서 위 아래로 벽


		float rayAngle = (ownerPlayer->angle - (fov / 2)) + ((float)i / (float)screenWidth) * fov;
		if (rayAngle < 0.0f)
		{
			rayAngle = 360.0f + rayAngle;
		}
		float rayAngleRadian = rayAngle * PI / 180;
		Vec2Float rayAngleDir;
		rayAngleDir.x = cos(rayAngleRadian);
		rayAngleDir.y = sin(rayAngleRadian);


		float curDist = 0.f;
		float xDelta = fabs(1 / rayAngleDir.x); // x 값이 1 바뀔 때 직선 거리
		float yDelta = fabs(1 / rayAngleDir.y); // y 값이 1 바뀔 때 직선 거리
		float xDist = xDelta * (ownerPlayer->pos.x - trunc(ownerPlayer->pos.x)); // 처음 x, y 값이 바뀔때 직선 거리
		float yDist = yDelta * (ownerPlayer->pos.y - trunc(ownerPlayer->pos.y));

		// DDA좌표확인용
		int xPosDDA = ownerPlayer->position.x;
		int yPosDDA = ownerPlayer->position.y;
		
		float maxDist = fabs(dist / sin(rayAngleRadian));
		if (maxDist > dist) maxDist = dist;

		while (curDist <= maxDist)
		{
			// yDist 가 xDist 보다 짧다 -> y = n 에서 만난다
			if (xDist > yDist)
			{
				// 교점까지 거리 ydist 임
				curDist = yDist;

				// yDist 의 값을 갱신
				yDist += yDelta;

				// 방향에 따른 y좌표 변경
				if (rayAngleDir.y > 0) --yPosDDA;
				else ++yPosDDA;
			}
			// xDist 가 yDist 보다 짧다 -> x = n 에서 만난다
			else
			{
				// 교점까지 거리 xdist 임
				curDist = xDist;

				// xDist 의 값을 갱신
				xDist += xDelta;

				// 방향에 따른 x좌표 변경
				if (rayAngleDir.x > 0) ++xPosDDA;
				else --xPosDDA;
			}

			// 정면 범위 내에 벽이 찍히면(ID 값이니까 벽 아니여도 가능하긴 함)
			if (xPosDDA < 0 || xPosDDA >= (dynamic_cast<GameLevel*>(owner)->GetMapWidth())) return;
			if (yPosDDA < 0 || yPosDDA >= (dynamic_cast<GameLevel*>(owner)->GetMapHeight())) return;

			// 벽을 찍었으면
			if (ownerPlayer->wallMap[yPosDDA][xPosDDA] != -1)
			{
				Actor* tempWall = owner->FindActorByID(ownerPlayer->wallMap[yPosDDA][xPosDDA]);
				// 벽으로 테스트용
				dynamic_cast<Wall*>(tempWall)->SetIsCheck(true);
				float wallDepth = fabs(curDist * cos((ownerPlayer->angle - rayAngle) * PI / 180.f));
				/*if(rayAngle >= 0.f && rayAngle < 90.f) wallDepth = fabs(curDist * cos(rayAngleRadian)); 
				else if (rayAngle >= 90.f && rayAngle < 180.f) wallDepth = fabs(curDist * cos(rayAngleRadian - PI/4));
				else if (rayAngle >= 180.f && rayAngle < 270.f) wallDepth = fabs(curDist * cos(rayAngleRadian - PI / 2));
				else wallDepth = fabs(curDist * cos(rayAngleRadian - (PI / 4) * 3)); ;*/

				for (int j = 0; j < (screenHeight / wallDepth); ++j)
				{
					if (((screenHeight / 2) + j >= screenHeight) || ((screenHeight / 2) - j < 0)) continue;

					char shade = '#';

					if (wallDepth < dist/5.f)						shade = '#';
					else if (wallDepth < dist/3.f)					shade = '*';
					else if (wallDepth < dist)						shade = '.';
					else										    shade = ' ';

					screen[(int)((screenHeight / 2) + j)][screenWidth - i - 1] = shade;
					screen[(int)((screenHeight / 2) - j)][screenWidth - i - 1] = shade;
				}

				break;
			}
		}
	}
}

void Camera::Render()
{
	if (dynamic_cast<GameLevel*>(owner)->isFPS)
	{
		for (int i = 0; i < screenHeight; ++i)
		{
			Engine::Get().WriteToBuffer(Vector2(0, i), screen[i], Color::White);
		}
	}
}
