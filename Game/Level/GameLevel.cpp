#include "GameLevel.h"
#include "Actor/Player.h"

// Todo:
// - 맵 파일 넣기 및 불러오기
//		- 하나의 배열로 위치에 따른 오브젝트 관리
//			- 해당 위치에 actor가 있는지 확인할려고 actor 를 다 도는건 너무 비효율적 (레이캐스팅 해야하기 때문에)
//		- 각 위치에 따른 깊이버퍼 만들기
// - 카메라 만들기
//		- 문자열로 Screen 설정할 것
//		- 해당 문자열을 출력해서 화면 그릴것 (커서를 처음으로 돌리고 출력 반복)
//		- 조건문으로 그냥 콘솔창 보는거랑 1인칭 만들기
// - 벽 생성 및 플레이어 충돌 설정
// - DDA 알고리즘을 통한 플레이어 정면 방향 레이캐스팅
//		- 충돌하는 벽 검출하기
//	
//

GameLevel::GameLevel()
{
	// 플레이어 추가.
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
