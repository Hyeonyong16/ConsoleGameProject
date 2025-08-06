#pragma once

#include "Level/Level.h"
#include <vector>
#include "Actor/Actor.h"
#include "Asset/Asset.h"

struct MenuItem
{
	// 함수 포인터 선언.
	typedef void (*OnSelected)();
	//using OnSelected = void (*)();

	MenuItem(Asset* _Asset, OnSelected onSelected)
		: onSelected(onSelected)
	{
		// 메뉴 텍스트 문자열 복사.
		menuText = _Asset;
	}

	~MenuItem()
	{
		//SafeDeleteArray(menuText);
	}

	// 메뉴 텍스트를 에셋으로 받음
	Asset* menuText = nullptr;

	// 메뉴 선택 시 실행할 동작.
	OnSelected onSelected = nullptr;
};

class MenuLevel : public Level
{
	RTTI_DECLARATIONS(MenuLevel, Level)

public:
	MenuLevel();
	~MenuLevel();

	void AssetLoading(const char* _fileName);

	virtual void Tick(float deltaTime) override;
	virtual void Render() override;

private:

	// 현재 선택된 아이템의 인덱스.
	int currentIndex = 0;

	// 아이템 선택 색상.
	Color selectedColor = Color::Green;

	// 아이템 선택 안됐을 때 색상.
	Color unselectedColor = Color::White;

	// 아이템 배열.
	std::vector<MenuItem*> items;

	// 메뉴 아이템 수.
	int length = 0;

	// 이미지 에셋을 미리 로딩 해놓을 컨테이너
	std::vector<Asset*> assets;
};