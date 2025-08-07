#include "Portal.h"
#include "Level/Level.h"
#include "Level/GameLevel.h"
#include "Game/Game.h"

Portal::Portal(const Vector2& _position)
	:Actor("N", Color::BlueIntensity, _position)
{
}

void Portal::Tick(float _deltaTime)
{
	super::Tick(_deltaTime);

	if(isActive)
	{
		if (isPlayerCollision)
		{
			Game::Get().GoNextLevel();
		}
	}
}

void Portal::Render()
{
	if (dynamic_cast<GameLevel*>(owner)->isFPS == false)
	{
		super::Render();
	}
}
