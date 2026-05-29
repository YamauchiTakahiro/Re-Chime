#pragma once

class TimerManager : public IGameObject
{
public:
	bool Start();
	void Update();
	void Render(RenderContext& rc);

	float DamageInterval();
	float AttackCoolTime();

private:
	float m_damageIntarvalTime = 0.0f;	//ダメージのインターバルタイム。
	float m_attackCoolTime = 0.0f;	//攻撃のクールタイム。
};