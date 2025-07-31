#include "GameLevel.h"
#include "Actor/Player.h"

GameLevel::GameLevel()
{
	// 플레이어 추가.
	AddActor(new Player());
}

void GameLevel::BeginPlay()
{
	super::BeginPlay();

}

void GameLevel::Tick(float _deltaTime)
{
	super::Tick(_deltaTime);
}

void GameLevel::Render()
{
	super::Render();
}
