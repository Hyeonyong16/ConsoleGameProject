#pragma once

#include "Actor/Actor.h"

class Item : public Actor
{
	RTTI_DECLARATIONS(Item, Actor)

public:
	Item(const Vector2& _position);

	inline void SetPlayerCollision(bool _playerCollision) { isPlayerCollision = _playerCollision; }
	inline bool GetPlayerCollision() { return isPlayerCollision; }

	virtual void Tick(float _deltaTime) override;
	virtual void Render() override;

private:
	bool isPlayerCollision = false;
};