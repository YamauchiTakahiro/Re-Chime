#include "stdafx.h"
#include "Game.h"
#include "Source/Actor/Character/Player/Player.h"
#include "source/Camera/GameCamera.h"
#include "Source/Actor/Stage/Stage.h"
#include "Source/Actor/Barrier/Barrier.h"
#include "Source/UIBase/Title/Title.h"
#include "Source/UIBase/UI/UI.h"
#include "Source/Actor/Character/Enemy/SmallRobot/SmallRobot.h"
#include "Source/Actor/Character/Enemy/FloorBoss/FloorBoss.h"
#include "Source/UIBase/GameOver/GameOver.h"
#include "Source/UIBase/DifficultyLevel/DifficultyLevel.h"
#include "Source/Actor/Item/Gire/Gire.h"
#include "Source/Actor/Character/Enemy/MediumRobot/MediumRobot.h"
#include "Source/Actor/Character/Enemy/FinalBoss/FinalBoss.h"
#include "Source/UIBase/GameClear/GameClear.h"
#include "Source/Sound/AudioManager/AudioManager.h"
#include "Source/Actor/Item/Potion/Heal/Heal.h"
#include "graphics/effect/EffectEmitter.h"

namespace
{
	float CalcCenterX(const wchar_t* text, float scale)
	{
		int len = wcslen(text);
		float width = len * 45.0f * scale; // ← ここ調整ポイント
		return 0.0f - width * 0.5f;
	}
}

Game::Game()
{
	//PhysicsWorld::GetInstance()->EnableDrawDebugWireFrame();
}

bool Game::Start()
{
	srand((unsigned)time(nullptr));
	m_Pause.Init("Assets/Sprite/pause.DDs", 1920.0f, 1080.0f);
	m_player = NewGO<Player>(0, "player");
	m_gameCamera = NewGO<GameCamera>(0, "gameCamera");
	m_ui = NewGO<UI>(0, "ui");
	/*m_heal = NewGO<Heal>(0, "heal");
	m_heal->SetPosition(Vector3(1000.0f, 0.0f, 0.0f));*/
	m_levelRender.Init("Assets/modelData/Level/ReChaim.tkl", [&](LevelObjectData& objData)
		{
			if (objData.EqualObjectName(L"Stage") == true)
			{
				m_stage = NewGO<Stage>(0, "stage");

				m_stage->SetPosition(objData.position);
			}
			/*else if (objData.EqualObjectName(L"smallRobot") == true)
			{
				auto smallRobot = NewGO<SmallRobot>(0, "smallRobot");

				smallRobot->SetPosition(objData.position);

				smallRobot->SetScale(objData.scale);

				m_smallRobot.push_back(smallRobot);

				m_enemyCount++;
			}*/
			/*else if (objData.EqualObjectName(L"FloorBoss") == true)
			{
				auto floorBoss = NewGO<FloorBoss>(0, "FloorBoss");

				floorBoss->SetPosition(objData.position);

				floorBoss->SetScale(objData.scale);

				m_floorBoss.push_back(floorBoss);

				m_enemyCount++;
			}*/
			else if (objData.EqualObjectName(L"mediumRobot") == true)
			{
				auto mediumRobot = NewGO<MediumRobot>(0, "mediumRobot");

				mediumRobot->SetPosition(objData.position);

				mediumRobot->SetScale(objData.scale);

				m_mediumRobot.push_back(mediumRobot);

				m_enemyCount++;
			}
			/*else if (objData.EqualObjectName(L"finalBoss") == true)
			{
				auto finalBoss = NewGO<FinalBoss>(0, "finalBoss");
				
				finalBoss->SetPosition(objData.position);
				
				finalBoss->SetScale(objData.scale);

				m_finalBoss.push_back(finalBoss);

				m_enemyCount++;
			}*/
			/*else if (objData.EqualObjectName(L"barrier1") == true)
			{
				m_barrier1 = NewGO<Barrier>(0, "barrier");
				m_barrier1->SetPosition(objData.position);
				m_barrier1->SetRotation(objData.rotation);
				m_barrier1->SetScale(objData.scale);
			}
			else if (objData.EqualObjectName(L"barrier2") == true)
			{
				m_barrier2 = NewGO<Barrier>(0, "barrier");
				m_barrier2->SetPosition(objData.position);
				m_barrier2->SetRotation(objData.rotation);
				m_barrier2->SetScale(objData.scale);
			}
			else if (objData.EqualObjectName(L"barrier3") == true)
			{
				m_barrier3 = NewGO<Barrier>(0, "barrier");
				m_barrier3->SetPosition(objData.position);

				m_barrier3->SetRotation(objData.rotation);

				m_barrier3->SetScale(objData.scale);
			}
			else if (objData.EqualObjectName(L"barrier4") == true)
			{
				m_barrier4 = NewGO<Barrier>(0, "barrier");
				m_barrier4->SetPosition(objData.position);
				m_barrier4->SetRotation(objData.rotation);
				m_barrier4->SetScale(objData.scale);
			}
			else if (objData.EqualObjectName(L"barrier5") == true)
			{
				m_barrier5 = NewGO<Barrier>(0, "barrier");
				m_barrier5->SetPosition(objData.position);
				m_barrier5->SetRotation(objData.rotation);
				m_barrier5->SetScale(objData.scale);
			}
			else if (objData.EqualObjectName(L"barrier6") == true)
			{
				m_barrier6 = NewGO<Barrier>(0, "barrier");
				m_barrier6->SetPosition(objData.position);
				m_barrier6->SetRotation(objData.rotation);
				m_barrier6->SetScale(objData.scale);
			}*/

			m_gire = NewGO<Gire>(0, "gire");
			m_gire->SetPosition(Vector3(0.0f, 7000.0f, 0.0f));

			EffectEngine::GetInstance()->ResistEffect(1, u"Assets/effect/efk/Heal.efk");
			EffectEngine::GetInstance()->ResistEffect(2, u"Assets/effect/efk/PowerBuff.efk");
			EffectEngine::GetInstance()->ResistEffect(3, u"Assets/effect/efk/AttackSpeedBuff.efk");
			EffectEngine::GetInstance()->ResistEffect(4, u"Assets/effect/efk/Explosion.efk");
			EffectEngine::GetInstance()->ResistEffect(5, u"Assets/effect/efk/BossExplosion.efk");

			return true;
	});

	m_audioManager = FindGO<AudioManager>("audioManager");
	if (m_audioManager)
	{
		m_audioManager->PlayBGM(enSound_StageBGM, 0.5f);
	}
	m_PlayerReturnText.SetText(L"ゲームに戻る");
	m_PlayerReturnText.SetScale(1.5f);

	m_SoundText.SetText(L"音量調節");
	m_SoundText.SetScale(1.5f);

	m_TitleReturnText.SetText(L"タイトル戻る");
	m_TitleReturnText.SetScale(1.5f);

	m_Cursor.SetText(L">");
	m_Cursor.SetScale(2.5f);

	m_cursorPos = Vector3(0, 0, 0);

	return true;
}

