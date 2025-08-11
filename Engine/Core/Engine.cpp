#include "Engine.h"
#include <iostream>
#include <Windows.h>

#include "Level/Level.h"
#include "Utils/Utils.h"
#include "Input.h"
#include "Render/ScreenBuffer.h"

// 정적 변수 초기화
Engine* Engine::instance = nullptr;

BOOL WINAPI ConsoleMessageProcedure(DWORD _CtrlType)
{
	switch (_CtrlType)
	{
	case CTRL_CLOSE_EVENT:
		// Engine의 메모리 해제
		Engine::Get().CleanUp();
		return false;
	}

	return false;
}

Engine::Engine()
{
	// 싱글톤이 원래는 객체를 하나만 생성되게 중복 확인해야함
	// 여기서는 엔진을 하나만 생성할 걸 알고 있으니 일단 스킵
	instance = this;

	// 콘솔 커서 끄기
	CONSOLE_CURSOR_INFO info;
	info.bVisible = false;
	info.dwSize = 1;

	SetConsoleCursorInfo(
		GetStdHandle(STD_OUTPUT_HANDLE),
		&info
	);

	//// 콘솔창 폰트 사이즈 조정
	//CONSOLE_FONT_INFOEX cfi;
	//cfi.cbSize = sizeof(cfi);
	//cfi.nFont = 0;
	//cfi.dwFontSize.X = 5;
	//cfi.dwFontSize.Y = 11;
	//cfi.FontFamily = FF_DONTCARE;
	//cfi.FontWeight = FW_NORMAL;

	//SetCurrentConsoleFontEx(
	//	GetStdHandle(STD_OUTPUT_HANDLE),
	//	FALSE,
	//	&cfi
	//);

	// 엔진 설정 로드
	LoadEngineSettings();

	// 랜덤 종자값(seed) 설정
	srand(static_cast<unsigned int>(time(nullptr)));

	// 이미지 버퍼 생성.
	Vector2 screenSize(settings.width, settings.height);
	imageBuffer = new CHAR_INFO[(screenSize.x + 1) * screenSize.y + 1];

	// 버퍼 초기화 (문자 버퍼).
	ClearImageBuffer();

	// 두 개의 버퍼 생성.
	renderTargets[0] = new ScreenBuffer(GetStdHandle(STD_OUTPUT_HANDLE), screenSize);
	renderTargets[1] = new ScreenBuffer(screenSize);

	// 버퍼 교환.
	Present();

	// 콘솔 창 이벤트 등록
	SetConsoleCtrlHandler(ConsoleMessageProcedure, TRUE);

	// 콘솔 창 크기 변경 안되도록 설정.
	// "관리자 모드에서만 제대로 실행됨"
	DisableToResizeWindow();

	// cls 호출.
	system("cls");
}

Engine::~Engine()
{
	CleanUp();
}

