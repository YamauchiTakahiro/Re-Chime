#include "stdafx.h"
#include "UI.h"
#include "Source/Actor/Character/Player/Player.h"
#include "Game.h"

UI::UI()
{
	m_HPBar.Init("Assets/UIData/HPFrame.DDs", 1024.0f, 128.0f);
	m_HPBar.SetPosition(Vector3(0.0f, -382.0f, 0.0f));
	m_HPBar.SetScale(Vector3(0.38f, 0.59f, 0.5f));
	m_HPBar.Update();

	m_HP.Init("Assets/UIData/HPBar.DDs", 1024.0f, 128.0f);
	m_HP.SetPosition(Vector3(-115.0f, -380.0f, 0.0f));
	m_HP.SetScale(Vector3(0.41f, 3.0f, 0.5f));
	m_HP.SetPivot(Vector2(0.0f, 0.5f));
	m_HP.Update();

	m_Gear.Init("Assets/UIData/gear.DDs", 128.0f, 128.0f);
	m_Gear.SetPosition(Vector3(-875.0f, -450.0f, 0.0f));
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

	m_UP.Init("Assets/UIData/ATKUP.DDs", 50.0f, 50.0f);
	m_UP.SetPosition(Vector3(675.0f, 450.0f, 0.0f));
	m_UP.SetScale(Vector3(3.0f, 3.0f, 0.0f));
	//m_UP.SetPivot(Vector2(0.0f, 0.5f));
	m_UP.Update();

	m_AttackSpeed.Init("Assets/UIData/ATKSPDUP.DDs", 50.0f, 50.0f);
	m_AttackSpeed.SetPosition(Vector3(825.0f, 450.0f, 0.0f));
	m_AttackSpeed.SetScale(Vector3(3.0f, 3.0f, 0.0f));
	m_AttackSpeed.Update();

	m_inventory.Init("Assets/UIData/Inventory.DDs", 128.0f, 45.0f);
	m_inventory.SetPosition(Vector3(-893.0f, 0.0f, 0.0f));
	m_inventory.SetScale(Vector3(3.0f, 3.0f, 0.0f));
	//画像の向きを変える
	m_inventoryRotation.SetRotationZ(45.55f);
	m_inventory.SetRotation(m_inventoryRotation);
	m_inventory.Update();

	m_Inventoryback.Init("Assets/sprite/pause.DDs", 1920.0f, 1080.0f);

	m_pickUpText.SetText(L"A:拾う");
	m_pickUpText.SetPosition({ -75.0f, 300.0f, 0.0f });
	m_pickUpText.SetScale(1.0f);
	m_pickUpText.SetColor(g_vec4White);

	//攻撃スピードアップ
	m_PS1.Init("Assets/UIData/ps1.DDs", 72.0f, 100.0f);
	m_PS1.SetPosition(Vector3(-893.0f, 125.0f, 0.0f));
	//m_PS1.SetScale(Vector3(3.0f, 3.0f, 0.0f));
	m_PS1.Update();

	//攻撃力アップ
	m_PS2.Init("Assets/UIData/ps2.DDs", 72.0f, 100.0f);
	m_PS2.SetPosition(Vector3(-893.0f, 0.0f, 0.0f));
	m_PS2.Update();
	
	//回復
	m_PS3.Init("Assets/UIData/ps3.DDs", 72.0f, 100.0f);
	m_PS3.SetPosition(Vector3(-893.0f, -125.0f, 0.0f));
	m_PS3.Update();

	m_selectFrame.Init("Assets/UIData/SelectFrame.DDs", 132.0f, 128.0f);
	m_selectFrame.SetPosition(Vector3(-893.0f, 125.0f, 0.0f));
	m_selectFrame.Update();
}

UI::~UI()
{
}

