#pragma once

#include "Core.h"
#include "Input.h"
#include "Math/Color.h"
#include "Math/Vector2.h"

#include <Windows.h>

// 엔진 설정 구조체
struct EngineSettings
{
	int	width = 0;				// 콘솔 화면 가로 크기
	int	height = 0;				// 콘솔 화면 세로 크기
	float framerate	= 0.0f;		// 타겟 프레임 속도
};

class Level;
class ScreenBuffer;
class Engine_API Engine
{
public:
	Engine();

	virtual ~Engine();

	// 엔진 실행 함수
	void Run();

	// 문자열 그리기 요청 함수.
	void WriteToBuffer(const Vector2& _position, const char* _image, Color _color = Color::White);
	void WriteToBuffer(const Vector2& _position, char _image, Color _color = Color::White);

	// 레벨 추가 함수
	void AddLevel(Level* _newLevel);

	// 메모리 해제 함수
	virtual void CleanUp();

	// 종료 함수
	void Quit();

	// 싱글톤 접근 함수
	static Engine& Get();

	// 화면 가로/세로 크기 반환 함수
	int GetWidth() const;
	int GetHeight() const;

protected:	

	// 엔진/게임 초기화 시 사용 (레벨 추가 등).
	virtual void OnInitialized();

	void BeginPlay();
	void Tick(float _deltaTime = 0.0f);

	// 화면 지우는 함수 (전체를 빈 문자열로 설정).
	void Clear();

	// 액터 그리기 함수 (백버퍼에 기록).
	void Render();

	// 버퍼 교환 함수 (프론트 버퍼 <-> 백버퍼)
	void Present();

	// 엔진 설정 로드 함수
	void LoadEngineSettings();

	// 콘솔 창 크기 조절 안되도록 할 때 사용하는 함수.
	void DisableToResizeWindow();

	// 백버퍼 렌더 타겟 반환 함수.
	ScreenBuffer* GetRenderer() const;

	// 글자 버퍼 지우는 함수.
	void ClearImageBuffer();

protected:
	// 엔진 종료 플래그
	bool isQuit = false;

	// 메인 레벨
	Level* mainLevel = nullptr;

	// 입력 관리자
	Input input;

	// 엔진 설정
	EngineSettings settings;

	// 백버퍼(프레임).
	CHAR_INFO* imageBuffer = nullptr;

	// 이중 버퍼.
	ScreenBuffer* renderTargets[2] = { };

	// 백버퍼로 사용하는 렌더 타겟의 인덱스.
	int currentRenderTargetIndex = 0;

	// 싱글톤 변수
	static Engine* instance;
};