#pragma once

#include "Engine.h"
#include <vector>
#include <utility>

class Level;
class Game : public Engine
{
public:
	Game();
	~Game();

	// 메뉴 전환 함수.
	void GoMenu();

	// 맵 데이터를 넣으면 해당 레벨로 이동
	void GoLevel(const char* _map);

	virtual void CleanUp() override;
	static Game& Get();

	
private:
	void AddLevelToVector(Level* _level, const char* _mapFile);

private:
	Level* menuLevel = nullptr;
	Level* map1Level = nullptr;

	bool isLevelChangePreviousFrame = false;

	static Game* instance;

	std::vector<std::pair<const char*, Level*>> loadedLevels;
};