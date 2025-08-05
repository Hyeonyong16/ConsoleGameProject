#pragma once
#include "Actor/Actor.h"
#include "Asset/Asset.h"

#include <vector>

// Manager ��� �������� ȭ�鿡 UI�� �׸� Actor

// Console
// width = 182
// height = 73

// Screen(Camera)
// width = 180
// Height = 50

// �׷��ߵ� UI
// 1. �׵θ�
// 2. ����
// 3. ����
// 4. ���� �� ���������� �ϳ�
// 5. �Ѿ�
// 6. �� �̹��� �ϳ�
class GameLevel;
class Camera;
class UIManager : public Actor
{
	RTTI_DECLARATIONS(UIManager, Actor)

public:
	UIManager(GameLevel* _gameLevel);
	~UIManager();

	virtual void Tick(float _deltaTime) override;
	virtual void Render() override;

private:
	void AssetLoading(const char* _fileName);

private:
	GameLevel* gameLevel = nullptr;
	Camera* gameCamera = nullptr;

	// �̹��� ������ �̸� �ε� �س��� �����̳�
	std::vector<Asset*> assets;
};