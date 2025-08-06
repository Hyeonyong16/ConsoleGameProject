#include "Monster.h"
#include "Level/GameLevel.h"

Monster::Monster(const Vector2& _position)
	: Actor("M", Color::Red, _position)
{
	DeathTimer.SetTargetTime(1.f);
}

void Monster::Tick(float _deltaTime)
{
	super::Tick(_deltaTime);

	if (isKilled)
	{
		DeathTimer.Tick(_deltaTime);
		if (DeathTimer.IsTimeout())
		{
			std::vector<int>& tempMonsterIds = GetOwner()->As<GameLevel>()->GetMonsterIDs();
			auto it = std::remove(tempMonsterIds.begin(), tempMonsterIds.end(), iD);
			tempMonsterIds.erase(it, tempMonsterIds.end());
			dynamic_cast<GameLevel*>(GetOwner())->DecreaseMonsterNum();
			dynamic_cast<GameLevel*>(GetOwner())->AddScore();
			Destroy();
		}
	}
}

void Monster::Render()
{
	if (dynamic_cast<GameLevel*>(owner)->isFPS == false)
	{
		super::Render();
	}
}
