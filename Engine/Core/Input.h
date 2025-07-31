#pragma once

#include "Core.h"

class Engine_API Input
{
	// friend 선언
	friend class Engine;

	// 키입력 확인을 위한 구조체
	struct KeyState
	{
		bool isKeyDown = false;			// 현재 프레임에 키가 눌렸는지 여부
		bool previousKeyDown = false;	// 이전 프레임에 키가 눌렸는지 여부
	};

public:
	Input();

	// 키 확인 함수
	bool GetKey(int _keyCode);
	bool GetKeyDown(int _keyCode);
	bool GetKeyUp(int _keyCode);

	static Input& Get();

private:
	void ProcessInput();
	void SavePreviousKeyStates();

private:
	// 키 입력 정보 관리 변수
	KeyState keyStates[255] = { };

	static Input* instance;
};

