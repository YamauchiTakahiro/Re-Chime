#include "stdafx.h"
#include "CoolRing.h"

void CoolRing::Init()
{
	const char* files[RING_MAX] =
	{
		"Assets/UIData/CoolRing_0.DDS",
		"Assets/UIData/CoolRing_1.DDS",
		"Assets/UIData/CoolRing_2.DDS",
		"Assets/UIData/CoolRing_3.DDS",
		"Assets/UIData/CoolRing_4.DDS",
		"Assets/UIData/CoolRing_5.DDS",
		"Assets/UIData/CoolRing_6.DDS",
		"Assets/UIData/CoolRing_7.DDS",
		"Assets/UIData/CoolRing_8.DDS"
	};

	m_pos = Vector3(700.0f, -410.0f, 0.0f);
	m_scale = Vector3(0.65f, 0.65f, 1.0f);

	for (int i = 0; i < RING_MAX; i++)
	{
		m_ring[i].Init(files[i], 128.0f, 128.0f);

		m_ring[i].SetPosition(m_pos);
		m_ring[i].SetScale(m_scale);
		m_ring[i].Update();
	}
}

void CoolRing::SetPosition(const Vector3& pos)
{
	m_pos = pos;

	for (int i = 0; i < RING_MAX; i++)
	{
		m_ring[i].SetPosition(pos);
		m_ring[i].Update();
	}
}

void CoolRing::SetScale(const Vector3& scale)
{
	m_scale = scale;

	for (int i = 0; i < RING_MAX; i++)
	{
		m_ring[i].SetScale(scale);
		m_ring[i].Update();
	}
}

void CoolRing::SetProgress(float rate)
{
	if (rate < 0.0f)
	{
		rate = 0.0f;
	}

	if (rate > 1.0f)
	{
		rate = 1.0f;
	}

	// 0～8へ変換
	m_nowRing = (int)(rate * 8.0f + 0.5f);

	if (m_nowRing < 0)
	{
		m_nowRing = 0;
	}

	if (m_nowRing > 8)
	{
		m_nowRing = 8;
	}
}

void CoolRing::Draw(RenderContext& rc)
{
	m_ring[m_nowRing].Update();
	m_ring[m_nowRing].Draw(rc);
}