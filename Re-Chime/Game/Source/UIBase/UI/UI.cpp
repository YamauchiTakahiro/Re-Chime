#include "stdafx.h"
#include "UI.h"
#include "Source/Actor/Character/Player/Player.h"
#include "Game.h"
#include "Source/UIBase/DifficultyLevel/DifficultyLevel.h"
#include "Source/Manager/AudioManager/AudioManager.h"

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
	m_Abutton.SetPosition(Vector3(700.0f, -410.0f, 0.0f));
	m_Abutton.SetScale(Vector3(1.2f, 1.2f, 1.2f));
	m_Abutton.Update();

	m_Bbutton.Init("Assets/UIData/B.DDs", 75.0f, 75.0f);
	m_Bbutton.SetPosition(Vector3(760.0f, -350.0f, 0.0f));
	m_Bbutton.SetScale(Vector3(1.2f, 1.2f, 1.0f));
	m_Bbutton.Update();

	m_Xbutton.Init("Assets/UIData/X.DDs", 75.0f, 75.0f);
	m_Xbutton.SetPosition(Vector3(640.0f, -350.0f, 0.0f));
	m_Xbutton.SetScale(Vector3(1.2f, 1.2f, 1.0f));
	m_Xbutton.Update();

	m_Ybutton.Init("Assets/UIData/Y.DDs", 75.0f, 75.0f);
	m_Ybutton.SetPosition(Vector3(700.0f, -290.0f, 0.0f));
	m_Ybutton.SetScale(Vector3(1.2f, 1.2f, 1.0f));
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

	m_goalText.SetScale(2.0f);
	m_goalText.SetPosition(Vector3(-200.0f, 500.0f, 0.0f));

	/*m_Cursor.SetText(L">");
	m_Cursor.SetScale(2.5f);*/

	m_player = FindGO<Player>("player");
	m_audioManager = FindGO<AudioManager>("audioManager");
	m_cursorPos = Vector3(0, 0, 0);
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

	if (!isPause && g_pad[0]->IsTrigger(enButtonSelect))
	{
		m_isInventoryOpen = !m_isInventoryOpen;

		if (m_audioManager)
		{
			m_audioManager->PlaySE(enSound_OpenSE);
		}
		else
		{
			m_audioManager->PlaySE(enSound_CloseSE);
		}

		m_game->SetGameStop(m_isInventoryOpen);
	}

	if (m_isInventoryOpen && g_pad[0]->IsTrigger(enButtonB))
	{
		m_isInventoryOpen = false;
		m_game->SetGameStop(false);
	}

	if (m_isInventoryOpen)
	{
		if (g_pad[0]->IsTrigger(enButtonX)&& m_inventoryUseCoolTime <= 0.0f)
		{
			if (m_audioManager)
			{
				m_audioManager->PlaySE(enSound_DecisionSE);
			}
			m_isUseItem = true;
		}
	}

	bool updateCoolTime = false;

	Difficulty difficulty =m_game->GetDifficulty();

	if (difficulty == EASY ||difficulty == NORMAL)
	{
		// Easy / Normal はインベントリ開いてても進む
		updateCoolTime = true;
	}
	else
	{
		// Hard / Lunatic は閉じてる時だけ進む
		updateCoolTime = !m_isInventoryOpen;
	}

	if (updateCoolTime)
	{
		m_inventoryUseCoolTime -=
			g_gameTime->GetFrameDeltaTime();

		if (m_inventoryUseCoolTime < 0.0f)
		{
			m_inventoryUseCoolTime = 0.0f;
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

	if (m_player->GetCoolTime() > 0.0f)
	{
		m_Abutton.SetMulColor(
			Vector4(0.3f, 0.3f, 0.3f, 1.0f));
	}
	else
	{
		m_Abutton.SetMulColor(g_vec4White);
	}

	if (g_pad[0]->IsPress(enButtonB))
	{
		m_Bbutton.SetMulColor(g_vec4Gray);
	}
	else
	{
		m_Bbutton.SetMulColor(g_vec4White);
	}

	if (g_pad[0]->IsPress(enButtonX))
	{
		m_Xbutton.SetMulColor(g_vec4Gray);
	}
	else
	{
		m_Xbutton.SetMulColor(g_vec4White);
	}

	if (g_pad[0]->IsPress(enButtonY))
	{
		m_Ybutton.SetMulColor(g_vec4Gray);
	}
	else
	{
		m_Ybutton.SetMulColor(g_vec4White);
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

		if (coolTime >= 1.0f)
		{
			swprintf_s(coolText, L"%.1d",(int)ceilf(coolTime));
		}
		else
		{
			swprintf_s(coolText, L"%.1f", coolTime);
		}

		m_CoolTimeText.SetPosition(Vector3(670.0f, -385.0f, 0.0f));
		m_CoolTimeText.SetScale(0.95f);
		m_CoolTimeText.SetText(coolText);
		m_CoolTimeText.SetColor(g_vec4White);
	}
	else
	{
		m_CoolTimeText.SetText(L"");
	}

	float itemCool =
		m_inventoryUseCoolTime;

	if (itemCool > 0.0f)
	{
		wchar_t itemText[256];

		swprintf_s(itemText,L"%.1f",itemCool
		);

		m_ItemCoolTimeText.SetPosition(Vector3(-960.0f,270.0f,0.0f));
		m_ItemCoolTimeText.SetScale(1.2f);
		m_ItemCoolTimeText.SetText(itemText);
		m_ItemCoolTimeText.SetColor(g_vec4White);
	}
	else
	{
		m_ItemCoolTimeText.SetText(L"");
	}

	if (m_isShowGoal)
	{
		m_goalTimer +=
			g_gameTime->GetFrameDeltaTime();

		// 最初の3秒は表示のみ
		if (m_goalTimer >= 3.0f)
		{
			m_goalFade -=g_gameTime->GetFrameDeltaTime()* 0.3f;
		}

		if (m_goalFade < 0.0f)
		{
			m_goalFade = 0.0f;
			m_isShowGoal = false;
		}
		m_goalText.SetText(
			m_goalMessage.c_str());

		m_goalText.SetColor(0.0f, 0.0f, 0.0f, m_goalFade);
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
		if (m_audioManager)
		{
			m_audioManager->PlaySE(enSound_ChoiceSE);
		}
	}

	if (g_pad[0]->IsTrigger(enButtonDown))
	{
		m_selectItem++;

		if (m_selectItem > 2)
		{
			m_selectItem = 0;
		}
		if (m_audioManager)
		{
			m_audioManager->PlaySE(enSound_ChoiceSE);
		}
	}

	Vector3 framePos;

	switch (m_selectItem)
	{
	case 0:
		framePos = Vector3(-893.0f, 125.0f, 0.0f);
		m_cursorPos =Vector3(-980.0f,125.0f,0.0f);
		break;

	case 1:
		framePos = Vector3(-893.0f, 0.0f, 0.0f);
		m_cursorPos =Vector3(-980.0f,0.0f,0.0f);
		break;

	case 2:
		framePos = Vector3(-893.0f, -125.0f, 0.0f);
		m_cursorPos =
			Vector3(
				-980.0f,
				-125.0f,0.0f);
		break;
	}

	m_selectFrame.SetPosition(framePos);
	m_selectFrame.Update();

	if (m_isUseItem)
	{
		bool useSuccess =m_player->UseItem(m_selectItem);

		if (useSuccess)
		{
			if (m_audioManager)
			{
				m_audioManager->PlaySE(enSound_DecisionSE);
			}

			if (m_game)
			{
				m_inventoryUseCoolTime =
					m_game->GetInventoryCoolTime();
			}
		}

		m_isUseItem = false;

		/*if (useSuccess == false)
		{
			m_audioManager->PlaySE(enSound_CancelSE);
		}*/
	}

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

void UI::ShowGoal(const wchar_t* text)
{
	m_goalMessage = text;

	m_goalFade = 1.0f;
	m_goalTimer = 0.0f;
	m_isShowGoal = true;
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
		m_ItemCoolTimeText.Draw(rc);
	}

	if (m_isShowGoal)
	{
		m_goalText.Draw(rc);
	}
}