Game::~Game()
{
	const auto& smallRobots = FindGOs<SmallRobot>("smallRobot");
	for (auto smallRobot : smallRobots)
	{
		DeleteGO(smallRobot);
	}
	const auto& floorBosses = FindGOs<FloorBoss>("floorBoss");
	for (auto floorBoss : floorBosses)
	{
		DeleteGO(floorBoss);
	}
	const auto& mediumRobots = FindGOs<MediumRobot>("mediumRobot");
	for (auto mediumRobot : mediumRobots)
	{
		DeleteGO(mediumRobot);
	}
	const auto& finalBosses = FindGOs<FinalBoss>("finalBoss");
	for (auto finalBoss : finalBosses)
	{
		DeleteGO(finalBoss);
	}
	DeleteGO(m_player);
	DeleteGO(m_gameCamera);
	DeleteGO(m_stage);
	DeleteGO(m_ui);
}

void Game::Update()
{	
	m_pauseTime += 0.05f;

	Pause();

	if (m_isPause)
	{
		PauseRender();
		return;
	}

	//プレイヤーのHPが0以下になったらゲームオーバー。
	int hp = 0;
	int playerHP = m_player->GetHP(hp);
	if (playerHP <= 0)
	{
		NewGO<GameOver>(0, "GameOver");
		if (m_audioManager)
		{
			m_audioManager->StopBGM();
		}
		DeleteGO(this);
		return;
	}
	//if (g_pad[0]->IsTrigger(enButtonSelect))
	//{
	//	NewGO<GameClear>(0, "GameClear");
	//	if (m_audioManager)
	//	 {
	//		 m_audioManager->StopBGM();
	//	}
	//	DeleteGO(this);
	//	return;
	//}
	if (m_numDefeatedEnemy == 6)
	{
		FirstFloor();
	}
	if(m_numDefeatedEnemy == 10)
	{
		SecondFloor();
	}
	if (m_numDefeatedEnemy == 11)
	{
		ThirdFloor();
	}
}

void Game::Pause()
{
	if (m_isSetting)
	{
		return;
	}

	if (g_pad[0]->IsTrigger(enButtonStart)) 
	{
		m_isPause = !m_isPause; 
	}
	m_Pause.Update();
}

