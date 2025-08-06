#include "Level.h"
#include "Actor/Actor.h"
#include "Utils/Utils.h"

#include <iostream>

Level::Level()
{ }


Level::~Level()
{ 
	for (Actor* actor : actors)
	{
		SafeDelete(actor);
	}

	actors.clear();

	for (Actor* actor : addRequestedActors)
	{
		SafeDelete(actor);
	}

	addRequestedActors.clear();

	for (Actor* actor : destroyRequestedActors)
	{
		SafeDelete(actor);
	}

	destroyRequestedActors.clear();
}

// 레벨에 액터를 추가할 때 사용
void Level::AddActor(Actor* _newActor)
{
	// 예외처리(중복 여부 확인) 필요
	
	// 대기 배열에 추가
	addRequestedActors.emplace_back(_newActor);

	// 오너십 설정
	//_newActor->SetOwner(this);
}

Actor* Level::FindActorByID(const int _iD)
{
	for (Actor* actor : actors)
	{
		if (actor->GetID() == _iD)
			if(actor->isExpired == false)
				return actor;
	}

	return nullptr;
}

void Level::DestroyActor(Actor* _destroyedActor)
{
	// 대기 배열에 추가
	destroyRequestedActors.emplace_back(_destroyedActor);
}

// 엔진 이벤트 함수
void Level::BeginPlay()
{
	for (Actor* const actor : actors)
	{
		// 액터 처리 여부 확인
		if (!actor->isActive || actor->isExpired)
		{
			continue;
		}

		// 이미 호출된 개체는 건너뛰기
		if (actor->HasBeganPlay())
		{
			continue;
		}

		actor->BeginPlay();
	}
}

void Level::Tick(float _deltaTime)
{
	for (Actor* const actor : actors)
	{
		if (nullptr == actor) continue;
		// 액터 처리 여부 확인
		if (!actor->isActive || actor->isExpired)
		{
			continue;
		}

		// 수명 주기 확인 및 처리.
		if (actor->autoDestroy)
		{
			// 수명 주기 감소 처리.
			actor->lifetime -= _deltaTime;

			// 수명이 다했는지 확인.
			if (actor->lifetime <= 0.0f)
			{
				// 액터 제거 요청하고, 다음 액터 처리.
				actor->lifetime = 0.0f;
				actor->Destroy();
				continue;
			}
		}

		actor->Tick(_deltaTime);
	}
}

void Level::Render()
{ 
	// 렌더링 전에 정렬 순서 기준으로 재배치
	SortActorsBySortingOrder();

	// Render Pass
	for (Actor* const actor : actors)
	{
		// 검사 ( 같은 위치에 정렬 순서가 높은 액터가 있는지 확인)
		Actor* searchedActor = nullptr;
		for (Actor* const otherActor : actors)
		{
			// 액터 처리 여부 확인
			if (!actor->isActive || actor->isExpired)
			{
				continue;
			}

			// 같은 액터면 무시
			if (actor == otherActor) continue;

			// 위치가 같은 액터 확인
			if (actor->GetPosition() == otherActor->GetPosition())
			{
				// 정렬 순서 비교 후 액터 저장
				if (actor->sortingOrder < otherActor->sortingOrder)
				{
					// 저장 및 루프 종료
					searchedActor = otherActor;
					break;
				}
			}
		}
		
		// 어떤 액터와 같은 위치에 정렬 순서가 더 높은 액터가 있으면 그리지 않고 건너뛰기
		if (nullptr != searchedActor)
		{
			continue;
		}

		// 드로우 콜
		actor->Render();
	}
}

void Level::ProcessAddAndDestroyActors()
{
	// actors 배열에서 삭제 처리
	for (auto iterator = actors.begin(); iterator != actors.end();)
	{
		// 삭제 요청된 액터인지 확인 후 배열에서 제외
		if ((*iterator)->isExpired)
		{
			// erase 함수를 사용하면 iterator 가 무효화되기 때문에
			// 반환되는 값을 저장해야함
			iterator = actors.erase(iterator);
			continue;
		}

		++iterator;
	}

	// destroyedRequestedActors 배열을 순회하면서 Actor를 delete
	for (auto* actor : destroyRequestedActors)
	{
		// 액터가 그렸던 곳 지우기
		Utils::SetConsolePosition(actor->position);

		// 콘솔에 빈문자 출력해서 지우기
		for (int i = 0; i < actor->width; ++i)
		{
			std::cout << " ";
		}

		// 리소스 해제
		SafeDelete(actor);
	}

	// 배열 초기화
	destroyRequestedActors.clear();

	// addRequestedActors 배열을 순회하면서 새로운 Actor 추가
	for (auto* const actor : addRequestedActors)
	{
		actors.emplace_back(actor);
		actor->SetOwner(this);
	}

	// 배열 초기화
	addRequestedActors.clear();
}

// SortingOrder 에 따른 Actor 정렬
void Level::SortActorsBySortingOrder()
{
	// 버블 정렬
	for (int i = 0; i < (int)actors.size(); ++i)
	{
		for (int j = 0; j < (int)actors.size() - 1; ++j)
		{
			// sortingOrder 값이 클수록 뒤 쪽에 배치
			if (actors[j]->sortingOrder > actors[j + 1]->sortingOrder)
			{
				// 두 액터 위치 교환
				std::swap(actors[j], actors[j + 1]);
			}
		}
	}
	
}
