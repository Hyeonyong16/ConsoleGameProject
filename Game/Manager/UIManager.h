#pragma once
#include "Actor/Actor.h"
#include "Asset/Asset.h"

#include <vector>

// Manager 라고 적었지만 화면에 UI를 그릴 Actor

// Console
// width = 182
// height = 73

// Screen(Camera)
// width = 180
// Height = 50

// 그려야될 UI
// 1. 테두리
// 2. 층수
// 3. 점수
// 4. 대충 얼굴 사진같은거 하나
// 5. 총알
// 6. 총 이미지 하나
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

	// 이미지 에셋을 미리 로딩 해놓을 컨테이너
	std::vector<Asset*> assets;
};