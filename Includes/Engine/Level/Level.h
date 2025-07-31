#pragma once

#include "Core.h"
#include <vector>
#include <RTTI.h>

class Actor;

class Engine_API Level : public RTTI
{
	RTTI_DECLARATIONS(Level, RTTI)

public:
	Level();
	virtual ~Level();

	// 레벨에 액터를 추가할 때 사용
	void AddActor(Actor* _newActor);

	// 엔진 이벤트 함수
	virtual void BeginPlay();
	virtual void Tick(float _deltaTime);
	virtual void Render();

private:
	// SortingOrder 로 Actors 정렬
	void SortActorsBySortingOrder();

protected:
	std::vector<Actor*> actors; //레벨에 배치된 모든 액터를 관리하는 배열

};

