#pragma once

class Timer
{
public:
	Timer(float targetTime = 0.0f);

	// �ʽð��� �ð��� �귯������ �ϱ� ���� �ʿ���.
	void Tick(float deltaTime);

	// ���� �Լ�.
	void Reset();

	// ������ �ð��� ����ߴ��� Ȯ���ϴ� �Լ�.
	bool IsTimeout() const;

	// Ÿ�̸� �ð� ���� �Լ�.
	void SetTargetTime(float newTargetTime);
	inline float GetTargetTime() const { return targetTime; }

	inline float GetElapsedTime() const { return elapsedTime; }

private:
	float elapsedTime = 0.0f;
	float targetTime = 0.0f;
};