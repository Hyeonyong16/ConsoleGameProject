#include "Camera.h"
#include "Level/GameLevel.h"
#include "Player.h"
#include "Actor/Wall.h"
#include "Actor/Item.h"
#include "Engine.h"

#include <string>
#include <cmath>

Camera::Camera(Player* _player)
	: ownerPlayer(_player)
{
	screen = new pixelInfo * [screenHeight];
	for (int i = 0; i < screenHeight; ++i)
	{
		screen[i] = new pixelInfo[screenWidth + 1];
	}
	for (int i = 0; i < screenHeight; ++i)
	{
		for (int j = 0; j < screenWidth + 1; ++j)
		{
			screen[i][j] = pixelInfo();
		}
	}
	int i = 0;

	depthBuffer = new float[screenWidth];
	for (int i = 0; i < screenWidth; ++i)
		depthBuffer[i] = 0.f;
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

	if (nullptr != depthBuffer)
	{
		delete[] depthBuffer;
		depthBuffer = nullptr;
	}
}

void Camera::SetCharinScreen(const char _c, Vector2 _pos)
{
	// screen 범위 밖이면 실행 X
	if (_pos.x < 0 || _pos.x > screenWidth - 1) return;
	if (_pos.y < 0 || _pos.y > screenHeight - 1) return;

	// 범위 안이면 해당 위치에 설정
	screen[_pos.y][_pos.x].text = _c;
}

