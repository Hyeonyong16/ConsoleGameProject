#include "MenuLevel.h"
#include "Game/Game.h"
#include "Utils/Utils.h"
#include "Input.h"
#include "Asset/Asset.h"

#include <iostream>

MenuLevel::MenuLevel()
{
	AssetLoading("TitleImage.txt");
	AssetLoading("StartMenuImage.txt");
	AssetLoading("ResetMenuImage.txt");
	AssetLoading("ExitMenuImage.txt");

	Asset* temp = nullptr;
	for (Asset* asset : assets)
	{
		if (strcmp(asset->GetKey(), "StartMenuImage") == 0)
		{
			temp = asset;
			break;
		}
	}

	// 메뉴 아이템 추가.
	items.emplace_back(new MenuItem(
		temp,
		[]() { 
			char buffer[30] = {};
			sprintf_s(buffer, 30, "Map%d.txt", Game::Get().GetCurStage());
			Game::Get().GoLevel(buffer); 
		}
	));

	for (Asset* asset : assets)
	{
		if (strcmp(asset->GetKey(), "ResetMenuImage") == 0)
		{
			temp = asset;
			break;
		}
	}

	items.emplace_back(new MenuItem(
		temp,
		[]() { Game::Get().ResetLevel(); }
	));

	for (Asset* asset : assets)
	{
		if (strcmp(asset->GetKey(), "ExitMenuImage") == 0)
		{
			temp = asset;
			break;
		}
	}

	items.emplace_back(new MenuItem(
		temp,
		[]() { Game::Get().Quit(); }
	));

	// 아이템 수 미리 저장.
	length = static_cast<int>(items.size());
}

MenuLevel::~MenuLevel()
{
	for (MenuItem* item : items)
	{
		SafeDelete(item);
	}
	items.clear();

	// asset 들 할당 해제
	for (Asset* asset : assets)
	{
		SafeDelete(asset);
	}
	assets.clear();
}

void MenuLevel::Tick(float deltaTime)
{
	super::Tick(deltaTime);

	// 입력 처리.
	if (Input::Get().GetKeyDown(VK_UP))
	{
		currentIndex = (currentIndex - 1 + length) % length;
	}

	if (Input::Get().GetKeyDown(VK_DOWN))
	{
		currentIndex = (currentIndex + 1) % length;
	}

	// Enter 키 입력.
	if (Input::Get().GetKeyDown(VK_RETURN))
	{
		// 메뉴 아이템이 저장하고 있는 함수 호출.
		items[currentIndex]->onSelected();
	}

	if (Input::Get().GetKeyDown(VK_ESCAPE))
	{
		Engine::Get().Quit();
	}

	// Todo : 테스트용 리셋
	if (Input::Get().GetKeyDown('O'))
	{
		Game::Get().ResetLevel();
	}
}

void MenuLevel::Render()
{
	super::Render();

	// 메뉴 제목 출력.
	{
		Asset* titleAsset = nullptr;
		for (Asset* asset : assets)
		{
			if (strcmp(asset->GetKey(), "TitleImage") == 0)
			{
				titleAsset = asset;
				break;
			}
		}

		if (nullptr != titleAsset)
		{
			int xPos = (Engine::Get().GetWidth() / 2) - (titleAsset->GetWidth() / 2);
			int yPos = 3;

			int imageSize = titleAsset->GetHeight() * titleAsset->GetWidth();
			for (int i = 0; i < imageSize; ++i)
			{
				if (titleAsset->GetImage()[i] == '\n' || titleAsset->GetImage()[i] == '\0')
				{
					continue;
				}

				Engine::Get().WriteToBuffer(
					Vector2((i % (titleAsset->GetWidth())) + xPos, (i / titleAsset->GetWidth()) + yPos),
					titleAsset->GetImage()[i],
					Color::White
				);
			}
		}
	}

	// 메뉴 아이템 렌더링.
	for (int ix = 0; ix < length; ++ix)
	{
		// 아이템 색상 확인.
		Color textColor =
			(ix == currentIndex) ? selectedColor : unselectedColor;

		// 색상 설정.
		Utils::SetConsoleTextColor(static_cast<WORD>(textColor));

		// 메뉴 텍스트 출력.
		if (nullptr != items[ix]->menuText)
		{
			int xPos = (Engine::Get().GetWidth() / 2) - (items[ix]->menuText->GetWidth() / 2);
			int yPos = 30 + 15 * ix;

			int imageSize = items[ix]->menuText->GetHeight() * items[ix]->menuText->GetWidth();
			for (int i = 0; i < imageSize; ++i)
			{
				if (items[ix]->menuText->GetImage()[i] == '\n' || items[ix]->menuText->GetImage()[i] == '\0')
				{
					continue;
				}

				Engine::Get().WriteToBuffer(
					Vector2(
						(i % (items[ix]->menuText->GetWidth())) + xPos,
						(i / items[ix]->menuText->GetWidth()) + yPos
					),
					items[ix]->menuText->GetImage()[i],
					textColor
				);
			}
		}
	}
}

// 파일 이름으로 에셋을 불러와서 저장해두는 함수
void MenuLevel::AssetLoading(const char* _fileName)
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