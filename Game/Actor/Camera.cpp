#include "Camera.h"
#include "Level/GameLevel.h"
#include "Player.h"
#include "Actor/Wall.h"
#include "Actor/Item.h"
#include "Actor/Monster.h"
#include "Actor/Portal.h"
#include "Engine.h"

#include <string>
#include <cmath>

// 렌더링 시 거리에 따른 비율 설정을 위한 static 전역 변수
static float itemScale = 3.0f;
static float monsterScale = 1.5f;

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
		depthBuffer[i] = 20.f;
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

// DDA 방식을 활용한 Actor를 그리는 함수
void Camera::DrawActorByDDA(std::vector<int> _actorIDs, float _renderScale)
{
	GameLevel* gameLevel = owner->As<GameLevel>();
	Vec2Float playerPos = ownerPlayer->pos;

	// 아이템이 들어있는 벡터를 받아서 반복문 돌리기
	for (int id : _actorIDs)
	{
		// 아이템의 위치 (사각형 기준 원점 잡으려고 0.5 더해줌
		Vec2Float itemPos;
		itemPos.x = (float)owner->FindActorByID(id)->GetPosition().x + 0.5f;
		itemPos.y = (float)owner->FindActorByID(id)->GetPosition().y + 0.5f;

		// 아이템과 플레이어 사이 거리 구하기
		float itemDist = sqrtf(((float)itemPos.x - playerPos.x) * ((float)itemPos.x - playerPos.x)
			+ ((float)itemPos.y - playerPos.y) * ((float)itemPos.y - playerPos.y));

		// 아이템 위치가 dist 보다 멀면 pass
		if (itemDist > dist) continue;

		// 내적으로 두 벡터 사이각 구하기
		// ownerPlayer->dir; // 유저 방향벡터
		Vec2Float itemDir;
		itemDir.x = (float)itemPos.x - playerPos.x;
		itemDir.y = (float)itemPos.y - playerPos.y;

		// 아이템방향과 플레이어방향 사이 각(라디안)
		float itemPlayerRadian = acosf(((itemDir.x * ownerPlayer->dir.x) + (itemDir.y * ownerPlayer->dir.y))
			/ sqrtf((itemDir.x * itemDir.x) + (itemDir.y * itemDir.y)));
		float itemPlayerDegree = itemPlayerRadian * 180 / PI;

		// 플레이어 방향 벡터 기준 아이템 위치의 좌, 우 판별을 위한 외적값
		float cross = (ownerPlayer->dir.x * itemDir.y) - (ownerPlayer->dir.y * itemDir.x);

		// 플레이어와 아이템 사이 각도 차이 절대값이 시야각 내부일때 진행 
		if (fabsf(itemPlayerDegree) < (45.f))
		{
			int i = 0;
			int maxDraw = 0;

			float rayAngle = (ownerPlayer->angle - (fov / 2)) + ((float)i / (float)screenWidth) * fov;
			// 외적값이 양수이면 오른쪽
			if (cross > 0)
			{
				// 현재 ray의 각도가 아이템 방향 각도가 될 때까지 (float 오차로 정확하지 않으니 커지면 바로 탈출)
				while (rayAngle < (ownerPlayer->angle - itemPlayerDegree))
				{
					++i;
					// ray 각도 값 갱신
					rayAngle = (ownerPlayer->angle - (fov / 2)) + ((float)i / (float)screenWidth) * fov;
				}

				float maxDrawAngleDegree = asinf(0.5f / itemDist) * 180 / PI;
				while ((((float)maxDraw / (float)screenWidth) * fov) < maxDrawAngleDegree)
				{
					++maxDraw;
				}

				int a = 0;
			}
			// 외적값이 음수이면 왼쪽
			else
			{
				// 현재 ray의 각도가 아이템 방향 각도가 될 때까지 (float 오차로 정확하지 않으니 커지면 바로 탈출)
				while (rayAngle < (ownerPlayer->angle + itemPlayerDegree))
				{
					++i;
					// ray 각도 값 갱신
					rayAngle = (ownerPlayer->angle - (fov / 2)) + ((float)i / (float)screenWidth) * fov;
				}

				float maxDrawAngleDegree = asinf(0.5f / itemDist) * 180 / PI;
				while ((((float)maxDraw / (float)screenWidth) * fov) < maxDrawAngleDegree)
				{
					++maxDraw;
				}

				int a = 0;
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
			float xDelta = rayAngleDir.x == 0.f ? 0.f : fabs(1 / rayAngleDir.x); // x 값이 1 바뀔 때 직선 거리
			float yDelta = rayAngleDir.y == 0.f ? 0.f : fabs(1 / rayAngleDir.y); // y 값이 1 바뀔 때 직선 거리
			//float xDist = xDelta * (ownerPlayer->pos.x - trunc(ownerPlayer->pos.x));	// 처음 x값이 바뀔때 직선 거리
			//float yDist = yDelta * (ownerPlayer->pos.y - trunc(ownerPlayer->pos.y));	// 처음 y값이 바뀔때 직선 거리

			// 방향에 따라 거리가 달라짐
			float xDist;
			float yDist;
			if ((rayAngle > 0.f && rayAngle <= 90.f) || (rayAngle > 270.f && rayAngle <= 360.f))
			{
				xDist = xDelta * (1 - (ownerPlayer->pos.x - trunc(ownerPlayer->pos.x)));
			}
			else
			{
				xDist = xDelta * (ownerPlayer->pos.x - trunc(ownerPlayer->pos.x));
			}

			if (rayAngle > 0.f && rayAngle <= 180.f)
			{
				yDist = yDelta * (ownerPlayer->pos.y - trunc(ownerPlayer->pos.y));
			}
			else
			{
				yDist = yDelta * (1 - (ownerPlayer->pos.y - trunc(ownerPlayer->pos.y)));
			}

			// DDA좌표확인용
			int xPosDDA = ownerPlayer->position.x;
			int yPosDDA = ownerPlayer->position.y;

			while (curDist <= itemDist)
			{

				if (xDelta == 0) xDist = 9999999;
				else if (yDelta == 0) yDist = 9999999;
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
			float wallDepth = fabs(itemDist * cos((ownerPlayer->angle - rayAngle) * PI / 180.f));
			//벽 거리에 따라 그리는 반복문

			for (int ix = -(maxDraw / 2);
				ix < (maxDraw / 2); ++ix)
			{
				bool isDraw = false;
				for (int j = 0;
					j < ((screenHeight / _renderScale) / wallDepth) / 2; ++j)
				{
					// 화면에서 벗어나지 않게 예외처리
					if (((screenHeight / 2) + j >= screenHeight)
						|| ((screenHeight / 2) - j < 0)) continue;
					if ((screenWidth - i - 1 + ix) < 0
						|| (screenWidth - i - 1 + ix) >= screenWidth) continue;

					// 벽까지 거리에 따른 벽 Shading
					char shade = 'o';

					if (wallDepth < depthBuffer[i - ix])
					{
						isDraw = true;
						screen[(int)((screenHeight / 2) + j)][screenWidth - i - 1 + ix] = {
							owner->FindActorByID(id)->GetImage()[0],
							owner->FindActorByID(id)->GetColor()
						};
						screen[(int)((screenHeight / 2) - j)][screenWidth - i - 1 + ix]
							= { owner->FindActorByID(id)->GetImage()[0],
							owner->FindActorByID(id)->GetColor()
						};
					}

					// 몬스터 죽음 처리
					if (owner->FindActorByID(id)->As<Monster>() && owner->FindActorByID(id)->As<Monster>()->GetIsKilled())
					{
						if ((((screenHeight / _renderScale) / wallDepth)
							* owner->FindActorByID(id)->As<Monster>()->GetDeathTimer().GetElapsedTime()) > j)
						{
							isDraw = true;
							screen[(int)((screenHeight / 2) + j)][screenWidth - i - 1 + ix] = {
								owner->FindActorByID(id)->GetImage()[0],
								Color::Yellow
							};
							screen[(int)((screenHeight / 2) - j)][screenWidth - i - 1 + ix]
								= { owner->FindActorByID(id)->GetImage()[0],
								Color::Yellow
							};
						}
						else
						{
							isDraw = true;
							screen[(int)((screenHeight / 2) + j)][screenWidth - i - 1 + ix] = {
								owner->FindActorByID(id)->GetImage()[0],
								owner->FindActorByID(id)->GetColor()
							};
							screen[(int)((screenHeight / 2) - j)][screenWidth - i - 1 + ix]
								= { owner->FindActorByID(id)->GetImage()[0],
								owner->FindActorByID(id)->GetColor()
							};
						}
					}


				}
				if (isDraw) depthBuffer[i - ix] = wallDepth;
			}
		}
	}
}

void Camera::DrawActorByDDA(int _actorId, float _renderScale)
{
	GameLevel* gameLevel = owner->As<GameLevel>();
	Vec2Float playerPos = ownerPlayer->pos;
	int id = _actorId;

	// 아이템의 위치 (사각형 기준 원점 잡으려고 0.5 더해줌
	Vec2Float itemPos;
	itemPos.x = (float)owner->FindActorByID(id)->GetPosition().x + 0.5f;
	itemPos.y = (float)owner->FindActorByID(id)->GetPosition().y + 0.5f;

	// 아이템과 플레이어 사이 거리 구하기
	float itemDist = sqrtf(((float)itemPos.x - playerPos.x) * ((float)itemPos.x - playerPos.x)
		+ ((float)itemPos.y - playerPos.y) * ((float)itemPos.y - playerPos.y));

	// 아이템 위치가 dist 보다 멀면 pass
	if (itemDist > dist) return;

	// 내적으로 두 벡터 사이각 구하기
	// ownerPlayer->dir; // 유저 방향벡터
	Vec2Float itemDir;
	itemDir.x = (float)itemPos.x - playerPos.x;
	itemDir.y = (float)itemPos.y - playerPos.y;

	// 아이템방향과 플레이어방향 사이 각(라디안)
	float itemPlayerRadian = acosf(((itemDir.x * ownerPlayer->dir.x) + (itemDir.y * ownerPlayer->dir.y))
		/ sqrtf((itemDir.x * itemDir.x) + (itemDir.y * itemDir.y)));
	float itemPlayerDegree = itemPlayerRadian * 180 / PI;

	// 플레이어 방향 벡터 기준 아이템 위치의 좌, 우 판별을 위한 외적값
	float cross = (ownerPlayer->dir.x * itemDir.y) - (ownerPlayer->dir.y * itemDir.x);

	// 플레이어와 아이템 사이 각도 차이 절대값이 시야각 내부일때 진행 
	if (fabsf(itemPlayerDegree) < (45.f))
	{
		int i = 0;
		int maxDraw = 0;

		float rayAngle = (ownerPlayer->angle - (fov / 2)) + ((float)i / (float)screenWidth) * fov;
		// 외적값이 양수이면 오른쪽
		if (cross > 0)
		{
			// 현재 ray의 각도가 아이템 방향 각도가 될 때까지 (float 오차로 정확하지 않으니 커지면 바로 탈출)
			while (rayAngle < (ownerPlayer->angle - itemPlayerDegree))
			{
				++i;
				// ray 각도 값 갱신
				rayAngle = (ownerPlayer->angle - (fov / 2)) + ((float)i / (float)screenWidth) * fov;
			}

			float maxDrawAngleDegree = asinf(0.5f / itemDist) * 180 / PI;
			while ((((float)maxDraw / (float)screenWidth) * fov) < maxDrawAngleDegree)
			{
				++maxDraw;
			}

			int a = 0;
		}
		// 외적값이 음수이면 왼쪽
		else
		{
			// 현재 ray의 각도가 아이템 방향 각도가 될 때까지 (float 오차로 정확하지 않으니 커지면 바로 탈출)
			while (rayAngle < (ownerPlayer->angle + itemPlayerDegree))
			{
				++i;
				// ray 각도 값 갱신
				rayAngle = (ownerPlayer->angle - (fov / 2)) + ((float)i / (float)screenWidth) * fov;
			}

			float maxDrawAngleDegree = asinf(0.5f / itemDist) * 180 / PI;
			while ((((float)maxDraw / (float)screenWidth) * fov) < maxDrawAngleDegree)
			{
				++maxDraw;
			}

			int a = 0;
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
		float xDelta = rayAngleDir.x == 0.f ? 0.f : fabs(1 / rayAngleDir.x); // x 값이 1 바뀔 때 직선 거리
		float yDelta = rayAngleDir.y == 0.f ? 0.f : fabs(1 / rayAngleDir.y); // y 값이 1 바뀔 때 직선 거리
		//float xDist = xDelta * (ownerPlayer->pos.x - trunc(ownerPlayer->pos.x));	// 처음 x값이 바뀔때 직선 거리
		//float yDist = yDelta * (ownerPlayer->pos.y - trunc(ownerPlayer->pos.y));	// 처음 y값이 바뀔때 직선 거리

		// 방향에 따라 거리가 달라짐
		float xDist;
		float yDist;
		if ((rayAngle > 0.f && rayAngle <= 90.f) || (rayAngle > 270.f && rayAngle <= 360.f))
		{
			xDist = xDelta * (1 - (ownerPlayer->pos.x - trunc(ownerPlayer->pos.x)));
		}
		else
		{
			xDist = xDelta * (ownerPlayer->pos.x - trunc(ownerPlayer->pos.x));
		}

		if (rayAngle > 0.f && rayAngle <= 180.f)
		{
			yDist = yDelta * (ownerPlayer->pos.y - trunc(ownerPlayer->pos.y));
		}
		else
		{
			yDist = yDelta * (1 - (ownerPlayer->pos.y - trunc(ownerPlayer->pos.y)));
		}

		// DDA좌표확인용
		int xPosDDA = ownerPlayer->position.x;
		int yPosDDA = ownerPlayer->position.y;

		while (curDist <= itemDist)
		{
			if (xDelta == 0) xDist = 9999999;
			else if (yDelta == 0) yDist = 9999999;
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
		float wallDepth = fabs(itemDist * cos((ownerPlayer->angle - rayAngle) * PI / 180.f));
		//벽 거리에 따라 그리는 반복문

		for (int ix = -(maxDraw / 2);
			ix < (maxDraw / 2); ++ix)
		{
			bool isDraw = false;
			for (int j = 0;
				j < ((screenHeight / _renderScale) / wallDepth) / 2; ++j)
			{
				// 화면에서 벗어나지 않게 예외처리
				if (((screenHeight / 2) + j >= screenHeight)
					|| ((screenHeight / 2) - j < 0)) continue;
				if ((screenWidth - i - 1 + ix) < 0
					|| (screenWidth - i - 1 + ix) >= screenWidth) continue;

				// 벽까지 거리에 따른 벽 Shading
				char shade = 'o';

				if (wallDepth < depthBuffer[i - ix])
				{
					isDraw = true;
					screen[(int)((screenHeight / 2) + j)][screenWidth - i - 1 + ix] = {
						owner->FindActorByID(id)->GetImage()[0],
						owner->FindActorByID(id)->GetColor()
					};
					screen[(int)((screenHeight / 2) - j)][screenWidth - i - 1 + ix]
						= { owner->FindActorByID(id)->GetImage()[0],
						owner->FindActorByID(id)->GetColor()
					};
				}

				// 몬스터 죽음 처리
				if (owner->FindActorByID(id)->As<Monster>() && owner->FindActorByID(id)->As<Monster>()->GetIsKilled())
				{
					if ((((screenHeight / _renderScale) / wallDepth)
						* owner->FindActorByID(id)->As<Monster>()->GetDeathTimer().GetElapsedTime()) > j)
					{
						isDraw = true;
						screen[(int)((screenHeight / 2) + j)][screenWidth - i - 1 + ix] = {
							owner->FindActorByID(id)->GetImage()[0],
							Color::Yellow
						};
						screen[(int)((screenHeight / 2) - j)][screenWidth - i - 1 + ix]
							= { owner->FindActorByID(id)->GetImage()[0],
							Color::Yellow
						};
					}
					else
					{
						isDraw = true;
						screen[(int)((screenHeight / 2) + j)][screenWidth - i - 1 + ix] = {
							owner->FindActorByID(id)->GetImage()[0],
							owner->FindActorByID(id)->GetColor()
						};
						screen[(int)((screenHeight / 2) - j)][screenWidth - i - 1 + ix]
							= { owner->FindActorByID(id)->GetImage()[0],
							owner->FindActorByID(id)->GetColor()
						};
					}
				}


			}
			if (isDraw) depthBuffer[i - ix] = wallDepth;
		}
	}

}

void Camera::BeginPlay()
{
	if (nullptr != owner->As<GameLevel>())
	{
		owner->As<GameLevel>()->SetCamera(this);
	}
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

		// 각도를 범위 내 수치로 변경(0 ~ 360)
		if (rayAngle < 0.0f) rayAngle += 360.0f;
		else if (rayAngle > 360.0f)	rayAngle -= 360.0f;

		float rayAngleRadian = rayAngle * PI / 180;
		Vec2Float rayAngleDir;
		rayAngleDir.x = cos(rayAngleRadian);
		rayAngleDir.y = sin(rayAngleRadian);

		float curDist = 0.f;
		float xDelta = rayAngleDir.x == 0.f ? 0.f : fabs(1 / rayAngleDir.x); // x 값이 1 바뀔 때 직선 거리
		float yDelta = rayAngleDir.y == 0.f ? 0.f : fabs(1 / rayAngleDir.y); // y 값이 1 바뀔 때 직선 거리
		//float xDist = xDelta * (1 - (ownerPlayer->pos.x - trunc(ownerPlayer->pos.x)));	// 처음 x값이 바뀔때 직선 거리
		//float yDist = yDelta * (1 - (ownerPlayer->pos.y - trunc(ownerPlayer->pos.y)));	// 처음 y값이 바뀔때 직선 거리

		// 방향에 따라 거리가 달라짐
		float xDist;
		float yDist;
		if ((rayAngle > 0.f && rayAngle <= 90.f) || (rayAngle > 270.f && rayAngle <= 360.f))
		{
			xDist = xDelta * (1 - (ownerPlayer->pos.x - trunc(ownerPlayer->pos.x)));
		}
		else
		{
			xDist = xDelta * (ownerPlayer->pos.x - trunc(ownerPlayer->pos.x));
		}

		if (rayAngle > 0.f && rayAngle <= 180.f)
		{
			yDist = yDelta * (ownerPlayer->pos.y - trunc(ownerPlayer->pos.y));
		}
		else
		{
			yDist = yDelta * (1 - (ownerPlayer->pos.y - trunc(ownerPlayer->pos.y)));
		}

		// DDA좌표확인용
		int xPosDDA = ownerPlayer->pos.x;
		int yPosDDA = ownerPlayer->pos.y;

		// 각 카메라 각도에서 나오는 Ray 의 최대 길이
		float maxDist = fabs(dist * cos((ownerPlayer->angle - rayAngle) * PI / 180));

		float tempDist = 0.f;
		while (curDist <= maxDist)
		{
			if (xDelta == 0) xDist = 9999999;
			else if (yDelta == 0) yDist = 9999999;
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
				//&& curDist > 0.1f)
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

					//if(owner->FindActorByID(ownerPlayer->wallMap[yPosDDA][xPosDDA])->As<Wall>()->GetColor() == Color::Magenta)
					screen[(int)((screenHeight / 2) + j)][screenWidth - i - 1] = { shade
						, owner->FindActorByID(ownerPlayer->wallMap[yPosDDA][xPosDDA])->As<Wall>()->GetColor() };
					screen[(int)((screenHeight / 2) - j)][screenWidth - i - 1] = { shade
						, owner->FindActorByID(ownerPlayer->wallMap[yPosDDA][xPosDDA])->As<Wall>()->GetColor() };
				}

				break;
			}
		}
	}

	// Item 그리기 확인용
	// 1. 플레이어 방향 벡터와 플레이어에서 아이템 방향 벡터 사이각 구하기
	// 2. 구해진 각도의 콘솔상에서 위치 구하기
	// 3. 해당 위치에서 일정 크기 만큼 계산
	// 4. 깊이 버퍼 계산해서 그릴 수 있는 만큼 만 그리기
	GameLevel* gameLevel = owner->As<GameLevel>();
	Vec2Float playerPos = ownerPlayer->pos;

	// Actor 그리기
	DrawActorByDDA(gameLevel->GetItemIDs(), itemScale);
	DrawActorByDDA(gameLevel->GetMonsterIDs(), monsterScale);

	// Portal 이 활성화 되어있으면 그리기
	if (owner->FindActorByID(gameLevel->GetPortalID())->As<Portal>()->GetIsActive())
	{
		DrawActorByDDA(gameLevel->GetPortalID(), 1.f);
	}
}

void Camera::Render()
{
	if (dynamic_cast<GameLevel*>(owner)->isFPS)
	{
		for (int i = 0; i < screenHeight; ++i)
		{
			for (int j = 0; j < screenWidth; ++j)
				Engine::Get().WriteToBuffer(Vector2(j + 1, i + 1), screen[i][j].text, screen[i][j].color);
		}
	}
}
