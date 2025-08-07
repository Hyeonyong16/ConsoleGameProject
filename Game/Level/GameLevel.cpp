#include "GameLevel.h"

#include "Actor/Player.h"
#include "Actor/Monster.h"
#include "Actor/Wall.h"
#include "Actor/Portal.h"
#include "Actor/Camera.h"
#include "Actor/Item.h"
#include "Manager/UIManager.h"

#include "Engine.h"
#include "Game/Game.h"

#include <iostream>


GameLevel::GameLevel()
{
	ReadMapFile("Map1.txt");
	AddActor(new UIManager(this));

	size_t length = strlen("Map1.txt");
	curMapFile = new char[length + 1];
	strcpy_s(curMapFile, length, "Map1.txt");
}

GameLevel::GameLevel(const char* _mapFile)
{
	ReadMapFile(_mapFile);
	AddActor(new UIManager(this));

	size_t length = strlen(_mapFile) + 1;
	curMapFile = new char[length];
	strcpy_s(curMapFile, length, _mapFile);
}

GameLevel::~GameLevel()
{
	for (int i = 0; i < mapHeight; ++i)
	{
		SafeDelete(wallMap[i]);
	}
	SafeDeleteArray(wallMap);

	SafeDeleteArray(curMapFile);
}

void GameLevel::BeginPlay()
{
	super::BeginPlay();
}

void GameLevel::Tick(float _deltaTime)
{
	super::Tick(_deltaTime);

	if (Input::Get().GetKeyDown('T')) { isFPS = !isFPS; }

	// Todo: Test �� ���� ���� ���� ����Ű
	if (Input::Get().GetKeyDown('P')) 
	{	Game::Get().GoNextLevel(); }

	// ���Ͱ� �� ������ ��Ż �����
	if (monsterNum == 0)
	{
		Portal* portal = FindActorByID(portalId)->As<Portal>();
		portal->SetIsActive(true);
	}
}

void GameLevel::Render()
{
	super::Render();

	if(!isFPS)
	{
		char buffer[20] = { };
		sprintf_s(buffer, 20, "Score: %d", score);
		Engine::Get().WriteToBuffer(Vector2(Engine::Get().GetWidth() - 35, 7), buffer);
		sprintf_s(buffer, 20, "Monster: %d", monsterNum);
		Engine::Get().WriteToBuffer(Vector2(Engine::Get().GetWidth() - 35, 8), buffer);
	}
}

void GameLevel::ReadMapFile(const char* _fileName)
{
	// ���� ���� ��� �ϼ�
	char filePath[256] = { };
	sprintf_s(filePath, 256, "../Assets/%s", _fileName);

	FILE* file = nullptr;
	fopen_s(&file, filePath, "rt");

	// ����ó��
	if (nullptr == file)
	{
		std::cout << "�� ���� �б� ����." << _fileName << "\n";
		__debugbreak();
		return;
	}

	int offset = 0;
	// �� ������ �׸��� ���ؼ� width, Height ���� �ҷ���
	fscanf_s(file, "width=%d\n", &mapWidth);
	fscanf_s(file, "height=%d\n", &mapHeight);
	offset = ftell(file);

	wallMap = new int*[mapHeight];
	for (int i = 0; i < mapHeight; ++i)
	{
		wallMap[i] = new int[mapWidth];
	}
	for (int i = 0; i < mapHeight; ++i)
	{
		for (int j = 0; j < mapWidth; ++j)
		{
			wallMap[i][j] = {-1};
		}
	}

	// �Ľ�(Parcing, �ؼ�)
	fseek(file, 0, SEEK_END);
	size_t fileSize = ftell(file) - offset;
	fseek(file, offset, SEEK_SET);

	// Ȯ���� ���� ũ�⸦ Ȱ���� ���� �Ҵ�
	char* buffer = new char[fileSize + 1];
	memset(buffer, 0, fileSize + 1);
	size_t readSize = fread(buffer, sizeof(char), fileSize, file);

	// �迭 ��ȸ�� ���� �ε��� ����
	int index = 0;

	// ���ڿ� ���� ��
	int size = (int)readSize;

	// x, y ��ǥ
	Vector2 position;

	//���� �迭 ��ȸ
	while (index < size)
	{
		// �� ���� Ȯ��
		char mapCharacter = buffer[index++];

		// ���� ���� ó��
		if (mapCharacter == '\n')
		{
			// ���� �ٷ� �ѱ�鼭, x ��ǥ �ʱ�ȭ
			++position.y;
			position.x = 0;

			continue;
		}

		//�� ���� ó��
		switch (mapCharacter)
		{
		case '#':	// ��
		{
			Wall* wall = new Wall(position);
			AddActor(wall);
			wallMap[position.y][position.x] = wall->GetID();
			break;
		}
		case 'i':	// item �ӽ� ó��
		case 'I':
		{
			Item* item = new Item(position);
			AddActor(item);
			itemIDs.emplace_back(item->GetID());
			break;
		}
		case 'p':	// �÷��̾� ���� ��ġ
		case 'P':
		{
			if(nullptr == this->player)
			{
				Player* player = new Player(position);
				AddActor(player);
				this->player = player;

				Camera* camera = new Camera(player);
				AddActor(camera);
				this->camera = camera;
				player->SetCamera(camera);
			}
			else
			{
				player->SetPosition(position);
				player->ResyncPos();
			}
			break;
		}
		case 'M':	// ���� ����
		case 'm':
		{
			Monster* monster = new Monster(position);
			AddActor(monster);
			monsterIDs.emplace_back(monster->GetID());

			++monsterNum;
			break;
		}

		case 'N':
		case 'n':
		{
			Portal* portal = new Portal(position);
			AddActor(portal);
			portalId = portal->GetID();
			break;
		}
		}

		// x ��ǥ ���� ó��
		++position.x;
	}

	// ���� ����
	delete[] buffer;

	// ���� �ݱ�
	fclose(file);
}
