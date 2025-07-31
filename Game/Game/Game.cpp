#include "Game.h"
#include "Level/GameLevel.h"

Game::Game()
{
	// 레벨 추가.
	AddLevel(new GameLevel());
}
