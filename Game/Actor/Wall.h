#pragma once

#include "Actor/Actor.h"

class Wall : public Actor
{
	RTTI_DECLARATIONS(Wall, Actor)

public:
	Wall(const Vector2& _position);

	virtual void Tick(float _deltaTime) override;
	virtual void Render() override;

	inline bool GetIsCheck() const { return isCheck; }
	inline void SetIsCheck(bool _isCheck) { isCheck = _isCheck; }

private:
	bool isCheck = false;
};