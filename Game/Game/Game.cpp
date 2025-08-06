#include "Game.h"
#include "Level/GameLevel.h"
#include "Level/MenuLevel.h"

Game* Game::instance = nullptr;

Game::Game()
{
	instance = this;
	// 레벨 추가.
	menuLevel = new MenuLevel();
	AddLevelToVector(menuLevel, " ");

	map1Level = new GameLevel("Map1.txt");
	AddLevelToVector(map1Level, "Map1.txt");
	//AddLevelToVector(new GameLevel("Map2.txt"), "Map2.txt");

	AddLevel(menuLevel);
}

Game::~Game()
{
	CleanUp();
}

void Game::GoMenu()
{
	// 화면 정리.
	// clear screen 명령어 실행.
	system("cls");

	// 메뉴 레벨을 메인 레벨로 설정.
	mainLevel = menuLevel;

	// 기존 레벨 제거
	isLevelChangePreviousFrame = true;
	//SafeDelete(backLevel);
}

void Game::GoLevel(const char* _map)
{
	// 화면 정리.
	// clear screen 명령어 실행.
	system("cls");

	/*for (std::pair<const char* , Level*> level : loadedLevels)
	{
		if (strcmp(level.first, _map) == 0)
		{
			mainLevel = level.second;
		}
	}*/
	mainLevel = map1Level;
}

void Game::CleanUp()
{
	SafeDelete(map1Level);
	SafeDelete(menuLevel);

	/*for (std::pair<const char*, Level*> level : loadedLevels)
	{
		SafeDelete(level.second);
	}*/

	mainLevel = nullptr;

	//loadedLevels.clear();

	Engine::CleanUp();
}

Game& Game::Get()
{
	return *instance;
}

// 레벨들을 관리하기 위한 벡터
void Game::AddLevelToVector(Level* _level, const char* _mapFile)
{
	loadedLevels.emplace_back(std::make_pair(_mapFile, _level));
}
