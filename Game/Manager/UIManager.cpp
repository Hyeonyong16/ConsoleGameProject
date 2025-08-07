#include "UIManager.h"
#include "Engine.h"
#include "Level/GameLevel.h"
#include "Actor/Player.h"
#include "Actor/Camera.h"

#include "Game/Game.h"

#include <iostream>

UIManager::UIManager(GameLevel* _gameLevel)
	: Actor(), gameLevel(_gameLevel)
{
	gameCamera = gameLevel->GetCamera();
	AssetLoading("CharacterFace.txt");
	AssetLoading("GunImage.txt");
	AssetLoading("HandWithGunImage.txt");
	AssetLoading("Num1.txt");
	AssetLoading("Num2.txt");

	fireRenderingTimer.SetTargetTime(0.3f);
}

UIManager::~UIManager()
{
	// asset 들 할당 해제
	for (Asset* asset : assets)
	{
		SafeDelete(asset);
	}

	assets.clear();
}

void UIManager::BeginPlay()
{
	super::BeginPlay();
}

void UIManager::Tick(float _deltaTime)
{
	if (gameLevel->GetPlayer()->GetIsFire() && !isFireRendering)
	{
		isFireRendering = true;
	}

	if (isFireRendering)
	{
		fireRenderingTimer.Tick(_deltaTime);
		if (fireRenderingTimer.IsTimeout())
		{
			if(!gameLevel->GetPlayer()->GetIsFire())
			{
				isFireRendering = false;
				fireRenderingTimer.Reset();
				fireRenderingTimer.SetTargetTime(0.3f);
			}
		}
	}
}

