#include "Actor.h"
#include "Utils/Utils.h"
#include "Engine.h"
#include "Level/level.h"

#include <iostream>
#include <Windows.h>

int Actor::nextID = 0;

Actor::Actor(const char* _image, Color _color, const Vector2& _position)
	: color(_color), position(_position), iD(nextID++)
{
	// 문자열 길이
	width = (int)strlen(_image);

	// 메모리 할당
	image = new char[width + 1];

	// 문자열 복사
	strcpy_s(image, width + 1, _image);
}

Actor::~Actor()
{
	// 메모리 해제
	SafeDeleteArray(image);
}

// 이벤트 함수
// 객체 생애주기(Lifetime)에 1번만 호출(초기화 목적)
void Actor::BeginPlay()
{
	hasBeganPlay = true;
}

// 프레임마다 호출 (반복성 작업/지속성이 필요한 작업)
void Actor::Tick(float _deltaTime)
{
}

// 그리기 함수
void Actor::Render()
{
	// 엔진이 관리하는 이미지 버퍼에 액터의 문자열/색상 기록.
	Engine::Get().WriteToBuffer(position, image, color);
}

void Actor::SetPosition(const Vector2& _newPosition)
{
	// 예외처리 (화면 벗어났는지 확인)
	if (_newPosition.x < 0) return;										// 화면 왼쪽
	if (_newPosition.x + width - 1 > Engine::Get().GetWidth()) return;	// 화면 오른쪽
	if (_newPosition.y < 0) return;										// 화면 위쪽
	if (_newPosition.y - 1 > Engine::Get().GetHeight()) return;			// 화면 아래쪽

	if (position == _newPosition) return;								// 같으면 업데이트 안함

	//// 지울 위치 확인
	//Vector2 direction = _newPosition - position;
	//position.x = direction.x >= 0 ? position.x : position.x + width - 1;

	//// 커서 이동
	//Utils::SetConsolePosition(position);

	//// 문자열 길이 고려해서 지울 위치 확인해야 함
	//std::cout << ' ';

	position = _newPosition;
}

Vector2 Actor::GetPosition() const
{
	return position;
}

void Actor::SetColor(const Color _color)
{
	color = _color;
}

int Actor::GetWidth() const
{
	return width;
}

void Actor::SetSortingOrder(unsigned int _sortingOrder)
{
	sortingOrder = _sortingOrder;
}

void Actor::SetOwner(Level* _newOwner)
{
	owner = _newOwner;
}

Level* Actor::GetOwner()
{
	return owner;
}

bool Actor::TestIntersect(const Actor* const _other)
{
	// AABB(Axis Aligned Bounding Box)
	// Note: 현재 액터 구조 상 세로는 크기가 없음 (크기가 1)
	//		 따라서 가로의 최소/최대 위치만 더 고려하면 됨

	// 이 액터의 x 좌표 정보
	int xMin = position.x;
	int xMax = position.x + width - 1;

	// 충돌 비교할 다른 액터의 x 좌표 정보
	int otherXMin = _other->position.x;
	int otherXMax = _other->position.x + _other->width - 1;

	// 안겹치는 조건 확인
	// 다른 액터의 왼쪽 좌표가 내 오른쪽 좌표보다 오른쪽에 있으면 안겹침
	if (otherXMin > xMax)
	{
		return false;
	}

	// 다른 액터의 오른쪽 좌표가 내 왼쪽 좌표보다 왼쪽에 있으면 안겹침
	if (otherXMax < xMin)
	{
		return false;
	}

	// y 좌표가 같은지 최종 확인
	return position.y == _other->position.y;
}

void Actor::Destroy()
{
	// 중복 삭제 방지 처리
	if (isExpired)
	{
		return;
	}

	isExpired = true;

	// 레벨에 삭제 요청
	owner->DestroyActor(this);

	// 죽음 요청됐다고 알림.
	OnDestroy();
}

void Actor::OnDestroy()
{
}

void Actor::SetLifetime(float _newLifetime)
{
	// 입력값 확인.
	if (_newLifetime <= 0.0f)
	{
		return;
	}

	// 수명 주기 설정.
	lifetime = _newLifetime;

	// 자동 제거 옵션 활성화.
	autoDestroy = true;
}

void Actor::QuitGame()
{
	Engine::Get().Quit();
}
