#pragma once
#include "Level/Level.h"

#include <vector>

class Player;
class Camera;
class GameLevel : public Level
{
	RTTI_DECLARATIONS(GameLevel, Level)

public:
	GameLevel();
	GameLevel(const char* _mapFile);
	~GameLevel();

	// 시점 함수
	virtual void BeginPlay() override;
	virtual void Tick(float _deltaTime) override;
	virtual void Render() override;

public:
	// 레벨 다시 진입 확인 함수
	bool isReEntry = false;

public:
	// Get, Set 함수
	inline int**& getWallMap() { return wallMap; }
	inline int GetMapWidth() const { return mapWidth; }
	inline int GetMapHeight() const { return mapHeight; }

	inline int GetScore() const { return score; }
	inline Player* GetPlayer() const { return player; }

	inline Camera* GetCamera() const { return camera; }
	inline void SetCamera(Camera* _camera) { camera = _camera; }

	inline int GetMonsterNum() const { return monsterNum; }
	inline void DecreaseMonsterNum() { --monsterNum; }

	std::vector<int>& GetItemIDs() { return itemIDs; }
	std::vector<int>& GetMonsterIDs() { return monsterIDs; }

	// 점수 증가
	inline void AddScore() { ++score; }

	// 1인칭 여부
	bool isFPS = true;

private:
	// 맵 파일 불러오기
	void ReadMapFile(const char* _fileName);

private:
	int mapWidth = 0;
	int mapHeight = 0;
	// iD 로 저장
	int** wallMap = nullptr;

	// 점수
	int score = 0;

	// 현재 맵 몬스터 수
	int monsterNum = 0;

	// 현재 레벨의 플레이어 기록용
	Player* player = nullptr;
	Camera* camera = nullptr;

	// 아이템 ID를 저장해둘 vector 컨테이너
	std::vector<int> itemIDs;
	std::vector<int> monsterIDs;

	// 현재 맵 파일
	char* curMapFile = nullptr;
};