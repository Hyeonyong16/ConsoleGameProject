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

	// �޴� ��ȯ �Լ�.
	void GoMenu();

	// �� �����͸� ������ �ش� ������ �̵�
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