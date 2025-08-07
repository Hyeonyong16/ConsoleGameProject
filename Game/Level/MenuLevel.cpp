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

	// �޴� ������ �߰�.
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

	// ������ �� �̸� ����.
	length = static_cast<int>(items.size());
}

MenuLevel::~MenuLevel()
{
	for (MenuItem* item : items)
	{
		SafeDelete(item);
	}
	items.clear();

	// asset �� �Ҵ� ����
	for (Asset* asset : assets)
	{
		SafeDelete(asset);
	}
	assets.clear();
}

void MenuLevel::Tick(float deltaTime)
{
	super::Tick(deltaTime);

	// �Է� ó��.
	if (Input::Get().GetKeyDown(VK_UP))
	{
		currentIndex = (currentIndex - 1 + length) % length;
	}

	if (Input::Get().GetKeyDown(VK_DOWN))
	{
		currentIndex = (currentIndex + 1) % length;
	}

	// Enter Ű �Է�.
	if (Input::Get().GetKeyDown(VK_RETURN))
	{
		// �޴� �������� �����ϰ� �ִ� �Լ� ȣ��.
		items[currentIndex]->onSelected();
	}

	if (Input::Get().GetKeyDown(VK_ESCAPE))
	{
		Engine::Get().Quit();
	}

	// Todo : �׽�Ʈ�� ����
	if (Input::Get().GetKeyDown('O'))
	{
		Game::Get().ResetLevel();
	}
}

void MenuLevel::Render()
{
	super::Render();

	// �޴� ���� ���.
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

	// �޴� ������ ������.
	for (int ix = 0; ix < length; ++ix)
	{
		// ������ ���� Ȯ��.
		Color textColor =
			(ix == currentIndex) ? selectedColor : unselectedColor;

		// ���� ����.
		Utils::SetConsoleTextColor(static_cast<WORD>(textColor));

		// �޴� �ؽ�Ʈ ���.
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

// ���� �̸����� ������ �ҷ��ͼ� �����صδ� �Լ�
void MenuLevel::AssetLoading(const char* _fileName)
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
	int assetWidth = 0;
	int assetHeight = 0;
	char buffer[50] = { };

	// ���� ������ ���ؼ� key, width, Height ���� �ҷ���
	fscanf_s(file, "key=%s\n", buffer, 50);
	fscanf_s(file, "width=%d\n", &assetWidth);
	fscanf_s(file, "height=%d\n", &assetHeight);
	offset = ftell(file);


	// �Ľ�(Parcing, �ؼ�)
	fseek(file, 0, SEEK_END);
	size_t fileSize = ftell(file) - offset;
	fseek(file, offset, SEEK_SET);

	// Ȯ���� ���� ũ�⸦ Ȱ���� ���� �Ҵ�
	char* image = new char[fileSize + 1];
	memset(image, 0, fileSize + 1);
	size_t readSize = fread(image, sizeof(char), fileSize, file);

	// �迭 ��ȸ�� ���� �ε��� ����
	int index = 0;

	// ���ڿ� ���� ��
	int size = (int)readSize;

	//���� �迭 ��ȸ
	while (index < size)
	{
		// �� ���� Ȯ��
		char mapCharacter = image[index++];
	}
	Asset* asset = new Asset(buffer, assetWidth, assetHeight, image);
	assets.emplace_back(asset);

	// ���� ����
	delete[] image;

	// ���� �ݱ�
	fclose(file);
}