#pragma once

#include "Actor/Actor.h"
#include "Utils/Timer.h"

class Monster : public Actor
{
	RTTI_DECLARATIONS(Monster, Actor)

public:
	Monster(const Vector2& _position);

	virtual void Tick(float _deltaTime) override;
	virtual void Render() override;

	inline void SetIsKilled(bool _isKilled) { isKilled = _isKilled; }
	inline bool GetIsKilled() const { return isKilled; }

	inline Timer GetDeathTimer() const { return DeathTimer; }

private:
	bool isKilled = false;
	Timer DeathTimer;
};