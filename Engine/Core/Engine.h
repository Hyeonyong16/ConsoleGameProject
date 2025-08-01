#pragma once

#include "Core.h"
#include "Input.h"
#include "Math/Color.h"
#include "Math/Vector2.h"

#include <Windows.h>

// ���� ���� ����ü
struct EngineSettings
{
	int	width = 0;				// �ܼ� ȭ�� ���� ũ��
	int	height = 0;				// �ܼ� ȭ�� ���� ũ��
	float framerate	= 0.0f;		// Ÿ�� ������ �ӵ�
};

class Level;
class ScreenBuffer;
class Engine_API Engine
{
public:
	Engine();

	virtual ~Engine();

	// ���� ���� �Լ�
	void Run();

	// ���ڿ� �׸��� ��û �Լ�.
	void WriteToBuffer(const Vector2& _position, const char* _image, Color _color = Color::White);

	// ���� �߰� �Լ�
	void AddLevel(Level* _newLevel);

	// �޸� ���� �Լ�
	virtual void CleanUp();

	// ���� �Լ�
	void Quit();

	// �̱��� ���� �Լ�
	static Engine& Get();

	// ȭ�� ����/���� ũ�� ��ȯ �Լ�
	int GetWidth() const;
	int GetHeight() const;

protected:	

	// ����/���� �ʱ�ȭ �� ��� (���� �߰� ��).
	virtual void OnInitialized();

	void BeginPlay();
	void Tick(float _deltaTime = 0.0f);

	// ȭ�� ����� �Լ� (��ü�� �� ���ڿ��� ����).
	void Clear();

	// ���� �׸��� �Լ� (����ۿ� ���).
	void Render();

	// ���� ��ȯ �Լ� (����Ʈ ���� <-> �����)
	void Present();

	// ���� ���� �ε� �Լ�
	void LoadEngineSettings();

	// ����� ���� Ÿ�� ��ȯ �Լ�.
	ScreenBuffer* GetRenderer() const;

	// ���� ���� ����� �Լ�.
	void ClearImageBuffer();

protected:
	// ���� ���� �÷���
	bool isQuit = false;

	// ���� ����
	Level* mainLevel = nullptr;

	// �Է� ������
	Input input;

	// ���� ����
	EngineSettings settings;

	// �����(������).
	CHAR_INFO* imageBuffer = nullptr;

	// ���� ����.
	ScreenBuffer* renderTargets[2] = { };

	// ����۷� ����ϴ� ���� Ÿ���� �ε���.
	int currentRenderTargetIndex = 0;

	// �̱��� ����
	static Engine* instance;
};