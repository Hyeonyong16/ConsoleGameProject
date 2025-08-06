#pragma once

#include "Level/Level.h"
#include <vector>
#include "Actor/Actor.h"
#include "Asset/Asset.h"

struct MenuItem
{
	// �Լ� ������ ����.
	typedef void (*OnSelected)();
	//using OnSelected = void (*)();

	MenuItem(Asset* _Asset, OnSelected onSelected)
		: onSelected(onSelected)
	{
		// �޴� �ؽ�Ʈ ���ڿ� ����.
		menuText = _Asset;
	}

	~MenuItem()
	{
		//SafeDeleteArray(menuText);
	}

	// �޴� �ؽ�Ʈ�� �������� ����
	Asset* menuText = nullptr;

	// �޴� ���� �� ������ ����.
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

	// ���� ���õ� �������� �ε���.
	int currentIndex = 0;

	// ������ ���� ����.
	Color selectedColor = Color::Green;

	// ������ ���� �ȵ��� �� ����.
	Color unselectedColor = Color::White;

	// ������ �迭.
	std::vector<MenuItem*> items;

	// �޴� ������ ��.
	int length = 0;

	// �̹��� ������ �̸� �ε� �س��� �����̳�
	std::vector<Asset*> assets;
};