void Camera::Tick(float _deltaTime)
{
	// 초기화
	for (int i = 0; i < screenHeight; ++i)
	{
		for (int j = 0; j < screenWidth; ++j)
		{
			screen[i][j].text = ' ';
			screen[i][j].color = Color::White;
			depthBuffer[i] = 0.f;
		}
	}

	// 플레이어 위치를 기준으로 진행해야함
	// 가로 픽셀만큼 레이캐스트 진행
	for (int i = 0; i < screenWidth; ++i)
	{
		// 현재 플레이어 기준 나가는 Ray 의 Angle
		// 최대를 왼쪽, 최소를 오른쪽 끝으로 잡고 콘솔 각 칸 수만큼 시야각을 나눠서 진행
		float rayAngle = (ownerPlayer->angle - (fov / 2)) + ((float)i / (float)screenWidth) * fov;

		// 각도를 범위 내 수치로 변경
		if (rayAngle < 0.0f) rayAngle += 360.0f;
		else if (rayAngle > 360.0f)	rayAngle -= 360.0f;

		float rayAngleRadian = rayAngle * PI / 180;
		Vec2Float rayAngleDir;
		rayAngleDir.x = cos(rayAngleRadian);
		rayAngleDir.y = sin(rayAngleRadian);

		float curDist = 0.f;
		float xDelta = fabs(1 / rayAngleDir.x); // x 값이 1 바뀔 때 직선 거리
		float yDelta = fabs(1 / rayAngleDir.y); // y 값이 1 바뀔 때 직선 거리
		float xDist = xDelta * (ownerPlayer->pos.x - trunc(ownerPlayer->pos.x));	// 처음 x값이 바뀔때 직선 거리
		float yDist = yDelta * (ownerPlayer->pos.y - trunc(ownerPlayer->pos.y));	// 처음 y값이 바뀔때 직선 거리

		// DDA좌표확인용
		int xPosDDA = ownerPlayer->position.x;
		int yPosDDA = ownerPlayer->position.y;

		// 각 카메라 각도에서 나오는 Ray 의 최대 길이
		float maxDist = fabs(dist / sin(rayAngleRadian));

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
			if (ownerPlayer->wallMap[yPosDDA][xPosDDA] != -1
				&& owner->FindActorByID(ownerPlayer->wallMap[yPosDDA][xPosDDA])->As<Wall>())
			{
				// 현재 위치에서 카메라 평면 상까지의 거리를 구함
				float wallDepth = fabs(curDist * cos((ownerPlayer->angle - rayAngle) * PI / 180.f));
				depthBuffer[i] = wallDepth;
				//벽 거리에 따라 그리는 반복문
				for (int j = 0; j < (screenHeight / wallDepth); ++j)
				{
					// 화면에서 벗어나지 않게 예외처리
					if (((screenHeight / 2) + j >= screenHeight) || ((screenHeight / 2) - j < 0)) continue;

					// 벽까지 거리에 따른 벽 Shading
					char shade = '#';

					if (wallDepth < dist / 5.f)						shade = '@';
					else if (wallDepth < dist / 4.f)				shade = '#';
					else if (wallDepth < dist / 3.f)				shade = '*';
					else if (wallDepth < dist)						shade = '.';
					else										    shade = ' ';

					screen[(int)((screenHeight / 2) + j)][screenWidth - i - 1] = { shade , Color::White };
					screen[(int)((screenHeight / 2) - j)][screenWidth - i - 1] = { shade , Color::White };
				}
				
				break;
			}
		}
	}

	// Todo: Item 그리기 확인용
	// 1. 플레이어 방향 벡터와 플레이어에서 아이템 방향 벡터 사이각 구하기
	// 2. 구해진 각도의 콘솔상에서 위치 구하기
	// 3. 해당 위치에서 일정 크기 만큼 계산
	// 4. 깊이 버퍼 계산해서 그릴 수 있는 만큼 만 그리기
	GameLevel* gameLevel = owner->As<GameLevel>();
	Vec2Float playerPos = ownerPlayer->pos;

	// 아이템이 들어있는 벡터를 받아서 반복문 돌리기
	for (int id : gameLevel->GetItemIDs())
	{
		Vector2 itemPos = owner->FindActorByID(id)->GetPosition();

		// 아이템과 플레이어 사이 거리 구하기
		float itemDist = sqrtf(((float)itemPos.x - playerPos.x) * ((float)itemPos.x - playerPos.x)
			+ ((float)itemPos.y - playerPos.y) * ((float)itemPos.y - playerPos.y));

		// 아이템 위치가 dist 보다 멀면 pass
		if (itemDist > dist) continue;

		// 내적으로 두 벡터 사이각 구하기
		ownerPlayer->dir; // 유저 방향벡터
		Vec2Float itemDir;
		itemDir.x = (float)itemPos.x - playerPos.x;
		itemDir.y = (float)itemPos.y - playerPos.y;

		// 아이템방향과 플레이어방향 사이 각(라디안)
		float itemPlayerRadian = acosf(((itemDir.x * ownerPlayer->dir.x) + (itemDir.y * ownerPlayer->dir.y))
			/ sqrtf((itemDir.x * itemDir.x) + (itemDir.y * itemDir.y)));
		float itemPlayerDegree = itemPlayerRadian * 180 / PI;

		float cross = (ownerPlayer->dir.x * itemDir.y) - (ownerPlayer->dir.y * itemDir.x);

		if (fabsf(itemPlayerDegree) < (45.f))
		{
			int i = 0;
			float rayAngle = (ownerPlayer->angle - (fov / 2)) + ((float)i / (float)screenWidth) * fov;
			if (cross > 0)
			{
				while(rayAngle < (ownerPlayer->angle - itemPlayerDegree))
				{
					++i;
					rayAngle = (ownerPlayer->angle - (fov / 2)) + ((float)i / (float)screenWidth) * fov;
				}
			}
			else
			{
				while (rayAngle < (ownerPlayer->angle + itemPlayerDegree))
				{
					++i;
					rayAngle = (ownerPlayer->angle - (fov / 2)) + ((float)i / (float)screenWidth) * fov;
				}
			}
			

			// 현재 플레이어 기준 나가는 Ray 의 Angle
			// 최대를 왼쪽, 최소를 오른쪽 끝으로 잡고 콘솔 각 칸 수만큼 시야각을 나눠서 진행

			// 각도를 범위 내 수치로 변경
			if (rayAngle < 0.0f) rayAngle += 360.0f;
			else if (rayAngle > 360.0f)	rayAngle -= 360.0f;

			float rayAngleRadian = rayAngle * PI / 180;
			Vec2Float rayAngleDir;
			rayAngleDir.x = cos(rayAngleRadian);
			rayAngleDir.y = sin(rayAngleRadian);

			float curDist = 0.f;
			float xDelta = fabs(1 / rayAngleDir.x); // x 값이 1 바뀔 때 직선 거리
			float yDelta = fabs(1 / rayAngleDir.y); // y 값이 1 바뀔 때 직선 거리
			float xDist = xDelta * (ownerPlayer->pos.x - trunc(ownerPlayer->pos.x));	// 처음 x값이 바뀔때 직선 거리
			float yDist = yDelta * (ownerPlayer->pos.y - trunc(ownerPlayer->pos.y));	// 처음 y값이 바뀔때 직선 거리

			// DDA좌표확인용
			int xPosDDA = ownerPlayer->position.x;
			int yPosDDA = ownerPlayer->position.y;

			while (curDist <= itemDist)
			{
				// yDist 가 xDist 보다 짧다 -> y = n 에서 만난다
				if (xDist > yDist)
				{
					// 교점까지 거리 ydist 임
					curDist = yDist;

					// yDist 의 값을 갱신
					yDist += yDelta;

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

					if (rayAngleDir.x > 0) ++xPosDDA;
					else --xPosDDA;
				}
			}

			// 현재 위치에서 카메라 평면 상까지의 거리를 구함
			float wallDepth = fabs(curDist * cos((ownerPlayer->angle - rayAngle) * PI / 180.f));
			//depthBuffer[i] = wallDepth;
			//벽 거리에 따라 그리는 반복문

			for (int ix = -2; ix < 3; ++ix)
			{
				for (int j = 0; j < 3; ++j)
				{
					// 화면에서 벗어나지 않게 예외처리
					if (((screenHeight / 2) + j >= screenHeight) || ((screenHeight / 2) - j < 0)) continue;
					if ((screenWidth - i - 1 + ix) < 0 || (screenWidth - i - 1 + ix) >= screenWidth) 
						continue;

					// 벽까지 거리에 따른 벽 Shading
					char shade = '^';

					if (wallDepth < depthBuffer[i])
					{
						screen[(int)((screenHeight / 2) + j)][screenWidth - i - 1 + ix] = { shade , Color::Cyan };
						screen[(int)((screenHeight / 2) - j)][screenWidth - i - 1 + ix] = { shade , Color::Cyan };
					}
					else
					{
						// Todo: Delta 값 오차 때문에 확 뛰어버려서 종종 출력이 안됨
						int a = 0;
					}
				}
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
			for(int j = 0; j < screenWidth; ++j)
				Engine::Get().WriteToBuffer(Vector2(j, i), screen[i][j].text, screen[i][j].color);
		}
	}
}
