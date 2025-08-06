#include "Wall.h"
#include "Level/GameLevel.h"

Wall::Wall(const Vector2& _position)
	: Actor("#", Color::White, _position), isCheck(false)
{
	SetSortingOrder(0);
}

void Wall::Tick(float _deltaTime)
{
	
}

void Wall::Render()
{
	if (dynamic_cast<GameLevel*>(owner)->isFPS == false)
	{
		super::Render();
	}
}
