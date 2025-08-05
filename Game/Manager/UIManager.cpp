#include "UIManager.h"
#include "Engine.h"
#include "Level/GameLevel.h"
#include "Actor/Camera.h"

#include <iostream>

UIManager::UIManager(GameLevel* _gameLevel)
	: Actor(), gameLevel(_gameLevel)
{
	gameCamera = gameLevel->GetCamera();
	AssetLoading("CharacterFace.txt");
	AssetLoading("GunImage.txt");
	AssetLoading("Num1.txt");
}

UIManager::~UIManager()
{
	// asset �� �Ҵ� ����
	for (Asset* asset : assets)
	{
		SafeDelete(asset);
	}

	assets.clear();
}

void UIManager::Tick(float _deltaTime)
{;
}

void UIManager::Render()
{
	int consoleWidth = Engine::Get().GetWidth();
	int consoleHeight = Engine::Get().GetHeight();

	// �׵θ�
	{
		// �׵θ� �׸��� (���μ�)
		for (int i = 0; i < consoleWidth; ++i)
		{
			Engine::Get().WriteToBuffer(Vector2(i, gameCamera->GetScreenHeight() + 1), 'A', Color::Magenta);
			Engine::Get().WriteToBuffer(Vector2(i, consoleHeight - 1), 'A', Color::Magenta);
			Engine::Get().WriteToBuffer(Vector2(i, 0), 'A', Color::Magenta);
		}
		// �׵θ� �׸��� (���μ�)
		for (int i = 0; i < consoleHeight; ++i)
		{
			Engine::Get().WriteToBuffer(Vector2(0, i), 'A', Color::Magenta);
			Engine::Get().WriteToBuffer(Vector2(consoleWidth - 1, i), 'A', Color::Magenta);
		}
		// �׵θ� �׸��� (���� ���м�)
		for (int i = 0; i < consoleHeight - gameCamera->GetScreenHeight() - 2; ++i)
		{
			Engine::Get().WriteToBuffer(Vector2(36, gameCamera->GetScreenHeight() + 2 + i), 'A', Color::Magenta);
			Engine::Get().WriteToBuffer(Vector2(72, gameCamera->GetScreenHeight() + 2 + i), 'A', Color::Magenta);
			Engine::Get().WriteToBuffer(Vector2(108, gameCamera->GetScreenHeight() + 2 + i), 'A', Color::Magenta);
		}
	}

	// �� (1 ~ 35)
	{
		int xPos = 1;
		int yPos = gameCamera->GetScreenHeight() + 2;
		Asset* characterAsset = nullptr;
		for (Asset* asset : assets)
		{
			if (strcmp(asset->GetKey(), "Num1") == 0)
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

				// Todo: �ϴ� ��� Ȯ�ο����� ��ġ�� ���� ��������� - ���߿� ���� �ʿ�
				Engine::Get().WriteToBuffer(
					Vector2((i % (characterAsset->GetWidth())) + xPos + 10, (i / characterAsset->GetWidth()) + yPos + 3),
					characterAsset->GetImage()[i],
					Color::White
				);
			}
		}
	}

	// ���� (37 ~ 71)


	// ���� (73 ~ 107) ���� 35ĭ ���� 20ĭ (57 ~ 71)
	// ���� ���� ���� 20, 20
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
	// �Ѿ� (109 ~ 180) ���� 72ĭ ���� 20ĭ (52 ~ 71)
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
	}

	// Todo: �Ѿ� ���� ������ �������
	{}


}

// ���� �̸����� ������ �ҷ��ͼ� �����صδ� �Լ�
void UIManager::AssetLoading(const char* _fileName)
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
