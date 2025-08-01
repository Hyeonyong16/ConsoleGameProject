#pragma once

#include "Core.h"
#include "Math/Vector2.h"
#include "Math/Color.h"
#include "RTTI.h"

// Actor ����
// 1. ��ġ ����
// 2. �ܼ� â�� �׸���(���/������)
// 3. ������ �̺�Ʈ �Լ� ȣ��(BeginPlay/Tick/Draw)

// ���漱��
class Level;

class Engine_API Actor : public RTTI		// �ֱ��� RTTI �� �ƴ� Object �̸����� ���� ���
{
	friend class Level;
	RTTI_DECLARATIONS(Actor, RTTI)			// ������ �ڼ�, �������� �θ�Ÿ�� ����

public:
	Actor(
		const char* _image = "",
		Color _color = Color::White,
		const Vector2& _position = Vector2::Zero
	);
	virtual ~Actor();

	// �̺�Ʈ �Լ�
	virtual void BeginPlay();				// ��ü �����ֱ�(Lifetime)�� 1���� ȣ��(�ʱ�ȭ ����)
	virtual void Tick(float _deltaTime);	// �����Ӹ��� ȣ�� (�ݺ��� �۾�/���Ӽ��� �ʿ��� �۾�)
	virtual void Render();					// �׸��� �Լ�

	// BeginPlay �Լ� ȣ�� ���� 
	inline bool HasBeganPlay() const { return hasBeganPlay; }

	// ��ġ ���� / �� �д� �Լ�
	void SetPosition(const Vector2& _newPosition);
	Vector2 GetPosition() const;

	// ���� ���� �Լ�
	void SetColor(const Color _color);

	int GetWidth() const;

	// SortingOrder ���� �Լ�
	// ���ڰ� Ŭ���� ī�޶� �����
	inline unsigned int GetSortingOrder() const { return sortingOrder; }
	void SetSortingOrder(unsigned int _sortingOrder);

	// ���ʽ� ����
	void SetOwner(Level* _newOwner);
	Level* GetOwner();

	// ID Ȯ��
	inline int GetID() const { return iD; }

	// �浹 Ȯ�� ��û �Լ�(������ AABB ����)
	bool TestIntersect(const Actor* const _other);
	
	// ��ü ���� �Լ�
	void Destroy();

	// ��ü�� �����Ǳ� ������ ȣ��Ǵ� �Լ�.
	virtual void OnDestroy();

	// ���� �ֱ� ���� �Լ�.
	void SetLifetime(float _newLifetime);

	// ���� ���� ��û �Լ�
	void QuitGame();

protected:
	Vector2 position;				// ��ü�� ��ġ
	char* image;				// �׸� ��
	int width = 0;					// ���ڿ� ����
	Color color;					// �ؽ�Ʈ ���� ��

	bool hasBeganPlay = false;		// BeginPlay ȣ���� �Ǿ����� Ȯ��

	unsigned int sortingOrder = 0;	// ���� ����

	bool isActive = true;			// ���Ͱ� Ȱ����������
	bool isExpired = false;			// ���� ��û�Ǿ����� 
	
	float lifetime = 0.0f;			// ���� �ֱ� (����: ��)
	bool autoDestroy = false;		// �� �÷��׸� true�� �����ϸ� ���� �ֱ⸦ ����� �ڵ� ����

	Level* owner = nullptr;			// ���� ����(���ʽ�)

	static int nextID;
	int iD = 1;
};