void UIManager::Render()
{
	int consoleWidth = Engine::Get().GetWidth();
	int consoleHeight = Engine::Get().GetHeight();

	// 테두리
	{
		// 테두리 그리기 (가로선)
		for (int i = 0; i < consoleWidth; ++i)
		{
			Engine::Get().WriteToBuffer(Vector2(i, gameCamera->GetScreenHeight() + 1), 'A', Color::Magenta);
			Engine::Get().WriteToBuffer(Vector2(i, consoleHeight - 1), 'A', Color::Magenta);
			Engine::Get().WriteToBuffer(Vector2(i, 0), 'A', Color::Magenta);
		}
		// 테두리 그리기 (세로선)
		for (int i = 0; i < consoleHeight; ++i)
		{
			Engine::Get().WriteToBuffer(Vector2(0, i), 'A', Color::Magenta);
			Engine::Get().WriteToBuffer(Vector2(consoleWidth - 1, i), 'A', Color::Magenta);
		}
		// 테두리 그리기 (세로 구분선)
		for (int i = 0; i < consoleHeight - gameCamera->GetScreenHeight() - 2; ++i)
		{
			Engine::Get().WriteToBuffer(Vector2(36, gameCamera->GetScreenHeight() + 2 + i), 'A', Color::Magenta);
			Engine::Get().WriteToBuffer(Vector2(72, gameCamera->GetScreenHeight() + 2 + i), 'A', Color::Magenta);
			Engine::Get().WriteToBuffer(Vector2(108, gameCamera->GetScreenHeight() + 2 + i), 'A', Color::Magenta);
		}
	}

	// 층 (1 ~ 35)
	{
		int xPos = 1;
		int yPos = gameCamera->GetScreenHeight() + 2;
		Asset* characterAsset = nullptr;
		char buffer[10];
		sprintf_s(buffer, 10, "Num%d", Game::Get().GetCurStage());
		for (Asset* asset : assets)
		{
			if (strcmp(asset->GetKey(), buffer) == 0)
			{
				characterAsset = asset;
				break;
			}
		}

		if (nullptr != characterAsset)
		{
			int imageSize = characterAsset->GetHeight() * characterAsset->GetWidth();
			for (int i = 0; i < imageSize; ++i)
			{
				if (characterAsset->GetImage()[i] == '\n' || characterAsset->GetImage()[i] == '\0')
				{
					continue;
				}
				
				Engine::Get().WriteToBuffer(
					Vector2((i % (characterAsset->GetWidth())) + xPos + 10, (i / characterAsset->GetWidth()) + yPos + 3),
					characterAsset->GetImage()[i],
					Color::White
				);
			}
		}
	}

	// 점수 (37 ~ 71)
	{
		char buffer[30] = { };
		sprintf_s(buffer, 30, "Score: %d", gameLevel->GetScore());
		Engine::Get().WriteToBuffer(Vector2(38, gameCamera->GetScreenHeight() + 7), buffer);
		sprintf_s(buffer, 30, "LeftEnemy: %d", gameLevel->GetMonsterNum());
		Engine::Get().WriteToBuffer(Vector2(38, gameCamera->GetScreenHeight() + 10), buffer);

		sprintf_s(buffer, 30, "pos x: %.2f, y: %.2f",
			gameLevel->GetPlayer()->GetPosFloat().x,
			gameLevel->GetPlayer()->GetPosFloat().y
		);
		Engine::Get().WriteToBuffer(Vector2(38, gameCamera->GetScreenHeight() + 14), buffer);

		sprintf_s(buffer, 30, "angle: %.2f",
			gameLevel->GetPlayer()->GetAngle()
		);
		Engine::Get().WriteToBuffer(Vector2(38, gameCamera->GetScreenHeight() + 15), buffer);

		sprintf_s(buffer, 30, "dir x: %.2f, y: %.2f", 
			gameLevel->GetPlayer()->GetDir().x,
			gameLevel->GetPlayer()->GetDir().y
		);
		Engine::Get().WriteToBuffer(Vector2(38, gameCamera->GetScreenHeight() + 16), buffer);
	}

	// 사진 (73 ~ 107) 넓이 35칸 높이 20칸 (57 ~ 71)
	// 개행 문자 포함 20, 20
	{
		int xPos = 73; // 73 80
		int yPos = gameCamera->GetScreenHeight() + 2;
		Asset* characterAsset = nullptr;
		for (Asset* asset : assets)
		{
			if (strcmp(asset->GetKey(), "CharacterFace") == 0)
			{
				characterAsset = asset;
				break;
			}
		}

		if(nullptr != characterAsset)
		{
			int imageSize = characterAsset->GetHeight() * characterAsset->GetWidth();
			for (int i = 0; i < imageSize; ++i)
			{
				if (characterAsset->GetImage()[i] == '\n' || characterAsset->GetImage()[i] == '\0')
				{
					continue;
				}

				Engine::Get().WriteToBuffer(
					Vector2((i % (characterAsset->GetWidth())) + xPos, (i / characterAsset->GetWidth()) + yPos),
					characterAsset->GetImage()[i],
					Color::White
				);
			}
		}
	}
	// 총알 (109 ~ 180) 넓이 72칸 높이 20칸 (52 ~ 71)
	{
		int xPos = 109; // 73 80
		int yPos = gameCamera->GetScreenHeight() + 2;
		Asset* characterAsset = nullptr;
		for (Asset* asset : assets)
		{
			if (strcmp(asset->GetKey(), "GunImage") == 0)
			{
				characterAsset = asset;
				break;
			}
		}

		if (nullptr != characterAsset)
		{
			int imageSize = characterAsset->GetHeight() * characterAsset->GetWidth();
			for (int i = 0; i < imageSize; ++i)
			{
				if (characterAsset->GetImage()[i] == '\n' || characterAsset->GetImage()[i] == '\0')
				{
					continue;
				}

				Engine::Get().WriteToBuffer(
					Vector2((i % (characterAsset->GetWidth())) + xPos, (i / characterAsset->GetWidth()) + yPos),
					characterAsset->GetImage()[i],
					Color::White
				);
			}
		}

		char buffer[20] = { };
		sprintf_s(buffer, 20, "Bullet : %d", gameLevel->GetPlayer()->GetBullet());
		Engine::Get().WriteToBuffer(Vector2(150, gameCamera->GetScreenHeight() + 19), buffer);
	}

	// Todo: 총알 관련 렌더링 해줘야함
	{}

	// 사격 효과
	{
		if (isFireRendering && !fireRenderingTimer.IsTimeout())
		{
			Asset* characterAsset = nullptr;
			for (Asset* asset : assets)
			{
				if (strcmp(asset->GetKey(), "HandWithGunImage") == 0)
				{
					characterAsset = asset;
					break;
				}
			}

			if (nullptr != characterAsset)
			{
				int xPos = gameCamera->GetScreenWidth() / 2 + 1;
				int yPos = gameCamera->GetScreenHeight() + 1 - characterAsset->GetHeight();

				float tempElapsedTime = fireRenderingTimer.GetElapsedTime();
				float tempTargetTime = fireRenderingTimer.GetTargetTime();

				for (int i = -3 + (int)(tempTargetTime / tempElapsedTime);
					i < 4 - (int)(tempTargetTime / tempElapsedTime); ++i)
				{
					for (int j = -4 + (int)(tempTargetTime / tempElapsedTime);
						j < 5 - (int)(tempTargetTime / tempElapsedTime); ++j)
					{
						Engine::Get().WriteToBuffer(
							Vector2(xPos + j, yPos + i),
							"*",
							Color::Yellow
						);
					}
				}
			}
			
		}
	}

	// 손에 총든 이미지 렌더링
	{
		Asset* characterAsset = nullptr;
		for (Asset* asset : assets)
		{
			if (strcmp(asset->GetKey(), "HandWithGunImage") == 0)
			{
				characterAsset = asset;
				break;
			}
		}

		if (nullptr != characterAsset)
		{
			int xPos = gameCamera->GetScreenWidth() / 2 - (characterAsset->GetWidth() / 2) + 1;
			int yPos = gameCamera->GetScreenHeight() + 1 - characterAsset->GetHeight();

			int imageSize = characterAsset->GetHeight() * characterAsset->GetWidth();
			for (int i = 0; i < imageSize; ++i)
			{
				if (characterAsset->GetImage()[i] == '\n' || characterAsset->GetImage()[i] == '\0'
					|| characterAsset->GetImage()[i] == ':')
				{
					continue;
				}

				Color tempColor = Color::White;

				if (characterAsset->GetImage()[i] == 'o')
				{
					tempColor = Color::YellowIntensity;
				}

				Engine::Get().WriteToBuffer(
					Vector2((i % (characterAsset->GetWidth())) + xPos, (i / characterAsset->GetWidth()) + yPos),
					characterAsset->GetImage()[i],
					tempColor
				);
			}
		}

	}
}

