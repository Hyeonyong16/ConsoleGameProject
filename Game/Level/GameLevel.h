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
	~GameLevel();

	virtual void BeginPlay() override;
	virtual void Tick(float _deltaTime) override;
	virtual void Render() override;

	inline int**& getWallMap() { return wallMap; }
	inline int GetMapWidth() const { return mapWidth; }
	inline int GetMapHeight() const { return mapHeight; }

	inline int GetScore() const { return score; }
	inline Player* GetPlayer() const { return player; }

	inline Camera* GetCamera() const { return camera; }
	inline void SetCamera(Camera* _camera) { camera = _camera; }

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

	// 점수
	int score = 0;

	// 현재 레벨의 플레이어 기록용
	Player* player = nullptr;
	Camera* camera = nullptr;

	std::vector<int> itemIDs;

};