#include "stdafx.h"
#include "Source/Manager/TimerManager/TimerManager.h"

bool TimerManager::Start()
{
	return true;
}

void TimerManager::Update()
{
}

void TimerManager::Render(RenderContext& rc)
{
	(void)rc;
}

float TimerManager::DamageInterval()
{
	if (m_damageIntarvalTime > 0.0f)
	{
		m_damageIntarvalTime -= g_gameTime->GetFrameDeltaTime();
	}
	return m_damageIntarvalTime;
}

float TimerManager::AttackCoolTime()
{
	if (m_attackCoolTime > 0.0f)
	{
		m_attackCoolTime -= g_gameTime->GetFrameDeltaTime();
	}
	return m_attackCoolTime;
}