// 파일 이름으로 에셋을 불러와서 저장해두는 함수
void UIManager::AssetLoading(const char* _fileName)
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
	int assetWidth = 0;
	int assetHeight = 0;
	char buffer[50] = { };

	// 에셋 저장을 위해서 key, width, Height 정보 불러옴
	fscanf_s(file, "key=%s\n", buffer, 50);
	fscanf_s(file, "width=%d\n", &assetWidth);
	fscanf_s(file, "height=%d\n", &assetHeight);
	offset = ftell(file);


	// 파싱(Parcing, 해석)
	fseek(file, 0, SEEK_END);
	size_t fileSize = ftell(file) - offset;
	fseek(file, offset, SEEK_SET);

	// 확인할 파일 크기를 활용해 버퍼 할당
	char* image = new char[fileSize + 1];
	memset(image, 0, fileSize + 1);
	size_t readSize = fread(image, sizeof(char), fileSize, file);

	// 배열 순회를 위한 인덱스 변수
	int index = 0;

	// 문자열 길이 값
	int size = (int)readSize;

	//문자 배열 순회
	while (index < size)
	{
		// 맵 문자 확인
		char mapCharacter = image[index++];
	}
	Asset* asset = new Asset(buffer, assetWidth, assetHeight, image);
	assets.emplace_back(asset);

	// 버퍼 해제
	delete[] image;

	// 파일 닫기
	fclose(file);
}
