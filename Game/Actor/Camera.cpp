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
	// screen ���� ���̸� ���� X
	if (_pos.x < 0 || _pos.x > screenWidth - 1) return;
	if (_pos.y < 0 || _pos.y > screenHeight - 1) return;

	// ���� ���̸� �ش� ��ġ�� ����
	screen[_pos.y][_pos.x] = _c;
}

void Camera::Tick(float _deltaTime)
{
	// �ʱ�ȭ
	for (int i = 0; i < screenHeight; ++i)
	{
		for (int j = 0; j < screenWidth; ++j)
		{
			screen[i][j] = { ' ' };
		}
	}


	// �÷��̾� ��ġ�� �������� �����ؾ���
	// ���� �ȼ���ŭ ����ĳ��Ʈ ����
	for (int i = 0; i < screenWidth; ++i)
	{
		// �� �ܼ� ���� ~ �������� �þ߰���
		// - ���� : �÷��̾� Angle - (fov / 2)
		// - ������ : �÷��̾� Angle + (fov / 2);
		// - �ܼ� n��° ĭ �þ߰� : (Angle - (fov / 2)) + (fov / screenWidth) * n
		//		- ���ʺ��� ���
		// 
		// - dist ���� curDist �� ũ�� ������ DDA ��� üũ
		//		- ���� ������ �ش� �κ� (ScreenHeight / 2) ��ġ�� üũ
		//		- Screen �߾Ӻ��� ScreenHeight / dist �ؼ� �� �Ʒ��� ��


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
		float xDelta = fabs(1 / rayAngleDir.x); // x ���� 1 �ٲ� �� ���� �Ÿ�
		float yDelta = fabs(1 / rayAngleDir.y); // y ���� 1 �ٲ� �� ���� �Ÿ�
		float xDist = xDelta * (ownerPlayer->pos.x - trunc(ownerPlayer->pos.x)); // ó�� x, y ���� �ٲ� ���� �Ÿ�
		float yDist = yDelta * (ownerPlayer->pos.y - trunc(ownerPlayer->pos.y));

		// DDA��ǥȮ�ο�
		int xPosDDA = ownerPlayer->position.x;
		int yPosDDA = ownerPlayer->position.y;
		
		float maxDist = fabs(dist / sin(rayAngleRadian));
		if (maxDist > dist) maxDist = dist;

		while (curDist <= maxDist)
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
			if (ownerPlayer->wallMap[yPosDDA][xPosDDA] != -1)
			{
				Actor* tempWall = owner->FindActorByID(ownerPlayer->wallMap[yPosDDA][xPosDDA]);
				// ������ �׽�Ʈ��
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
