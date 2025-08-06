#pragma once

#include "Core.h"
#include "Math/Vector2.h"
#include "Math/Color.h"
#include "RTTI.h"

// Actor 정의
// 1. 위치 점령
// 2. 콘솔 창에 그리기(어떻게/무엇을)
// 3. 엔진의 이벤트 함수 호출(BeginPlay/Tick/Draw)

// 전방선언
class Level;

class Engine_API Actor : public RTTI		// 최근은 RTTI 가 아닌 Object 이름으로 자주 사용
{
	friend class Level;
	RTTI_DECLARATIONS(Actor, RTTI)			// 왼쪽은 자손, 오른쪽은 부모타입 지정

public:
	Actor(
		const char* _image = "",
		Color _color = Color::White,
		const Vector2& _position = Vector2::Zero
	);
	virtual ~Actor();

	// 이벤트 함수
	virtual void BeginPlay();				// 객체 생애주기(Lifetime)에 1번만 호출(초기화 목적)
	virtual void Tick(float _deltaTime);	// 프레임마다 호출 (반복성 작업/지속성이 필요한 작업)
	virtual void Render();					// 그리기 함수

	// BeginPlay 함수 호출 여부 
	inline bool HasBeganPlay() const { return hasBeganPlay; }

	// 위치 설정 / 값 읽는 함수
	void SetPosition(const Vector2& _newPosition);
	Vector2 GetPosition() const;

	// 색상 변경 함수
	Color GetColor() const { return color; }
	void SetColor(const Color _color);

	int GetWidth() const;
	const char* GetImage() const { return image; }

	// SortingOrder 설정 함수
	// 숫자가 클수록 카메라에 가까움
	inline unsigned int GetSortingOrder() const { return sortingOrder; }
	void SetSortingOrder(unsigned int _sortingOrder);

	// 오너십 설정
	void SetOwner(Level* _newOwner);
	Level* GetOwner();

	// ID 확인
	inline int GetID() const { return iD; }

	// 충돌 확인 요청 함수(간단한 AABB 로직)
	bool TestIntersect(const Actor* const _other);
	
	// 객체 삭제 함수
	void Destroy();

	// 객체가 삭제되기 직전에 호출되는 함수.
	virtual void OnDestroy();

	// 수명 주기 설정 함수.
	void SetLifetime(float _newLifetime);

	// 게임 종료 요청 함수
	void QuitGame();

protected:
	Vector2 position;				// 개체의 위치
	char* image;					// 그릴 값
	int width = 0;					// 문자열 길이
	Color color;					// 텍스트 색상 값

	bool hasBeganPlay = false;		// BeginPlay 호출이 되었는지 확인

	unsigned int sortingOrder = 0;	// 정렬 순서

	bool isActive = true;			// 액터가 활성상태인지
	bool isExpired = false;			// 삭제 요청되었는지 
	
	float lifetime = 0.0f;			// 수명 주기 (단위: 초)
	bool autoDestroy = false;		// 이 플래그를 true로 설정하면 수명 주기를 사용해 자동 제거

	Level* owner = nullptr;			// 소유 레벨(오너십)

	static int nextID;
	int iD = 1;
};

