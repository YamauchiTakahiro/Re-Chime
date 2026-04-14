#include "stdafx.h"
#include "UI.h"
#include "Source/Actor/Character/Player/Player.h"
#include "Game.h"

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

	m_Gear.Init("Assets/UIData/gear.DDs", 128.0f, 128.0f);
	m_Gear.SetPosition(Vector3(750.0f, 450.0f, 0.0f));
	m_Gear.Update();

	m_Abutton.Init("Assets/UIData/A.DDs", 75.0f, 75.0f);
	m_Abutton.SetPosition(Vector3(700.0f, -400.0f, 0.0f));
	m_Abutton.Update();

	m_Bbutton.Init("Assets/UIData/B.DDs", 75.0f, 75.0f);
	m_Bbutton.SetPosition(Vector3(750.0f, -350.0f, 0.0f));
	m_Bbutton.Update();

	m_Xbutton.Init("Assets/UIData/X.DDs", 75.0f, 75.0f);
	m_Xbutton.SetPosition(Vector3(650.0f, -350.0f, 0.0f));
	m_Xbutton.Update();

	m_Ybutton.Init("Assets/UIData/Y.DDs", 75.0f, 75.0f);
	m_Ybutton.SetPosition(Vector3(700.0f, -300.0f, 0.0f));
	m_Ybutton.Update();

	m_game = FindGO<Game>("game");
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
	bool isPause = false;
	isPause = m_game->GetIsPause(isPause);
	if (isPause)
	{
		return;
	}

	int nowHP = 0;
	int MaxHP = 0;

	nowHP = m_player->GetHP(nowHP);
	MaxHP = m_player->GetMaxHP(MaxHP);
	float Wari = (float)nowHP / (float)MaxHP;
	Vector3 scale = { 0.28f, 0.28f, 0.5f };
	scale.x *= Wari;
	m_HP.SetScale(scale);
	if (nowHP <= MaxHP / 4)
	{
		m_HP.SetMulColor(g_vec4Red);
	}
	else
	{
		m_HP.SetMulColor(g_vec4White);
	}
	m_HP.Update();

	if (g_pad[0]->IsPress(enButtonA))
	{
		m_Abutton.SetMulColor(g_vec4White);
	}
	else
	{
		m_Abutton.SetMulColor(g_vec4Gray);
	}

	if (g_pad[0]->IsPress(enButtonB))
	{
		m_Bbutton.SetMulColor(g_vec4White);
	}
	else
	{
		m_Bbutton.SetMulColor(g_vec4Gray);
	}

	if (g_pad[0]->IsPress(enButtonX))
	{
		m_Xbutton.SetMulColor(g_vec4White);
	}
	else
	{
		m_Xbutton.SetMulColor(g_vec4Gray);
	}

	if (g_pad[0]->IsPress(enButtonY))
	{
		m_Ybutton.SetMulColor(g_vec4White);
	}
	else
	{
		m_Ybutton.SetMulColor(g_vec4Gray);
	}
}

void UI::Render(RenderContext& rc)
{
	m_HPBar.Draw(rc);
	m_HP.Draw(rc);
	m_Gear.Draw(rc);
	m_Abutton.Draw(rc);
	m_Bbutton.Draw(rc);
	m_Xbutton.Draw(rc);
	m_Ybutton.Draw(rc);
}