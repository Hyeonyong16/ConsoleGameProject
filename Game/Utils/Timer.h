#pragma once

class Timer
{
public:
	Timer(float targetTime = 0.0f);

	// 초시계의 시간이 흘러가도록 하기 위해 필요함.
	void Tick(float deltaTime);

	// 리셋 함수.
	void Reset();

	// 설정한 시간이 경과했는지 확인하는 함수.
	bool IsTimeout() const;

	// 타이머 시간 변경 함수.
	void SetTargetTime(float newTargetTime);
	inline float GetTargetTime() const { return targetTime; }

	inline float GetElapsedTime() const { return elapsedTime; }

private:
	float elapsedTime = 0.0f;
	float targetTime = 0.0f;
};