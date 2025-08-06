#include "Camera.h"
#include "Level/GameLevel.h"
#include "Player.h"
#include "Actor/Wall.h"
#include "Actor/Item.h"
#include "Actor/Monster.h"
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
						screen[(int)((screenHeight / 2) + j)][screenWidth - i - 1 + ix]= { 
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

		float tempDist = 0.f;
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
				&& owner->FindActorByID(ownerPlayer->wallMap[yPosDDA][xPosDDA])->As<Wall>()
				&& curDist > 0.1f)
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

	// �������� ����ִ� ���͸� �޾Ƽ� �ݺ��� ������
	/*for (int id : gameLevel->GetItemIDs())
	{
		// �������� ��ġ (�簢�� ���� ���� �������� 0.5 ������
		Vector2 itemPos = owner->FindActorByID(id)->GetPosition();
		itemPos.x += 0.5f;
		itemPos.y += 0.5f;

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
			float rayAngle = (ownerPlayer->angle - (fov / 2)) + ((float)i / (float)screenWidth) * fov;
			// �������� ����̸� ������
			if (cross > 0)
			{
				// ���� ray�� ������ ������ ���� ������ �� ������ (float ������ ��Ȯ���� ������ Ŀ���� �ٷ� Ż��)
				while(rayAngle < (ownerPlayer->angle - itemPlayerDegree))
				{
					++i;
					// ray ���� �� ����
					rayAngle = (ownerPlayer->angle - (fov / 2)) + ((float)i / (float)screenWidth) * fov;
				}
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
			float wallDepth = fabs(itemDist * cos((ownerPlayer->angle - rayAngle) * PI / 180.f));
			//�� �Ÿ��� ���� �׸��� �ݺ���

			for (int ix = -((screenHeight / itemScale / wallDepth) / 2.f);
				ix < ((screenHeight / itemScale) / wallDepth) / 2.f; ++ix)
			{
				bool isDraw = false;
				for (int j = 0;
					j < ((screenHeight / itemScale) / wallDepth) / 2; ++j)
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
						screen[(int)((screenHeight / 2) + j)][screenWidth - i - 1 + ix] = { shade , Color::Cyan };
						screen[(int)((screenHeight / 2) - j)][screenWidth - i - 1 + ix] = { shade , Color::Cyan };
					}
				}
				if (isDraw) depthBuffer[i - ix] = wallDepth;
			}
		}
	}*/
	// ���Ͱ� ����ִ� ���͸� �޾Ƽ� �ݺ��� ������
	/*for (int id : gameLevel->GetMonsterIDs())
	{
		// �������� ��ġ (�簢�� ���� ���� �������� 0.5 ������
		Vector2 monsterPos = owner->FindActorByID(id)->GetPosition();
		monsterPos.x += 0.5f;
		monsterPos.y += 0.5f;

		// �����۰� �÷��̾� ���� �Ÿ� ���ϱ�
		float monsterDist = sqrtf(((float)monsterPos.x - playerPos.x) * ((float)monsterPos.x - playerPos.x)
			+ ((float)monsterPos.y - playerPos.y) * ((float)monsterPos.y - playerPos.y));

		// ������ ��ġ�� dist ���� �ָ� pass
		if (monsterDist > dist) continue;

		// �������� �� ���� ���̰� ���ϱ�
		// ownerPlayer->dir; // ���� ���⺤��
		Vec2Float monsterDir;
		monsterDir.x = (float)monsterPos.x - playerPos.x;
		monsterDir.y = (float)monsterPos.y - playerPos.y;

		// �����۹���� �÷��̾���� ���� ��(����)
		float monsterPlayerRadian = acosf(((monsterDir.x * ownerPlayer->dir.x) + (monsterDir.y * ownerPlayer->dir.y))
			/ sqrtf((monsterDir.x * monsterDir.x) + (monsterDir.y * monsterDir.y)));
		float monsterPlayerDegree = monsterPlayerRadian * 180 / PI;

		// �÷��̾� ���� ���� ���� ������ ��ġ�� ��, �� �Ǻ��� ���� ������
		float cross = (ownerPlayer->dir.x * monsterDir.y) - (ownerPlayer->dir.y * monsterDir.x);

		// �÷��̾�� ������ ���� ���� ���� ���밪�� �þ߰� �����϶� ���� 
		if (fabsf(monsterPlayerDegree) < (45.f))
		{
			int i = 0;
			float rayAngle = (ownerPlayer->angle - (fov / 2)) + ((float)i / (float)screenWidth) * fov;
			// �������� ����̸� ������
			if (cross > 0)
			{
				// ���� ray�� ������ ������ ���� ������ �� ������ (float ������ ��Ȯ���� ������ Ŀ���� �ٷ� Ż��)
				while (rayAngle < (ownerPlayer->angle - monsterPlayerDegree))
				{
					++i;
					// ray ���� �� ����
					rayAngle = (ownerPlayer->angle - (fov / 2)) + ((float)i / (float)screenWidth) * fov;
				}
			}
			// �������� �����̸� ����
			else
			{
				// ���� ray�� ������ ������ ���� ������ �� ������ (float ������ ��Ȯ���� ������ Ŀ���� �ٷ� Ż��)
				while (rayAngle < (ownerPlayer->angle + monsterPlayerDegree))
				{
					++i;
					// ray ���� �� ����
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

			while (curDist <= monsterDist)
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
			float wallDepth = fabs(monsterDist * cos((ownerPlayer->angle - rayAngle) * PI / 180.f));
			//�� �Ÿ��� ���� �׸��� �ݺ���

			for (int ix = -((screenHeight / monsterScale / wallDepth) / 2.f);
				ix < ((screenHeight / monsterScale) / wallDepth) / 2.f; ++ix)
			{
				// ���� ���� üũ�� ���η� �׸��� �ʾƼ� ���� �� üũ�� �÷���
				bool isDraw = false;
				for (int j = 0;
					j < ((screenHeight / monsterScale) / wallDepth) / 2; ++j)
				{
					// ȭ�鿡�� ����� �ʰ� ����ó��
					if (((screenHeight / 2) + j >= screenHeight)
						|| ((screenHeight / 2) - j < 0)) continue;
					if ((screenWidth - i - 1 + ix) < 0
						|| (screenWidth - i - 1 + ix) >= screenWidth) continue;

					// ���� ���� Char
					char shade = 'O';

					if (wallDepth < depthBuffer[i - ix])
					{
						isDraw = true;
						screen[(int)((screenHeight / 2) + j)][screenWidth - i - 1 + ix] = { shade , Color::Red };
						screen[(int)((screenHeight / 2) - j)][screenWidth - i - 1 + ix] = { shade , Color::Red };
					}
				}
				if(isDraw) depthBuffer[i - ix] = wallDepth;
			}
		}
	}*/
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
