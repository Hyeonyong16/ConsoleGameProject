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

	// Todo: 테스트용 다음 레벨 가는 함수
	void GoNextLevel();

	inline int GetCurStage() { return curStage; }
	inline void SetCurStage(int _curStage) { curStage = _curStage; }

	virtual void CleanUp() override;
	static Game& Get();

public:
	// 레벨 초기화
	void ResetLevel();
	
private:
	void AddLevelToVector(Level* _level, const char* _mapFile);

private:
	Level* menuLevel = nullptr;

	// 현재 진행중 스테이지를 나타낼 변수
	// 메뉴 제외 1 = 1층, 2 = 2층 ~
	int curStage = 1;

	bool isLevelChangePreviousFrame = false;

	static Game* instance;

	std::vector<std::pair<const char*, Level*>> loadedLevels;
};