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

// ������ �� �Ÿ��� ���� ���� ������ ���� static ���� ����
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
	// screen ���� ���̸� ���� X
	if (_pos.x < 0 || _pos.x > screenWidth - 1) return;
	if (_pos.y < 0 || _pos.y > screenHeight - 1) return;

	// ���� ���̸� �ش� ��ġ�� ����
	screen[_pos.y][_pos.x].text = _c;
}

// DDA ����� Ȱ���� Actor�� �׸��� �Լ�
void Camera::DrawActorByDDA(std::vector<int> _actorIDs, float _renderScale)
{
	GameLevel* gameLevel = owner->As<GameLevel>();
	Vec2Float playerPos = ownerPlayer->pos;

	// �������� ����ִ� ���͸� �޾Ƽ� �ݺ��� ������
	for (int id : _actorIDs)
	{
		// �������� ��ġ (�簢�� ���� ���� �������� 0.5 ������
		Vec2Float itemPos;
		itemPos.x = (float)owner->FindActorByID(id)->GetPosition().x + 0.5f;
		itemPos.y = (float)owner->FindActorByID(id)->GetPosition().y + 0.5f;

		// �����۰� �÷��̾� ���� �Ÿ� ���ϱ�
		float itemDist = sqrtf(((float)itemPos.x - playerPos.x) * ((float)itemPos.x - playerPos.x)
			+ ((float)itemPos.y - playerPos.y) * ((float)itemPos.y - playerPos.y));

		// ������ ��ġ�� dist ���� �ָ� pass
		if (itemDist > dist) continue;

		// �������� �� ���� ���̰� ���ϱ�
		// ownerPlayer->dir; // ���� ���⺤��
		Vec2Float itemDir;
		itemDir.x = (float)itemPos.x - playerPos.x;
		itemDir.y = (float)itemPos.y - playerPos.y;

		// �����۹���� �÷��̾���� ���� ��(����)
		float itemPlayerRadian = acosf(((itemDir.x * ownerPlayer->dir.x) + (itemDir.y * ownerPlayer->dir.y))
			/ sqrtf((itemDir.x * itemDir.x) + (itemDir.y * itemDir.y)));
		float itemPlayerDegree = itemPlayerRadian * 180 / PI;

		// �÷��̾� ���� ���� ���� ������ ��ġ�� ��, �� �Ǻ��� ���� ������
		float cross = (ownerPlayer->dir.x * itemDir.y) - (ownerPlayer->dir.y * itemDir.x);

		// �÷��̾�� ������ ���� ���� ���� ���밪�� �þ߰� �����϶� ���� 
		if (fabsf(itemPlayerDegree) < (45.f))
		{
			int i = 0;
			int maxDraw = 0;

			float rayAngle = (ownerPlayer->angle - (fov / 2)) + ((float)i / (float)screenWidth) * fov;
			// �������� ����̸� ������
			if (cross > 0)
			{
				// ���� ray�� ������ ������ ���� ������ �� ������ (float ������ ��Ȯ���� ������ Ŀ���� �ٷ� Ż��)
				while (rayAngle < (ownerPlayer->angle - itemPlayerDegree))
				{
					++i;
					// ray ���� �� ����
					rayAngle = (ownerPlayer->angle - (fov / 2)) + ((float)i / (float)screenWidth) * fov;
				}

				float maxDrawAngleDegree = asinf(0.5f / itemDist) * 180 / PI;
				while ((((float)maxDraw / (float)screenWidth) * fov) < maxDrawAngleDegree)
				{
					++maxDraw;
				}

				int a = 0;
			}
			// �������� �����̸� ����
			else
			{
				// ���� ray�� ������ ������ ���� ������ �� ������ (float ������ ��Ȯ���� ������ Ŀ���� �ٷ� Ż��)
				while (rayAngle < (ownerPlayer->angle + itemPlayerDegree))
				{
					++i;
					// ray ���� �� ����
					rayAngle = (ownerPlayer->angle - (fov / 2)) + ((float)i / (float)screenWidth) * fov;
				}

				float maxDrawAngleDegree = asinf(0.5f / itemDist) * 180 / PI;
				while ((((float)maxDraw / (float)screenWidth) * fov) < maxDrawAngleDegree)
				{
					++maxDraw;
				}

				int a = 0;
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
			float xDelta = rayAngleDir.x == 0.f ? 0.f : fabs(1 / rayAngleDir.x); // x ���� 1 �ٲ� �� ���� �Ÿ�
			float yDelta = rayAngleDir.y == 0.f ? 0.f : fabs(1 / rayAngleDir.y); // y ���� 1 �ٲ� �� ���� �Ÿ�
			//float xDist = xDelta * (ownerPlayer->pos.x - trunc(ownerPlayer->pos.x));	// ó�� x���� �ٲ� ���� �Ÿ�
			//float yDist = yDelta * (ownerPlayer->pos.y - trunc(ownerPlayer->pos.y));	// ó�� y���� �ٲ� ���� �Ÿ�

			// ���⿡ ���� �Ÿ��� �޶���
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

			// DDA��ǥȮ�ο�
			int xPosDDA = ownerPlayer->position.x;
			int yPosDDA = ownerPlayer->position.y;

			while (curDist <= itemDist)
			{

				if (xDelta == 0) xDist = 9999999;
				else if (yDelta == 0) yDist = 9999999;
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
			float wallDepth = fabs(itemDist * cos((ownerPlayer->angle - rayAngle) * PI / 180.f));
			//�� �Ÿ��� ���� �׸��� �ݺ���

			for (int ix = -(maxDraw / 2);
				ix < (maxDraw / 2); ++ix)
			{
				bool isDraw = false;
				for (int j = 0;
					j < ((screenHeight / _renderScale) / wallDepth) / 2; ++j)
				{
					// ȭ�鿡�� ����� �ʰ� ����ó��
					if (((screenHeight / 2) + j >= screenHeight)
						|| ((screenHeight / 2) - j < 0)) continue;
					if ((screenWidth - i - 1 + ix) < 0
						|| (screenWidth - i - 1 + ix) >= screenWidth) continue;

					// ������ �Ÿ��� ���� �� Shading
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

					// ���� ���� ó��
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

	// �������� ��ġ (�簢�� ���� ���� �������� 0.5 ������
	Vec2Float itemPos;
	itemPos.x = (float)owner->FindActorByID(id)->GetPosition().x + 0.5f;
	itemPos.y = (float)owner->FindActorByID(id)->GetPosition().y + 0.5f;

	// �����۰� �÷��̾� ���� �Ÿ� ���ϱ�
	float itemDist = sqrtf(((float)itemPos.x - playerPos.x) * ((float)itemPos.x - playerPos.x)
		+ ((float)itemPos.y - playerPos.y) * ((float)itemPos.y - playerPos.y));

	// ������ ��ġ�� dist ���� �ָ� pass
	if (itemDist > dist) return;

	// �������� �� ���� ���̰� ���ϱ�
	// ownerPlayer->dir; // ���� ���⺤��
	Vec2Float itemDir;
	itemDir.x = (float)itemPos.x - playerPos.x;
	itemDir.y = (float)itemPos.y - playerPos.y;

	// �����۹���� �÷��̾���� ���� ��(����)
	float itemPlayerRadian = acosf(((itemDir.x * ownerPlayer->dir.x) + (itemDir.y * ownerPlayer->dir.y))
		/ sqrtf((itemDir.x * itemDir.x) + (itemDir.y * itemDir.y)));
	float itemPlayerDegree = itemPlayerRadian * 180 / PI;

	// �÷��̾� ���� ���� ���� ������ ��ġ�� ��, �� �Ǻ��� ���� ������
	float cross = (ownerPlayer->dir.x * itemDir.y) - (ownerPlayer->dir.y * itemDir.x);

	// �÷��̾�� ������ ���� ���� ���� ���밪�� �þ߰� �����϶� ���� 
	if (fabsf(itemPlayerDegree) < (45.f))
	{
		int i = 0;
		int maxDraw = 0;

		float rayAngle = (ownerPlayer->angle - (fov / 2)) + ((float)i / (float)screenWidth) * fov;
		// �������� ����̸� ������
		if (cross > 0)
		{
			// ���� ray�� ������ ������ ���� ������ �� ������ (float ������ ��Ȯ���� ������ Ŀ���� �ٷ� Ż��)
			while (rayAngle < (ownerPlayer->angle - itemPlayerDegree))
			{
				++i;
				// ray ���� �� ����
				rayAngle = (ownerPlayer->angle - (fov / 2)) + ((float)i / (float)screenWidth) * fov;
			}

			float maxDrawAngleDegree = asinf(0.5f / itemDist) * 180 / PI;
			while ((((float)maxDraw / (float)screenWidth) * fov) < maxDrawAngleDegree)
			{
				++maxDraw;
			}

			int a = 0;
		}
		// �������� �����̸� ����
		else
		{
			// ���� ray�� ������ ������ ���� ������ �� ������ (float ������ ��Ȯ���� ������ Ŀ���� �ٷ� Ż��)
			while (rayAngle < (ownerPlayer->angle + itemPlayerDegree))
			{
				++i;
				// ray ���� �� ����
				rayAngle = (ownerPlayer->angle - (fov / 2)) + ((float)i / (float)screenWidth) * fov;
			}

			float maxDrawAngleDegree = asinf(0.5f / itemDist) * 180 / PI;
			while ((((float)maxDraw / (float)screenWidth) * fov) < maxDrawAngleDegree)
			{
				++maxDraw;
			}

			int a = 0;
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
		float xDelta = rayAngleDir.x == 0.f ? 0.f : fabs(1 / rayAngleDir.x); // x ���� 1 �ٲ� �� ���� �Ÿ�
		float yDelta = rayAngleDir.y == 0.f ? 0.f : fabs(1 / rayAngleDir.y); // y ���� 1 �ٲ� �� ���� �Ÿ�
		//float xDist = xDelta * (ownerPlayer->pos.x - trunc(ownerPlayer->pos.x));	// ó�� x���� �ٲ� ���� �Ÿ�
		//float yDist = yDelta * (ownerPlayer->pos.y - trunc(ownerPlayer->pos.y));	// ó�� y���� �ٲ� ���� �Ÿ�

		// ���⿡ ���� �Ÿ��� �޶���
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

		// DDA��ǥȮ�ο�
		int xPosDDA = ownerPlayer->position.x;
		int yPosDDA = ownerPlayer->position.y;

		while (curDist <= itemDist)
		{
			if (xDelta == 0) xDist = 9999999;
			else if (yDelta == 0) yDist = 9999999;
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
		float wallDepth = fabs(itemDist * cos((ownerPlayer->angle - rayAngle) * PI / 180.f));
		//�� �Ÿ��� ���� �׸��� �ݺ���

		for (int ix = -(maxDraw / 2);
			ix < (maxDraw / 2); ++ix)
		{
			bool isDraw = false;
			for (int j = 0;
				j < ((screenHeight / _renderScale) / wallDepth) / 2; ++j)
			{
				// ȭ�鿡�� ����� �ʰ� ����ó��
				if (((screenHeight / 2) + j >= screenHeight)
					|| ((screenHeight / 2) - j < 0)) continue;
				if ((screenWidth - i - 1 + ix) < 0
					|| (screenWidth - i - 1 + ix) >= screenWidth) continue;

				// ������ �Ÿ��� ���� �� Shading
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

				// ���� ���� ó��
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

		// ������ ���� �� ��ġ�� ����(0 ~ 360)
		if (rayAngle < 0.0f) rayAngle += 360.0f;
		else if (rayAngle > 360.0f)	rayAngle -= 360.0f;

		float rayAngleRadian = rayAngle * PI / 180;
		Vec2Float rayAngleDir;
		rayAngleDir.x = cos(rayAngleRadian);
		rayAngleDir.y = sin(rayAngleRadian);

		float curDist = 0.f;
		float xDelta = rayAngleDir.x == 0.f ? 0.f : fabs(1 / rayAngleDir.x); // x ���� 1 �ٲ� �� ���� �Ÿ�
		float yDelta = rayAngleDir.y == 0.f ? 0.f : fabs(1 / rayAngleDir.y); // y ���� 1 �ٲ� �� ���� �Ÿ�
		//float xDist = xDelta * (1 - (ownerPlayer->pos.x - trunc(ownerPlayer->pos.x)));	// ó�� x���� �ٲ� ���� �Ÿ�
		//float yDist = yDelta * (1 - (ownerPlayer->pos.y - trunc(ownerPlayer->pos.y)));	// ó�� y���� �ٲ� ���� �Ÿ�

		// ���⿡ ���� �Ÿ��� �޶���
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

		// DDA��ǥȮ�ο�
		int xPosDDA = ownerPlayer->pos.x;
		int yPosDDA = ownerPlayer->pos.y;

		// �� ī�޶� �������� ������ Ray �� �ִ� ����
		float maxDist = fabs(dist * cos((ownerPlayer->angle - rayAngle) * PI / 180));

		float tempDist = 0.f;
		while (curDist <= maxDist)
		{
			if (xDelta == 0) xDist = 9999999;
			else if (yDelta == 0) yDist = 9999999;
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
				//&& curDist > 0.1f)
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

	// Item �׸��� Ȯ�ο�
	// 1. �÷��̾� ���� ���Ϳ� �÷��̾�� ������ ���� ���� ���̰� ���ϱ�
	// 2. ������ ������ �ֻܼ󿡼� ��ġ ���ϱ�
	// 3. �ش� ��ġ���� ���� ũ�� ��ŭ ���
	// 4. ���� ���� ����ؼ� �׸� �� �ִ� ��ŭ �� �׸���
	GameLevel* gameLevel = owner->As<GameLevel>();
	Vec2Float playerPos = ownerPlayer->pos;

	// Actor �׸���
	DrawActorByDDA(gameLevel->GetItemIDs(), itemScale);
	DrawActorByDDA(gameLevel->GetMonsterIDs(), monsterScale);

	// Portal �� Ȱ��ȭ �Ǿ������� �׸���
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
