#include <iostream>
#include "Game/Game.h"

void TestScreenAndBufferSize()
{
	for (int width = 50; width <= 180; ++width)
	{
		for (int height = 40; height <= 100; ++height)
		{
			Vector2 screenSize(width, height);

			HANDLE buffer = GetStdHandle(STD_OUTPUT_HANDLE);

			// 콘솔 창 크기 설정.
			SMALL_RECT rect{ 0, 0, (short)screenSize.x, (short)screenSize.y };
			if (SetConsoleScreenBufferSize(buffer, Vector2(screenSize.x + 1, screenSize.y + 1))
				&& SetConsoleWindowInfo(buffer, TRUE, &rect))
			{
				char message[100] = {};
				sprintf_s(message, 100, "Succeeded screenSize: (%d, %d)\n", screenSize.x, screenSize.y);
				OutputDebugStringA(message);
				std::cout << message;
			}
			else
			{
				char message[100] = {};
				sprintf_s(message, 100, "Failed screenSize: (%d, %d)\n", screenSize.x, screenSize.y);
				OutputDebugStringA(message);
			}
		}
	}
}

#define Test 0

int main()
{
#if !Test
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	Game game;
	game.Run();
#else
	TestScreenAndBufferSize();
	std::cin.get();
#endif
}