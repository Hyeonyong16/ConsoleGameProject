#pragma once
#include "Level/Level.h"
#include <vector>


class GameLevel : public Level
{
	RTTI_DECLARATIONS(GameLevel, Level)

public:
	GameLevel();
	~GameLevel();

	virtual void BeginPlay() override;
	virtual void Tick(float _deltaTime) override;
	virtual void Render() override;

	inline int**& getWallMap() { return wallMap; }
	inline int GetMapWidth() { return mapWidth; }
	inline int GetMapHeight() { return mapHeight; }

	std::vector<int>& GetItemIDs() { return itemIDs; }

	inline void AddScore() { ++score; }

	bool isFPS = true;

private:
	// 맵 파일 불러오기
	void ReadMapFile(const char* _fileName);

private:
	int mapWidth = 0;
	int mapHeight = 0;
	// iD 로 저장
	int** wallMap = nullptr;

	// Todo: item 확인용 점수
	int score = 0;

	std::vector<int> itemIDs;

};