#pragma once
#include "Level/Level.h"


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

	bool isFPS = true;

private:
	// �� ���� �ҷ�����
	void ReadMapFile(const char* _fileName);

private:
	int mapWidth = 0;
	int mapHeight = 0;
	// iD �� ����
	int** wallMap = nullptr;
};