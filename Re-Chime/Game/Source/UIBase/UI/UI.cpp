#include "stdafx.h"
#include "UI.h"
#include "Source/Actor/Character/Player/Player.h"
UI::UI()
{
	m_HPBar.Init("Assets/UIData/HPBar.DDs", 1024.0f, 128.0f);
	m_HPBar.SetPosition(Vector3(0.0f, -380.0f, 0.0f));
	m_HPBar.SetScale(Vector3(0.5f, 3.0f, 0.5f));
	m_HPBar.Update();

	m_HP.Init("Assets/UIData/HP.DDs", 1024.0f, 128.0f);
	m_HP.SetPosition(Vector3(-115.0f, -367.7f, 0.0f));
	m_HP.SetScale(Vector3(0.41f, 3.0f, 0.5f));
	m_HP.SetPivot(Vector2(0.0f, 0.5f));
	m_HP.Update();
	m_player = FindGO<Player>("player");
}

UI::~UI()
{
}

bool UI::Start()
{
	return true;
}

void UI::Update()
{
	int nowHP = 0;
	nowHP = m_player->GetHP(nowHP);
	float Wari = (float)nowHP / (float)nowHP;
	Vector3 scale = { 0.28f, 0.28f, 0.5f };
	scale.x *= Wari;
	m_HP.SetScale(scale);
	if (nowHP <= nowHP / 4)
	{
		m_HP.SetMulColor(g_vec4Red);
	}
	else
	{
		m_HP.SetMulColor(g_vec4White);
	}
	m_HP.Update();
}

void UI::Render(RenderContext& rc)
{
	m_HPBar.Draw(rc);
	m_HP.Draw(rc);
}