void Game::PauseRender()
{
	if (!m_isPause) return;

	if (m_isSetting)
	{
		return;
	}

	// ↓移動
	if (g_pad[0]->IsTrigger(enButtonDown))
	{
		m_pauseSelect++;
		if (m_pauseSelect > 2) m_pauseSelect = 0;
	}

	// ↑移動
	if (g_pad[0]->IsTrigger(enButtonUp))
	{
		m_pauseSelect--;
		if (m_pauseSelect < 0) m_pauseSelect = 2;
	}

	// 決定
	if (g_pad[0]->IsTrigger(enButtonA))
	{
		switch (m_pauseSelect)
		{
		case 0: // ゲームに戻る
			m_isPause = false;
			m_audioManager->PlayBGM(enSound_StageBGM, 0.5f);
			break;
			m_isPause = false;
		case 1: // 音量調節
			if (m_difficul == nullptr)
			{
				m_difficul = NewGO<DifficultyLevel>(0, "DifficultyLevel");
				m_isSetting = true;
			}
			break;

		case 2: // タイトルに戻る
			m_title = NewGO<Title>(0, "Title");
			DeleteGO(this);
			break;

		}
	}
}

void Game::FirstFloor()
{
	m_gire = NewGO<Gire>(0, "gire");
	m_gire->SetPosition(Vector3(0.0f, 0.0f, 0.0f));
	DeleteGO(m_barrier1);
	DeleteGO(m_barrier2);
}

void Game::SecondFloor()
{
	m_gire = NewGO<Gire>(0, "gire");
	m_gire->SetPosition(Vector3(0.0f, 2115.0f, 0.0f));
	DeleteGO(m_barrier3);
	DeleteGO(m_barrier4);
}

void Game::ThirdFloor()
{
	m_gire = NewGO<Gire>(0, "gire");
	m_gire->SetPosition(Vector3(0.0f, 4280.0f, 0.0f));
	DeleteGO(m_barrier5);
	DeleteGO(m_barrier6);
}

void Game::Render(RenderContext& rc)
{
	m_gear.Draw(rc);
	if (m_isPause)
	{
		float move = sinf(m_pauseTime) * 10.0f;

		// スケール
		float baseScale = 1.5f;   // ← 全体サイズ
		float selectScale = 2.0f; // ← 選択中サイズ

		float scale0 = (m_pauseSelect == 0) ? selectScale : baseScale;
		float scale1 = (m_pauseSelect == 1) ? selectScale : baseScale;
		float scale2 = (m_pauseSelect == 2) ? selectScale : baseScale;

		// 中央X計算
		float x0 = CalcCenterX(L"ゲームに戻る", scale0);
		float x1 = CalcCenterX(L"音量調節", scale1);
		float x2 = CalcCenterX(L"タイトル戻る", scale2);

		// Y座標の計算
		float centerY = 80.0f;
		float spacing = 140.0f;

		float y0 = centerY + spacing;
		float y1 = centerY;
		float y2 = centerY - spacing;

		// 反映
		m_PlayerReturnText.SetScale(scale0);
		m_SoundText.SetScale(scale1);
		m_TitleReturnText.SetScale(scale2);

		m_PlayerReturnText.SetPosition(Vector3(x0, y0, 0));
		m_SoundText.SetPosition(Vector3(x1, y1, 0));
		m_TitleReturnText.SetPosition(Vector3(x2, y2, 0));

		m_Pause.SetMulColor({ 1.0f, 1.0f, 1.0f, 0.4f });
		m_Pause.Draw(rc);
		m_TitleReturnText.SetColor(g_vec4White);
		m_PlayerReturnText.SetColor(g_vec4White);
		m_SoundText.SetColor(g_vec4White);

		if (m_pauseSelect == 0)
		{
			m_PlayerReturnText.SetColor(1, 0, 0, 1);
		}
		if (m_pauseSelect == 1)
		{
			m_SoundText.SetColor(1, 0, 0, 1);
		}
		if (m_pauseSelect == 2)
		{
			m_TitleReturnText.SetColor(1, 0, 0, 1);
		}

		float cursorX = 0.0f;
		float cursorY = 0.0f;

		switch (m_pauseSelect)
		{
		case 0:
			cursorX = x0 - 80.0f;
			cursorY = y0;
			break;
		case 1:
			cursorX = x1 - 80.0f;
			cursorY = y1;
			break;
		case 2:
			cursorX = x2 - 80.0f;
			cursorY = y2;
			break;
		}

		Vector3 targetPos = Vector3(cursorX, cursorY, 0);

		// ← ここで揺れを追加する
		targetPos.x += sinf(m_pauseTime * 2.0f) * 5.0f;

		// 補間
		m_cursorPos += (targetPos - m_cursorPos) * 0.15f;

		m_Cursor.SetPosition(m_cursorPos);

		m_TitleReturnText.Draw(rc);
		m_PlayerReturnText.Draw(rc);
		m_SoundText.Draw(rc);
		m_Cursor.Draw(rc);
	}
}