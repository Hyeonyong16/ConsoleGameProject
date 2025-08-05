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
	// 플레이어 추가.
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
		case 'i':	// Todo: item 임시 처리
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

		// x 좌표 증가 처리
		++position.x;
	}

	// 버퍼 해제
	delete[] buffer;

	// 파일 닫기
	fclose(file);
}
