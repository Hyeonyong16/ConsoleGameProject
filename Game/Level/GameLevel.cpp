#include "GameLevel.h"

#include "Actor/Player.h"
#include "Actor/Wall.h"
#include "Actor/Camera.h"
#include "Actor/Item.h"
#include "Manager/UIManager.h"

#include "Engine.h"

#include <iostream>


GameLevel::GameLevel()
{
	// �÷��̾� �߰�.
	//AddActor(new Player());
	ReadMapFile("Map1.txt");
	AddActor(new UIManager(this));
}

GameLevel::~GameLevel()
{
	for (int i = 0; i < mapHeight; ++i)
	{
		if (nullptr != wallMap[i])
		{
			delete[] wallMap[i];
			wallMap[i] = nullptr;
		}
	}

	delete[] wallMap;
}

void GameLevel::BeginPlay()
{
	super::BeginPlay();

}

void GameLevel::Tick(float _deltaTime)
{
	super::Tick(_deltaTime);

	if (Input::Get().GetKeyDown('T')) { isFPS = !isFPS; }
}

void GameLevel::Render()
{
	super::Render();

	if(!isFPS)
	{
		char buffer[20] = { };
		sprintf_s(buffer, 20, "Score: %d", score);
		Engine::Get().WriteToBuffer(Vector2(Engine::Get().GetWidth() - 35, 7), buffer);
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
		case 'i':	// Todo: item �ӽ� ó��
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
			Player* player = new Player(position);
			AddActor(player);
			this->player = player;

			Camera* camera = new Camera(player);
			AddActor(camera);
			this->camera = camera;
			player->SetCamera(camera);
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
