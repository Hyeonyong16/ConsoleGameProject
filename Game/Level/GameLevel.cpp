#include "GameLevel.h"
#include "Actor/Player.h"

// Todo:
// - �� ���� �ֱ� �� �ҷ�����
//		- �ϳ��� �迭�� ��ġ�� ���� ������Ʈ ����
//			- �ش� ��ġ�� actor�� �ִ��� Ȯ���ҷ��� actor �� �� ���°� �ʹ� ��ȿ���� (����ĳ���� �ؾ��ϱ� ������)
//		- �� ��ġ�� ���� ���̹��� �����
// - ī�޶� �����
//		- ���ڿ��� Screen ������ ��
//		- �ش� ���ڿ��� ����ؼ� ȭ�� �׸��� (Ŀ���� ó������ ������ ��� �ݺ�)
//		- ���ǹ����� �׳� �ܼ�â ���°Ŷ� 1��Ī �����
// - �� ���� �� �÷��̾� �浹 ����
// - DDA �˰����� ���� �÷��̾� ���� ���� ����ĳ����
//		- �浹�ϴ� �� �����ϱ�
//	
//

GameLevel::GameLevel()
{
	// �÷��̾� �߰�.
	AddActor(new Player());
}

void GameLevel::BeginPlay()
{
	super::BeginPlay();

}

void GameLevel::Tick(float _deltaTime)
{
	super::Tick(_deltaTime);
}

void GameLevel::Render()
{
	super::Render();
}