void Engine::Run()
{
	// 밀리 세컨드 단위로 현재 시간을 알려줌
	LARGE_INTEGER currentTime;
	LARGE_INTEGER previousTime;
	QueryPerformanceCounter(&currentTime);
	previousTime = currentTime;

	// 하드웨어 시계의 정밀도(주파수) 가져오기
	// 나중에 초로 변환하기 위해
	LARGE_INTEGER frequency;
	QueryPerformanceFrequency(&frequency);

	// 타겟 프레임
	float targetFrameRate = settings.framerate == 0.0f ? 60.0f : settings.framerate;

	// 타겟 한 프레임 시간
	float oneFrameTime = 1.f / targetFrameRate;

	// GameLoop
	while (true)
	{
		// 엔진 종료 여부 확인
		if (isQuit)
		{
			// 루프 종료
			break;
		}

		// 프레임 시간 계산
		// (현재 시간 - 이전 시간) / 주파수 = 초단위
		QueryPerformanceCounter(&currentTime);

		// 프레임 시간
		float deltaTime = (currentTime.QuadPart - previousTime.QuadPart) / (float)frequency.QuadPart;

		// 입력은 최대한 빠르게 처리
		input.ProcessInput();

		// 고정 프레임
		if (deltaTime >= oneFrameTime)
		{
			BeginPlay();
			Tick(deltaTime);
			Render();

			// 제목에 FPS 출력
			char title[50] = { };
			sprintf_s(title, 50, "FPS: %f", (1.0f / deltaTime));
			SetConsoleTitleA(title);

			// 시간 업데이트
			previousTime = currentTime;

			// 현재 프레임의 입력을 기록
			input.SavePreviousKeyStates();

			// 이전 프레임에 추가 및 삭제 요청된 액터 처리
			if (mainLevel)
			{
				mainLevel->ProcessAddAndDestroyActors();
			}
		}
	}

	// 정리 - 텍스트 색상 원래대로 돌려놓기
	Utils::SetConsoleTextColor(FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
}

void Engine::WriteToBuffer(const Vector2& _position, const char* _image, Color _color)
{
	// 문자열 길이.
	int length = static_cast<int>(strlen(_image));

	// 문자열 기록.
	for (int ix = 0; ix < length; ++ix)
	{
		// 기록할 문자 위치.
		int index = (_position.y * (settings.width)) + _position.x + ix;

		// 버퍼에 문자/색상 기록.
		imageBuffer[index].Char.AsciiChar = _image[ix];
		imageBuffer[index].Attributes = (WORD)_color;
	}
}

void Engine::WriteToBuffer(const Vector2& _position, char _image, Color _color)
{
	// 기록할 문자 위치.
	int index = (_position.y * (settings.width)) + _position.x;

	// 버퍼에 문자/색상 기록.
	imageBuffer[index].Char.AsciiChar = _image;
	imageBuffer[index].Attributes = (WORD)_color;

}

void Engine::AddLevel(Level* _newLevel)
{
	// 기존에 있던 레벨은 제거
	if (nullptr != mainLevel)
	{
		delete mainLevel;
	}

	mainLevel = _newLevel;
}

void Engine::CleanUp()
{
	// 레벨 삭제.
	SafeDelete(mainLevel);

	// 문자 버퍼 삭제.
	SafeDeleteArray(imageBuffer);

	// 렌더 타겟 삭제.
	SafeDelete(renderTargets[0]);
	SafeDelete(renderTargets[1]);
}

void Engine::Quit()
{
	// 종료 플래그 설정
	isQuit = true;
}

Engine& Engine::Get()
{
	return *instance;
}

int Engine::GetWidth() const
{
	return settings.width;
}

int Engine::GetHeight() const
{
	return settings.height;
}

void Engine::OnInitialized()
{
}

void Engine::BeginPlay()
{
	if (nullptr != mainLevel)
	{
		mainLevel->BeginPlay();
	}
}

void Engine::Tick(float _deltaTime)
{
	// 레벨 업데이트
	if (nullptr != mainLevel)
	{
		mainLevel->Tick(_deltaTime);
	}
}

void Engine::Clear()
{
	ClearImageBuffer();
	GetRenderer()->Clear();
}

void Engine::Render()
{
	// 화면 지우기.
	Clear();


	if (nullptr != mainLevel)
	{
		mainLevel->Render();
	}

	// 백버퍼에 데이터 쓰기.
	GetRenderer()->Render(imageBuffer);

	// 버퍼 교환.
	Present();
}

void Engine::Present()
{
	// 버퍼 교환.
	SetConsoleActiveScreenBuffer(GetRenderer()->buffer);

	// 인덱스 뒤집기. 1->0, 0->1.
	currentRenderTargetIndex = 1 - currentRenderTargetIndex;
}

void Engine::LoadEngineSettings()
{
	FILE* file = nullptr;
	fopen_s(&file, "../Settings/EngineSettings.txt", "rt");
	if (nullptr == file)
	{
		std::cout << "Failed to load engine settings.\n";
		__debugbreak();
		return;
	}

	// 로드

	// FP(File Position) 포인터를 가장 뒤로 옮기기
	fseek(file, 0, SEEK_END);

	// 이 위치 구하기
	size_t fileSize = ftell(file);

	// 다시 첫 위치로 되돌리기
	rewind(file);

	// 파일 내용을 저장할 버퍼 할당
	char* buffer = new char[fileSize + 1];
	memset(buffer, 0, fileSize + 1);

	// 내용 읽기
	size_t readSize = fread(buffer, sizeof(char), fileSize, file);

	// 파싱(Parcing, 구문 해석 -> 필요한 정보를 얻는 과정)
	char* context = nullptr;
	char* token = nullptr;

	token = strtok_s(buffer, "\n", &context);

	// 구문 분석
	while (nullptr != token)
	{
		// 키/값 분리
		char header[10] = { };

		// 아래 구문이 제대로 동작하려면 키와 값 사이의 빈칸이 있어야 함
		sscanf_s(token, "%s", header, 10);

		// 헤더 문자열 비교
		if (strcmp(header, "framerate") == 0)
		{
			sscanf_s(token, "framerate = %f", &settings.framerate);
		}
		else if (strcmp(header, "width") == 0)
		{
			sscanf_s(token, "width = %d", &settings.width);
		}
		else if (strcmp(header, "height") == 0)
		{
			sscanf_s(token, "height = %d", &settings.height);
		}

		// 그 다움줄 분리
		token = strtok_s(nullptr, "\n", &context);
	}

	// 버퍼 해제
	SafeDeleteArray(buffer);

	// 파일 닫기
	fclose(file);
}

void Engine::DisableToResizeWindow()
{
	// 콘솔 창 핸들 가져오기.
	HWND window = GetConsoleWindow();

	// 콘솔 창에 설정된 스타일 값 가져오기.
	LONG style = GetWindowLong(window, GWL_STYLE);

	// 콘솔 창 스타일에서 크기 조절 관련 옵션 제거.
	style &= ~WS_MAXIMIZEBOX;
	style &= ~WS_SIZEBOX;

	// 콘솔창에 변경된 스타일 적용.
	SetWindowLongW(window, GWL_STYLE, style);
}

ScreenBuffer* Engine::GetRenderer() const
{
	return renderTargets[currentRenderTargetIndex];
}

void Engine::ClearImageBuffer()
{
	// 글자 버퍼 덮어쓰기.
	for (int y = 0; y < settings.height; ++y)
	{
		for (int x = 0; x < settings.width; ++x)
		{
			CHAR_INFO& buffer = imageBuffer[(y * (settings.width + 1)) + x];
			buffer.Char.AsciiChar = ' ';
			buffer.Attributes = 0;
		}

		// 각 줄 끝에 개행 문자 추가.
		CHAR_INFO& buffer = imageBuffer[(y * (settings.width + 1)) + settings.width];
		buffer.Char.AsciiChar = '\n';
		buffer.Attributes = 0;
	}

	// 마지막에 널 문자 추가.
	CHAR_INFO& buffer = imageBuffer[(settings.width + 1) * settings.height];
	buffer.Char.AsciiChar = '\0';
	buffer.Attributes = 0;
}
