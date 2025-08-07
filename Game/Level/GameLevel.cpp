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

	// Todo: Test 용 다음 레벨 가는 단축키
	if (Input::Get().GetKeyDown('P')) 
	{	Game::Get().GoNextLevel(); }

	// 몬스터가 다 죽으면 포탈 만들기
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
	// 최종 에셋 경로 완성
	char filePath[256] = { };
	sprintf_s(filePath, 256, "../Assets/%s", _fileName);

	FILE* file = nullptr;
	fopen_s(&file, filePath, "rt");

	// 예외처리
	if (nullptr == file)
	{
		std::cout << "맵 파일 읽기 실패." << _fileName << "\n";
		__debugbreak();
		return;
	}

	int offset = 0;
	// 맵 지도를 그리기 위해서 width, Height 정보 불러옴
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

	// 파싱(Parcing, 해석)
	fseek(file, 0, SEEK_END);
	size_t fileSize = ftell(file) - offset;
	fseek(file, offset, SEEK_SET);

	// 확인할 파일 크기를 활용해 버퍼 할당
	char* buffer = new char[fileSize + 1];
	memset(buffer, 0, fileSize + 1);
	size_t readSize = fread(buffer, sizeof(char), fileSize, file);

	// 배열 순회를 위한 인덱스 변수
	int index = 0;

	// 문자열 길이 값
	int size = (int)readSize;

	// x, y 좌표
	Vector2 position;

	//문자 배열 순회
	while (index < size)
	{
		// 맵 문자 확인
		char mapCharacter = buffer[index++];

		// 개행 문자 처리
		if (mapCharacter == '\n')
		{
			// 다음 줄로 넘기면서, x 좌표 초기화
			++position.y;
			position.x = 0;

			continue;
		}

		//각 문자 처리
		switch (mapCharacter)
		{
		case '#':	// 벽
		{
			Wall* wall = new Wall(position);
			AddActor(wall);
			wallMap[position.y][position.x] = wall->GetID();
			break;
		}
		case 'i':	// item 임시 처리
		case 'I':
		{
			Item* item = new Item(position);
			AddActor(item);
			itemIDs.emplace_back(item->GetID());
			break;
		}
		case 'p':	// 플레이어 시작 위치
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
		case 'M':	// 몬스터 생성
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

		// x 좌표 증가 처리
		++position.x;
	}

	// 버퍼 해제
	delete[] buffer;

	// 파일 닫기
	fclose(file);
}
