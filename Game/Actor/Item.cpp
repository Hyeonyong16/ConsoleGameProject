#include "Item.h"
#include "Level/Level.h"
#include "Level/GameLevel.h"

Item::Item(const Vector2& _position)
	: Actor("I", Color::Cyan, _position)
{
}

void Item::Tick(float _deltaTime)
{
	super::Tick(_deltaTime);

	if(isPlayerCollision)
	{
		if (GetOwner()->As<GameLevel>())
		{
			std::vector<int>& tempItemIds = GetOwner()->As<GameLevel>()->GetItemIDs();
			auto it = std::remove(tempItemIds.begin(), tempItemIds.end(), iD);
			tempItemIds.erase(it, tempItemIds.end());
			dynamic_cast<GameLevel*>(GetOwner())->AddScore();
		}
		Destroy();
	}
}

void Item::Render()
{
	if (dynamic_cast<GameLevel*>(owner)->isFPS == false)
	{
		super::Render();
	}
}
