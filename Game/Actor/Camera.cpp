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
	// screen ���� ���̸� ���� X
	if (_pos.x < 0 || _pos.x > screenWidth - 1) return;
	if (_pos.y < 0 || _pos.y > screenHeight - 1) return;

	// ���� ���̸� �ش� ��ġ�� ����
	screen[_pos.y][_pos.x].text = _c;
}

void Camera::Tick(float _deltaTime)
{
	// �ʱ�ȭ
	for (int i = 0; i < screenHeight; ++i)
	{
		for (int j = 0; j < screenWidth; ++j)
		{
			screen[i][j].text = ' ';
			screen[i][j].color = Color::White;
			depthBuffer[i] = 0.f;
		}
	}

	// �÷��̾� ��ġ�� �������� �����ؾ���
	// ���� �ȼ���ŭ ����ĳ��Ʈ ����
	for (int i = 0; i < screenWidth; ++i)
	{
		// ���� �÷��̾� ���� ������ Ray �� Angle
		// �ִ븦 ����, �ּҸ� ������ ������ ��� �ܼ� �� ĭ ����ŭ �þ߰��� ������ ����
		float rayAngle = (ownerPlayer->angle - (fov / 2)) + ((float)i / (float)screenWidth) * fov;

		// ������ ���� �� ��ġ�� ����
		if (rayAngle < 0.0f) rayAngle += 360.0f;
		else if (rayAngle > 360.0f)	rayAngle -= 360.0f;

		float rayAngleRadian = rayAngle * PI / 180;
		Vec2Float rayAngleDir;
		rayAngleDir.x = cos(rayAngleRadian);
		rayAngleDir.y = sin(rayAngleRadian);

		float curDist = 0.f;
		float xDelta = fabs(1 / rayAngleDir.x); // x ���� 1 �ٲ� �� ���� �Ÿ�
		float yDelta = fabs(1 / rayAngleDir.y); // y ���� 1 �ٲ� �� ���� �Ÿ�
		float xDist = xDelta * (ownerPlayer->pos.x - trunc(ownerPlayer->pos.x));	// ó�� x���� �ٲ� ���� �Ÿ�
		float yDist = yDelta * (ownerPlayer->pos.y - trunc(ownerPlayer->pos.y));	// ó�� y���� �ٲ� ���� �Ÿ�

		// DDA��ǥȮ�ο�
		int xPosDDA = ownerPlayer->position.x;
		int yPosDDA = ownerPlayer->position.y;

		// �� ī�޶� �������� ������ Ray �� �ִ� ����
		float maxDist = fabs(dist / sin(rayAngleRadian));

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
			if (ownerPlayer->wallMap[yPosDDA][xPosDDA] != -1
				&& owner->FindActorByID(ownerPlayer->wallMap[yPosDDA][xPosDDA])->As<Wall>())
			{
				// ���� ��ġ���� ī�޶� ��� ������� �Ÿ��� ����
				float wallDepth = fabs(curDist * cos((ownerPlayer->angle - rayAngle) * PI / 180.f));
				depthBuffer[i] = wallDepth;
				//�� �Ÿ��� ���� �׸��� �ݺ���
				for (int j = 0; j < (screenHeight / wallDepth); ++j)
				{
					// ȭ�鿡�� ����� �ʰ� ����ó��
					if (((screenHeight / 2) + j >= screenHeight) || ((screenHeight / 2) - j < 0)) continue;

					// ������ �Ÿ��� ���� �� Shading
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

	// Todo: Item �׸��� Ȯ�ο�
	// 1. �÷��̾� ���� ���Ϳ� �÷��̾�� ������ ���� ���� ���̰� ���ϱ�
	// 2. ������ ������ �ֻܼ󿡼� ��ġ ���ϱ�
	// 3. �ش� ��ġ���� ���� ũ�� ��ŭ ���
	// 4. ���� ���� ����ؼ� �׸� �� �ִ� ��ŭ �� �׸���
	GameLevel* gameLevel = owner->As<GameLevel>();
	Vec2Float playerPos = ownerPlayer->pos;

	// �������� ����ִ� ���͸� �޾Ƽ� �ݺ��� ������
	for (int id : gameLevel->GetItemIDs())
	{
		Vector2 itemPos = owner->FindActorByID(id)->GetPosition();

		// �����۰� �÷��̾� ���� �Ÿ� ���ϱ�
		float itemDist = sqrtf(((float)itemPos.x - playerPos.x) * ((float)itemPos.x - playerPos.x)
			+ ((float)itemPos.y - playerPos.y) * ((float)itemPos.y - playerPos.y));

		// ������ ��ġ�� dist ���� �ָ� pass
		if (itemDist > dist) continue;

		// �������� �� ���� ���̰� ���ϱ�
		ownerPlayer->dir; // ���� ���⺤��
		Vec2Float itemDir;
		itemDir.x = (float)itemPos.x - playerPos.x;
		itemDir.y = (float)itemPos.y - playerPos.y;

		// �����۹���� �÷��̾���� ���� ��(����)
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
			

			// ���� �÷��̾� ���� ������ Ray �� Angle
			// �ִ븦 ����, �ּҸ� ������ ������ ��� �ܼ� �� ĭ ����ŭ �þ߰��� ������ ����

			// ������ ���� �� ��ġ�� ����
			if (rayAngle < 0.0f) rayAngle += 360.0f;
			else if (rayAngle > 360.0f)	rayAngle -= 360.0f;

			float rayAngleRadian = rayAngle * PI / 180;
			Vec2Float rayAngleDir;
			rayAngleDir.x = cos(rayAngleRadian);
			rayAngleDir.y = sin(rayAngleRadian);

			float curDist = 0.f;
			float xDelta = fabs(1 / rayAngleDir.x); // x ���� 1 �ٲ� �� ���� �Ÿ�
			float yDelta = fabs(1 / rayAngleDir.y); // y ���� 1 �ٲ� �� ���� �Ÿ�
			float xDist = xDelta * (ownerPlayer->pos.x - trunc(ownerPlayer->pos.x));	// ó�� x���� �ٲ� ���� �Ÿ�
			float yDist = yDelta * (ownerPlayer->pos.y - trunc(ownerPlayer->pos.y));	// ó�� y���� �ٲ� ���� �Ÿ�

			// DDA��ǥȮ�ο�
			int xPosDDA = ownerPlayer->position.x;
			int yPosDDA = ownerPlayer->position.y;

			while (curDist <= itemDist)
			{
				// yDist �� xDist ���� ª�� -> y = n ���� ������
				if (xDist > yDist)
				{
					// �������� �Ÿ� ydist ��
					curDist = yDist;

					// yDist �� ���� ����
					yDist += yDelta;

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

					if (rayAngleDir.x > 0) ++xPosDDA;
					else --xPosDDA;
				}
			}

			// ���� ��ġ���� ī�޶� ��� ������� �Ÿ��� ����
			float wallDepth = fabs(curDist * cos((ownerPlayer->angle - rayAngle) * PI / 180.f));
			//depthBuffer[i] = wallDepth;
			//�� �Ÿ��� ���� �׸��� �ݺ���

			for (int ix = -2; ix < 3; ++ix)
			{
				for (int j = 0; j < 3; ++j)
				{
					// ȭ�鿡�� ����� �ʰ� ����ó��
					if (((screenHeight / 2) + j >= screenHeight) || ((screenHeight / 2) - j < 0)) continue;
					if ((screenWidth - i - 1 + ix) < 0 || (screenWidth - i - 1 + ix) >= screenWidth) 
						continue;

					// ������ �Ÿ��� ���� �� Shading
					char shade = '^';

					if (wallDepth < depthBuffer[i])
					{
						screen[(int)((screenHeight / 2) + j)][screenWidth - i - 1 + ix] = { shade , Color::Cyan };
						screen[(int)((screenHeight / 2) - j)][screenWidth - i - 1 + ix] = { shade , Color::Cyan };
					}
					else
					{
						// Todo: Delta �� ���� ������ Ȯ �پ������ ���� ����� �ȵ�
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
