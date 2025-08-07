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

	// Todo: �׽�Ʈ�� ���� ���� ���� �Լ�
	void GoNextLevel();

	inline int GetCurStage() { return curStage; }
	inline void SetCurStage(int _curStage) { curStage = _curStage; }

	virtual void CleanUp() override;
	static Game& Get();

public:
	// ���� �ʱ�ȭ
	void ResetLevel();
	
private:
	void AddLevelToVector(Level* _level, const char* _mapFile);

private:
	Level* menuLevel = nullptr;

	// ���� ������ ���������� ��Ÿ�� ����
	// �޴� ���� 1 = 1��, 2 = 2�� ~
	int curStage = 1;

	bool isLevelChangePreviousFrame = false;

	static Game* instance;

	std::vector<std::pair<const char*, Level*>> loadedLevels;
};