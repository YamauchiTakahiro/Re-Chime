#include "stdafx.h"
#include "UI.h"
#include "Source/Actor/Character/Player/Player.h"
#include "Game.h"
#include "Source/UIBase/DifficultyLevel/DifficultyLevel.h"
#include "Source/Manager/AudioManager/AudioManager.h"
#include "CoolRing.h"
#include "Source/Camera/GameCamera.h"

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

	m_Abutton.Init("Assets/UIData/A.DDs", 50.0f, 50.0f);
	m_Abutton.SetPosition(Vector3(700.0f, -380.0f, 0.0f));
	m_Abutton.SetScale(Vector3(1.0f, 1.0f, 1.0f));
	m_Abutton.Update();

	m_Bbutton.Init("Assets/UIData/B.DDs", 50.0f, 50.0f);
	m_Bbutton.SetPosition(Vector3(730.0f, -350.0f, 0.0f));
	m_Bbutton.SetScale(Vector3(1.0f, 1.0f, 1.0f));
	m_Bbutton.Update();

	m_Xbutton.Init("Assets/UIData/X.DDs", 50.0f, 50.0f);
	m_Xbutton.SetPosition(Vector3(670.0f, -350.0f, 0.0f));
	m_Xbutton.SetScale(Vector3(1.0f, 1.0f, 1.0f));
	m_Xbutton.Update();

	m_game = FindGO<Game>("game");

	m_UP.Init("Assets/UIData/ATKUP.DDs", 50.0f, 50.0f);
	m_UP.SetPosition(Vector3(-145.0f, -325.0f, 0.0f));
	m_UP.SetScale(Vector3(1.5f, 1.5f, 0.0f));
	m_UP.Update();

	m_AttackSpeed.Init("Assets/UIData/ATKSPDUP.DDs", 50.0f, 50.0f);
	m_AttackSpeed.SetPosition(Vector3(-75.0f, -325.0f, 0.0f));
	m_AttackSpeed.SetScale(Vector3(1.5f, 1.5f, 0.0f));
	m_AttackSpeed.Update();

	m_inventory.Init("Assets/UIData/Inventory.DDs", 128.0f, 45.0f);
	m_inventory.SetPosition(Vector3(-893.0f, 0.0f, 0.0f));
	m_inventory.SetScale(Vector3(3.0f, 3.0f, 0.0f));
	m_inventoryRotation.SetRotationZ(45.55f);
	m_inventory.SetRotation(m_inventoryRotation);
	m_inventory.Update();

	m_Inventoryback.Init("Assets/sprite/pause.DDs", 1920.0f, 1080.0f);

	m_pickUpText.SetText(L"A:拾う");
	m_pickUpText.SetPosition({ -75.0f, 300.0f, 0.0f });
	m_pickUpText.SetScale(1.0f);
	m_pickUpText.SetColor(g_vec4White);

	m_PS1.Init("Assets/UIData/ps1.DDs", 72.0f, 100.0f);
	m_PS1.SetPosition(Vector3(-893.0f, 125.0f, 0.0f));
	m_PS1.Update();

	m_PS2.Init("Assets/UIData/ps2.DDs", 72.0f, 100.0f);
	m_PS2.SetPosition(Vector3(-893.0f, 0.0f, 0.0f));
	m_PS2.Update();
	
	m_PS3.Init("Assets/UIData/ps3.DDs", 72.0f, 100.0f);
	m_PS3.SetPosition(Vector3(-893.0f, -125.0f, 0.0f));
	m_PS3.Update();

	m_selectFrame.Init("Assets/UIData/SelectFrame.DDs", 132.0f, 128.0f);
	m_selectFrame.SetPosition(Vector3(-893.0f, 125.0f, 0.0f));
	m_selectFrame.Update();

	m_LButton.Init("Assets/UIData/LButton.DDs", 75.0f, 50.0f);
	m_LButton.SetPosition(Vector3(860.0f, -210.0f, 0.0f));
	m_LButton.Update();

	m_Guardsmark.Init("Assets/UIData/guardsmark.DDs", 75.0f, 75.0f);
	m_Guardsmark.SetPosition(Vector3(800.0f, -210.0f, 0.0f));
	m_Guardsmark.Update();

	m_RButton.Init("Assets/UIData/RButton.DDs", 75.0f, 50.0f);
	m_RButton.SetPosition(Vector3(860.0f, -120.0f, 0.0f));
	m_RButton.Update();

	m_RunMark.Init("Assets/UIData/RunMark.DDs", 75.0f, 75.0f);
	m_RunMark.SetPosition(Vector3(800.0f, -120.0f, 0.0f));
	m_RunMark.Update();

	m_coolRing.Init();
	m_coolRing.SetPosition(Vector3(700.0f, -420.0f, 0.0f));
	m_coolRing.SetScale(Vector3(0.66f, 0.66f, 1.0f));

	m_guardRing.Init();
	m_guardRing.SetPosition(Vector3(800.0f, -210.0f, 0.0f));
	m_guardRing.SetScale(Vector3(0.65f, 0.65f, 1.0f));

	m_goalText.SetScale(2.0f);
	m_goalText.SetPosition(Vector3(-200.0f, 500.0f, 0.0f));

	m_pickItemText.SetScale(0.1f);
	m_pickItemText.SetPosition(Vector3(-1000.0f, 250.0f, 0.0f));
	m_pickItemText.SetColor(1.0f, 0.9f, 0.3f, 1.0f);

	Quaternion staminaRot;
	staminaRot.SetRotationZ(89.5f);

	m_StaminaBar.Init("Assets/UIData/StaminaFrame.DDs", 1024.0f, 128.0f);
	m_StaminaBar.SetPosition(Vector3(235.0f, -68.0f, 0.0f));
	m_StaminaBar.SetScale(Vector3(0.28f, 0.28f, 0.5f));
	m_StaminaBar.SetRotation(staminaRot);
	m_StaminaBar.Update();

	m_Stamina.Init("Assets/UIData/StaminaBar.DDs",1024.0f,128.0f);
	m_Stamina.SetPosition(Vector3(230.0f, -213.0f, 0.0f));
	m_Stamina.SetScale(Vector3(0.41f, 3.0f, 0.5f));
	m_Stamina.SetPivot(Vector2(0.0f, 0.5f));
	m_Stamina.SetRotation(staminaRot);
	m_Stamina.Update();

	m_tackleRing.Init();
	m_tackleRing.SetPosition(Vector3(630.0f, -350.0f, 0.0f));
	m_tackleRing.SetScale(Vector3(0.67f, 0.67f, 1.0f));

	m_Attack.Init("Assets/UIData/AttackUI.DDs", 75.0f, 75.0f);
	m_Attack.SetPosition(Vector3(700.0f, -420.0f, 0.0f));
	m_Attack.SetScale(Vector3(1.2f, 1.2f, 1.2f));
	m_Attack.Update();

	m_Tackle.Init("Assets/UIData/TackleUI.DDs" ,75.0f, 75.0f);
	m_Tackle.SetPosition(Vector3(630.0f, -350.0f, 0.0f));
	m_Tackle.SetScale(Vector3(1.2f, 1.2f, 1.0f));
	m_Tackle.Update();

	m_Jump.Init("Assets/UIData/JumpUI.DDs", 75.0f, 75.0f);
	m_Jump.SetPosition(Vector3(770.0f, -350.0f, 0.0f));
	m_Jump.SetScale(Vector3(1.2f, 1.2f, 1.0f));
	m_Jump.Update();

	m_description.Init("Assets/UIData/Description.DDs", 75.0f, 75.0f);
	m_description.SetPosition(Vector3(0.0f, -180.0f, 0.0f));
	m_description.SetScale(Vector3(15.0f, 3.0f, 0.0f));
	m_description.Update();

	m_SkipText.SetText(L"Y：スキップ");
	m_SkipText.SetPosition(Vector3(650.0f, -450.0f, 0.0f)); // 右下
	m_SkipText.SetScale(1.2f);
	m_SkipText.SetColor(g_vec4White);

	m_ItemDescriptionText.SetPosition(Vector3(-500.0f, -100.0f, 0.0f));
	m_ItemDescriptionText.SetScale(2.0f);
	m_ItemDescriptionText.SetColor(g_vec4White);

	m_audioManager = FindGO<AudioManager>("audioManager");
	GameCamera* camera = FindGO<GameCamera>("gameCamera");

	bool isHideUI = m_game->IsFade() || m_game->GetIntro() || m_game->GetBossIntro() || camera->IsCameraTransition();
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
			if (m_isInventoryOpen)
			{
				m_audioManager->PlaySE(enSound_OpenSE);
			}
			else
			{
				m_audioManager->PlaySE(enSound_CloseSE);
			}
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

	Difficulty difficulty = m_game->GetDifficulty();

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
		m_inventoryUseCoolTime -= g_gameTime->GetFrameDeltaTime();

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

	float stamina = m_player->GetStamina();
	float maxStamina = m_player->GetMaxStamina();

	float staminaRate = stamina / maxStamina;

	Vector3 staminaScale = {0.28f,0.28f,0.5f};

	staminaScale.x *= staminaRate;

	m_Stamina.SetScale(staminaScale);

	if (stamina <= maxStamina * 0.25f)
	{
		// 25%以下
		m_Stamina.SetMulColor(g_vec4Red);
	}
	else if (stamina <= maxStamina * 0.5f)
	{
		// 50%以下
		m_Stamina.SetMulColor(g_vec4Yellow);
	}
	else
	{
		// 50%以上
		m_Stamina.SetMulColor(Vector4(0.2f, 0.8f, 1.0f, 1.0f));
	}

	m_Stamina.Update();

	if (m_player->GetCoolTime() > 0.0f)
	{
		m_Attack.SetMulColor(Vector4(0.3f, 0.3f, 0.3f, 1.0f));
	}
	else
	{
		m_Attack.SetMulColor(g_vec4White);
	}

	if (g_pad[0]->IsPress(enButtonB))
	{
		m_Jump.SetMulColor(g_vec4Gray);
	}
	else
	{
		m_Jump.SetMulColor(g_vec4White);
	}

	if (m_player->GetTackleCoolTime() > 0.0f)
	{
		m_Tackle.SetMulColor(Vector4(0.3f,0.3f,0.3f,1.0f));
	}
	else if (g_pad[0]->IsPress(enButtonX))
	{
		m_Tackle.SetMulColor(g_vec4Gray);
	}
	else
	{
		m_Tackle.SetMulColor(g_vec4White);
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
	float maxTime = m_player->GetCoolTimeMax();

	float rate = 0.0f;

	if (maxTime > 0.0f)
	{
		rate = coolTime / maxTime;
	}

	m_coolRing.SetProgress(rate);

	// クールタイムが始まったら表示
	m_isShowCoolTime = (coolTime > 0.0f);

	if (m_isShowCoolTime)
	{
		wchar_t coolText[256];

		swprintf_s(coolText,L"%.1f",coolTime);

		m_CoolTimeText.SetPosition(Vector3(665.0f, -400.0f, 0.0f));
		m_CoolTimeText.SetScale(0.95f);
		m_CoolTimeText.SetText(coolText);
		m_CoolTimeText.SetColor(g_vec4White);
	}
	else
	{	
		m_CoolTimeText.SetText(L"");
	}

	float tackleCoolTime = m_player->GetTackleCoolTime();

	if (tackleCoolTime > 0.0f)
	{
		wchar_t tackleText[64];

		swprintf_s(tackleText,L"%.1f",tackleCoolTime);

		m_TackleCoolTimeText.SetText(tackleText);
		m_TackleCoolTimeText.SetPosition(Vector3(600.0f, -332.5f, 0.0f));
		m_TackleCoolTimeText.SetScale(0.8f);
		m_TackleCoolTimeText.SetColor(g_vec4White);
	}
	else
	{
		m_TackleCoolTimeText.SetText(L"");
	}

	float tackleRate = tackleCoolTime / m_player->GetTackleCoolTimeMax();
	m_tackleRing.SetProgress(tackleRate);

	float itemCool = m_inventoryUseCoolTime;

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

	float guardCoolTime =m_player->GetGuardTimeLimit();

	//bool isShowGuardCoolTime =(guardCoolTime > 0.0f);

	if (guardCoolTime < m_player->GetGuardTimeLimitMax())
	{
		wchar_t guardText[64];

		swprintf_s(guardText,L"%.1f",guardCoolTime);

		m_GuardCoolTimeText.SetText(guardText);
		m_GuardCoolTimeText.SetPosition(Vector3(770.0f, -190.0f, 0.0f));
		m_GuardCoolTimeText.SetScale(0.8f);
		m_GuardCoolTimeText.SetColor(g_vec4White);
	}
	else
	{
		m_GuardCoolTimeText.SetText(L"");
	}

	float guardRate = 1.0f - (guardCoolTime / m_player->GetGuardTimeLimitMax());

	m_guardRing.SetProgress(guardRate);

	if (m_isShowGoal)
	{
		m_goalTimer +=g_gameTime->GetFrameDeltaTime();

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
		m_goalText.SetText(m_goalMessage.c_str());

		m_goalText.SetColor(0.0f, 0.0f, 0.0f, m_goalFade);
    }

	if (m_isShowPickItem)
	{
		m_pickItemTimer += g_gameTime->GetFrameDeltaTime();

		// 少し上へ移動
		Vector3 pos = Vector3(300.0f, 300.0f + m_pickItemTimer * 30.0f, 0.0f);

		m_pickItemText.SetPosition(pos);

		// 2秒後からフェード
		if (m_pickItemTimer >= 2.0f)
		{
			m_pickItemFade -= g_gameTime->GetFrameDeltaTime() * 2.0f;
		}

		if (m_pickItemFade <= 0.0f)
		{
			m_pickItemFade = 0.0f;
			m_isShowPickItem = false;
		}

		m_pickItemText.SetText(m_pickItemMessage.c_str());

		// 金色っぽい色
		m_pickItemText.SetColor(1.0f, 0.9f, 0.3f, m_pickItemFade);
	}

	if (m_player->GetGuardTimeLimit() < 3.0f)
	{
		m_Guardsmark.SetMulColor(Vector4(0.3f, 0.3f, 0.3f, 1.0f));
	}
	else if (g_pad[0]->IsPress(enButtonLB1))
	{
		m_Guardsmark.SetMulColor(g_vec4Gray);
	}
	else
	{
		m_Guardsmark.SetMulColor(g_vec4White);
	}

	// ダッシュボタン演出
	if (g_pad[0]->IsPress(enButtonRB1))
	{
		m_RunMark.SetScale(Vector3(1.1f, 1.1f, 1.0f));
	}
	else
	{
		m_RunMark.SetScale(Vector3(1.2f, 1.2f, 1.0f));
	}

	// 実際にダッシュ中ならアイコンを強調
	if (m_player->IsDashing())
	{
		m_RunMark.SetMulColor(g_vec4Gray);
	}
	else
	{
		m_RunMark.SetMulColor(g_vec4White);
	}

	if (m_player->GetStamina() <= 0.0f)
	{
		m_RunMark.SetMulColor(Vector4(0.3f, 0.3f, 0.3f, 1.0f));
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
		m_cursorPos = Vector3(-980.0f, 125.0f, 0.0f);
		break;

	case 1:
		framePos = Vector3(-893.0f, 0.0f, 0.0f);
		m_cursorPos = Vector3(-980.0f, 0.0f, 0.0f);
		break;

	case 2:
		framePos = Vector3(-893.0f, -125.0f, 0.0f);
		m_cursorPos = Vector3(-980.0f, -125.0f, 0.0f);
		break;
	}

	m_selectFrame.SetPosition(framePos);
	m_selectFrame.Update();

	if (m_isUseItem)
	{
		bool useSuccess = m_player->UseItem(m_selectItem);

		if (useSuccess)
		{
			if (m_audioManager)
			{
				m_audioManager->PlaySE(enSound_DecisionSE);
			}

			if (m_game)
			{
				m_inventoryUseCoolTime = m_game->GetInventoryCoolTime();
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

	const wchar_t* description = L"";

	switch (m_selectItem)
	{
	case 0:
		description = L"速度薬\n"
			L"一定時間攻撃感覚短縮";
		m_ItemDescriptionText.SetColor(g_vec4Blue);
		break;

	case 1:
		description = L"攻撃薬\n"
			          L"一定時間攻撃力UP";
		m_ItemDescriptionText.SetColor(g_vec4Red);
		break;

	case 2:
		description = L"回復薬\n"
			L"HPを回復";
		m_ItemDescriptionText.SetColor(g_vec4Green);
		break;
	}
	m_ItemDescriptionText.SetText(description);
}

void UI::ShowGoal(const wchar_t* text)
{
	m_goalMessage = text;

	m_goalFade = 1.0f;
	m_goalTimer = 0.0f;
	m_isShowGoal = true;
}

void UI::ShowPickItem(const wchar_t* text)
{
	m_pickItemMessage = text;

	m_pickItemTimer = 0.0f;
	m_pickItemFade = 1.0f;
	m_isShowPickItem = true;

	m_pickItemText.SetScale(1.0f);
}

void UI::Render(RenderContext& rc)
{
	if (!m_isVisible)
	{
		return;
	}
	GameCamera* camera = FindGO<GameCamera>("gameCamera");

	bool isHideUI = m_game->IsFade() || camera->IsPlayingEventCamera();

	bool isEvent = m_game->GetIntro() || m_game->GetBossIntro() || camera->IsCameraTransition();

	if (!isHideUI)
	{
		m_HPBar.Draw(rc);
		m_HP.Draw(rc);
		m_Gear.Draw(rc);
		m_Attack.Draw(rc);
		if (m_player->GetCoolTime() > 0.0f)
		{
			m_coolRing.Draw(rc);
		}
		m_Abutton.Draw(rc);
		m_Jump.Draw(rc);
		m_Bbutton.Draw(rc);
		m_Tackle.Draw(rc);
		if (m_player->GetTackleCoolTime() > 0.0f)
		{
			m_tackleRing.Draw(rc);
		}
		m_Xbutton.Draw(rc);
		m_TackleCoolTimeText.Draw(rc);
		m_GireText.Draw(rc);
		m_Guardsmark.Draw(rc);
		m_GuardCoolTimeText.Draw(rc);
		m_guardRing.Draw(rc);
		m_LButton.Draw(rc);
		m_RunMark.Draw(rc);
		m_RButton.Draw(rc);
		if (m_player->IsDashing() || m_player->GetStamina() < m_player->GetMaxStamina())
		{
			m_StaminaBar.Draw(rc);
			m_Stamina.Draw(rc);
		}

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
			m_ItemDescriptionText.Draw(rc);
			m_description.Draw(rc);
		}
	}

	if (m_isShowPickItem)
	{
		m_pickItemText.Draw(rc);
	}

	if (m_isShowGoal)
	{
		m_goalText.Draw(rc);
	}

	if (isEvent && m_isBlinkOn)
	{
		m_SkipText.Draw(rc);
	}
}