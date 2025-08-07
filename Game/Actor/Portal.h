#pragma once

#include "Actor/Actor.h"

class Portal : public Actor
{
	RTTI_DECLARATIONS(Portal, Actor)

public:
	Portal(const Vector2& _position);

	virtual void Tick(float _deltaTime) override;
	virtual void Render() override;

public:
	inline bool GetIsActive() { return isActive; }
	inline void SetIsActive(bool _isActive) { isActive = _isActive; }

	inline bool GetPlayerCollision() { return isPlayerCollision; }
	inline void SetPlayerCollision(bool _playerCollision) { isPlayerCollision = _playerCollision; }

private:
	bool isActive = false;
	bool isPlayerCollision = false;
};