bool UI::Start()
{
	m_player = FindGO<Player>("player");
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

	if (g_pad[0]->IsTrigger(enButtonSelect))
	{
		m_isInventoryOpen = !m_isInventoryOpen;

		m_game->SetGameStop(m_isInventoryOpen);
	}

	if (m_isInventoryOpen && g_pad[0]->IsTrigger(enButtonB))
	{
		m_isInventoryOpen = false;
		m_game->SetGameStop(false);
	}

	if (m_isInventoryOpen)
	{
		if (g_pad[0]->IsTrigger(enButtonX))
		{
			m_player->UseItem(m_selectItem);
		}
	}

	int nowHP = 0;
	int MaxHP = 0;

	nowHP = m_player->GetHP();
	MaxHP = m_player->GetMaxHP();
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
		m_HP.SetMulColor(g_vec4Green);
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
	// 点滅制御
	m_blinkTimer += g_gameTime->GetFrameDeltaTime();
	if (m_blinkTimer > 0.2f)
	{
		m_blinkTimer = 0.0f;
		m_isBlinkOn = !m_isBlinkOn;
	}

	int gireCount = m_player->GetGireCount();

	int needCount = m_game->GetNeedGireCount();

	wchar_t text[256];
	swprintf_s(text, L"%d/%d", gireCount , needCount);
	m_GireText.SetPosition(Vector3(-800.0f, -410.0f, 0.0f));
	m_GireText.SetScale(2.0f);
	m_GireText.SetText(text);
	m_GireText.SetColor(g_vec4Black);

	//クールタイム表示
	float coolTime = m_player->GetCoolTime();

	// クールタイムが始まったら表示
	if (coolTime > 0.0f)
	{
		m_isShowCoolTime = true;
	}

	// クールタイム終了で非表示
	if (coolTime <= 0.0f)
	{
		m_isShowCoolTime = false;
	}

	if (m_isShowCoolTime)
	{
		wchar_t coolText[256];

		swprintf_s(coolText, L"ATKCL:%.1f", coolTime);

		m_CoolTimeText.SetPosition(Vector3(640.0f, -450.0f, 0.0f));
		m_CoolTimeText.SetScale(1.4f);
		m_CoolTimeText.SetText(coolText);
		m_CoolTimeText.SetColor(g_vec4White);
	}
	else
	{
		m_CoolTimeText.SetText(L"");
	}
	if (m_isInventoryOpen)
	{
		Inventory();
	}
}

void UI::Inventory()
{
	if (!m_isInventoryOpen)
	{
		return;
	}

	if (g_pad[0]->IsTrigger(enButtonUp))
	{
		m_selectItem--;

		if (m_selectItem < 0)
		{
			m_selectItem = 2;
		}
	}

	if (g_pad[0]->IsTrigger(enButtonDown))
	{
		m_selectItem++;

		if (m_selectItem > 2)
		{
			m_selectItem = 0;
		}
	}

	Vector3 framePos;

	switch (m_selectItem)
	{
	case 0:
		framePos = Vector3(-893.0f, 125.0f, 0.0f);
		break;

	case 1:
		framePos = Vector3(-893.0f, 0.0f, 0.0f);
		break;

	case 2:
		framePos = Vector3(-893.0f, -125.0f, 0.0f);
		break;
	}

	m_selectFrame.SetPosition(framePos);
	m_selectFrame.Update();

	wchar_t text[64];

	swprintf_s(text, L"x%d", m_player->GetAttackSpeedPotionCount());

	m_PS3CountText.SetText(text);
	m_PS3CountText.SetPosition(Vector3(-820.0f, 125.0f, 0.0f));
	m_PS3CountText.SetScale(1.0f);

	swprintf_s(text, L"x%d", m_player->GetPowerPotionCount());

	m_PS2CountText.SetText(text);
	m_PS2CountText.SetPosition(Vector3(-820.0f, 0.0f, 0.0f));
	m_PS2CountText.SetScale(1.0f);

	swprintf_s(text, L"x%d", m_player->GetHealPotionCount());

	m_PS1CountText.SetText(text);
	m_PS1CountText.SetPosition(Vector3(-820.0f, -125.0f, 0.0f));
	m_PS1CountText.SetScale(1.0f);
}

void UI::Render(RenderContext& rc)
{
	if (!m_isVisible)
	{
		return;
	}
	m_HPBar.Draw(rc);
	m_HP.Draw(rc);
	m_Gear.Draw(rc);
	m_Abutton.Draw(rc);
	m_Bbutton.Draw(rc);
	m_Xbutton.Draw(rc);
	m_Ybutton.Draw(rc);
	m_GireText.Draw(rc);
	// 攻撃力バフ
	if (m_player->GetPowerBuffFlag())
	{
		float time = m_player->GetPowerBuffTime();

		if (time < 5.0f)
		{
			if (m_isBlinkOn)
			{
				m_UP.Draw(rc);
			}
		}
		else
		{
			m_UP.Draw(rc);
		}
	}

	// 攻撃速度バフ
	if (m_player->GetAttackSpeedBuffFlag())
	{
		float time = m_player->GetAttackSpeedBuffTime();

		if (time < 5.0f)
		{
			if (m_isBlinkOn)
			{
				m_AttackSpeed.Draw(rc);
			}
		}
		else
		{
			m_AttackSpeed.Draw(rc);
		}
	}
	m_CoolTimeText.Draw(rc);
	if (m_player->IsNearItem())
	{
		m_pickUpText.Draw(rc);
	}

	if (m_isInventoryOpen)
	{
		m_Inventoryback.SetMulColor({ 1.0f, 1.0f, 1.0f, 0.4f });
		m_Inventoryback.Draw(rc);
		m_inventory.Draw(rc);
		m_PS1.Draw(rc);
		m_PS2.Draw(rc);
		m_PS3.Draw(rc);
		m_PS1CountText.Draw(rc);
		m_PS2CountText.Draw(rc);
		m_PS3CountText.Draw(rc);
		m_selectFrame.Draw(rc);
	}
}