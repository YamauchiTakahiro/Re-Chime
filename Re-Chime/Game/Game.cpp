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
#include "Source/Actor/Character/Enemy/RareRobot/RareRobot.h"
#include "Source/UIBase/GameClear/GameClear.h"
#include "Source/Sound/AudioManager/AudioManager.h"
#include "Source/Actor/Item/Potion/Heal/Heal.h"
#include "Source/UIBase/Fade/Fade.h"
#include "Source/EffectManager/EffectManager.h"
#include "Source/UIBase/VolumeSettings/VolumeSettings.h"

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
    m_isLoading = true;

    m_isReady = false;

    m_loadStep = enLoad_Pause;

    m_loadCount = 0;

    m_player = nullptr;

    m_gameCamera = nullptr;

    m_ui = nullptr;

    m_fade = nullptr;

	//PhysicsWorld::GetInstance()->EnableDrawDebugWireFrame();
}

bool Game::Start()
{
	m_maxLoadCount = 8;
	srand((unsigned)time(NULL));

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
	const auto& rareRobots = FindGOs<RareRobot>("rareRobot");
	for (auto rareRobot : rareRobots)
	{
		DeleteGO(rareRobot);
	}
	DeleteGO(m_player);
	DeleteGO(m_gameCamera);
	DeleteGO(m_stage);
	DeleteGO(m_ui);
	DeleteGO(m_gameOver);
	DeleteGO(m_gameClear);
	DeleteGO(m_gire);
	DeleteGO(m_volumeSetting);
	DeleteGO(m_effectManager);
	DeleteGO(m_fade);

}

void Game::Update()
{	
	//ロード中なら停止
	if (m_isLoading)
	{
		switch (m_loadStep)
		{
		case enLoad_Pause:

			m_Pause.Init("Assets/Sprite/pause.DDs",1920,1080);

			m_PlayerReturnText.SetText(L"ゲームに戻る");
			m_PlayerReturnText.SetScale(1.5f);

			m_SoundText.SetText(L"音量調節");
			m_SoundText.SetScale(1.5f);

			m_TitleReturnText.SetText(L"タイトル戻る");
			m_TitleReturnText.SetScale(1.5f);

			m_Cursor.SetText(L">");
			m_Cursor.SetScale(2.5f);

			m_cursorPos = Vector3(0, 0, 0);

			m_loadCount++;

			m_loadStep =enLoad_Player;

			break;

		case enLoad_Player:

			m_player =NewGO<Player>(0,"player");

			m_loadCount++;

			m_loadStep =enLoad_Camera;

			break;

		case enLoad_Camera:

			m_gameCamera =NewGO<GameCamera>(0,"gameCamera");

			m_loadCount++;

			m_loadStep =enLoad_Level;

			break;

		case enLoad_Level:

			m_levelRender.Init("Assets/modelData/Level/ReChaim.tkl",[&](LevelObjectData& objData)-> bool
				{
					if (objData.EqualObjectName(L"Stage"))
					{
						m_stage =
							NewGO<Stage>(0,"stage");

						m_stage->SetPosition(objData.position);
					}
					else if (objData.EqualObjectName(L"mediumRobot"))
					{
						auto enemy = NewGO<MediumRobot>(0, "mediumRobot");

						enemy->SetPosition(objData.position);

						enemy->SetScale(objData.scale);

						m_mediumRobot.push_back(enemy);

						m_enemyCount++;
					}
					else if (objData.EqualObjectName(L"smallRobot") == true)
					{
						auto smallRobot = NewGO<SmallRobot>(0, "smallRobot");

						smallRobot->SetPosition(objData.position);

						smallRobot->SetScale(objData.scale);

						m_smallRobot.push_back(smallRobot);

						m_enemyCount++;
					}
					else if (objData.EqualObjectName(L"FloorBoss") == true)
					{
						auto floorBoss = NewGO<FloorBoss>(0, "floorBoss");

						floorBoss->SetPosition(objData.position);

						floorBoss->SetScale(objData.scale);

						m_floorBoss.push_back(floorBoss);

						m_enemyCount++;
					}
					else if (objData.EqualObjectName(L"finalBoss") == true)
					{
						auto finalBoss = NewGO<FinalBoss>(0, "finalBoss");

						finalBoss->SetPosition(objData.position);

						finalBoss->SetScale(objData.scale);

						m_finalBoss.push_back(finalBoss);

						m_enemyCount++;
					}
					else if (objData.EqualObjectName(L"rareRobot") == true)
					{
						// 0～99 の乱数
						int randValue = rand() % 100;
						int spawnRate = 20;

						// 30%で生成
						if (randValue < spawnRate)
						{
							auto rareRobot = NewGO<RareRobot>(0, "rareRobot");

							rareRobot->SetPosition(objData.position);

							rareRobot->SetScale(objData.scale);

							m_rareRobot.push_back(rareRobot);
						}
					}
					else if (objData.EqualObjectName(L"barrier1") == true)
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
					}
					return true;
				});

			m_loadCount++;

			m_loadStep =enLoad_Effect;

			break;

		case enLoad_Effect:

			EffectManager::GetInstance().Load();

			m_loadCount++;

			m_loadStep = enLoad_UI;

			break;

		case enLoad_UI:

			m_ui = NewGO<UI>(0, "ui");

			if (m_ui)
			{
				m_ui->ShowGoal(L"敵を倒せ");
			}

			m_loadCount++;

			m_loadStep = enLoad_Fade;

			break;

		case enLoad_Fade:

			m_fade = NewGO<Fade>(0, "fade");
			m_fadeAreas.push_back({ Vector3(2350.0f, 350.0f, 3600.0f), 300.0f, Vector3(-1276.3, 2137.0f, 3600.0f), true, false }); // 1階
			m_fadeAreas.push_back({ Vector3(-1680.0f, 2600.0f, -4010.0f), 300.0f, Vector3(1900.0f, 4285.0f, -4050.0f), true, false });      // 2階
			m_fadeAreas.push_back({ Vector3(2200.0f, 4700.0f, 3600.0f), 300.0f, Vector3(-1324.0f, 6442.2f, 3639.6f), false, true });      // 3階

			m_loadCount++;

			m_loadStep = enLoad_End;

			break;

		case enLoad_End:

			m_audioManager =FindGO<AudioManager>("audioManager");

			if (m_audioManager)
			{
				m_audioManager->PlayBGM(enSound_StageBGM,0.5f);
			}

			m_isReady = true;

			m_isLoading = false;

			return;
		}

		return;
	}

	m_pauseTime += 0.05f;

	Pause();

	if (m_isPause)
	{
		PauseRender();
		return;
	}

	if (m_player == nullptr)
	{
		m_player = FindGO<Player>("player");

		if (m_player == nullptr)
		{
			return;
		}
	}

	//プレイヤーのHPが0以下になったらゲームオーバー。
	int hp = 0;
	int playerHP = m_player->GetHP();
	if (playerHP <= 0)
	{
		NewGO<GameOver>(0, "GameOver");
		if (m_audioManager)
		{
			m_audioManager->StopBGM(enSound_StageBGM);
		}
		DeleteGO(this);
		return;
	}

	int gireCount = m_player->GetGireCount();
	if (gireCount >= m_needGireCount)
	{
		switch (m_floorNo)
		{
		case 1:
			FirstFloor();
			m_createGire = false;
			break;

		case 2:
			SecondFloor();
			m_createGire = false;
			break;

		case 3:
			ThirdFloor();
			m_createGire = false;
			break;

		case 4:
			m_gameClear = NewGO<GameClear>(0, "gameClear");

			if (m_audioManager)
			{
				m_audioManager->StopBGM(enSound_StageBGM);
			}

			DeleteGO(this);
			return;
		}
	}

	//プレイヤーの現在の座標を表示
	Vector3 playerPos = m_player->GetPosition();

	/*wchar_t text[256];
	swprintf_s(text, L"X: %.1f Y: %.1f Z: %.1f",
		playerPos.x, playerPos.y, playerPos.z);

	m_Pos.SetText(text);*/

	// フェード判定
	if (m_fade != nullptr && !m_isMoveNextFloor)
	{
		bool isInAnyArea = false;

		for (auto& area : m_fadeAreas) {
			Vector3 diff = playerPos - area.pos;
			if (diff.Length() < area.radius &&
				playerPos.y > area.pos.y - 50.0f)
			{
				m_nextMovePos = area.targetPos;

				m_nextIntro = area.intro;
				m_nextBossIntro = area.bossIntro;

				m_currentFadeArea = &area;

				m_isMoveNextFloor = true;

				m_audioManager->PlaySE(
					enSound_StairsSE,
					0.5f,
					enSEPlay_AllowOverlap
				);

				m_fade->StartFadeOut();
	
				// 次フロアへ進んだ時だけ必要数増加
				if (m_fade->IsFadeOutFinished())
				{
					m_player->SetPosition(m_nextMovePos);

					m_intro = m_nextIntro;
					m_bossIntro = m_nextBossIntro;

					m_floorNo++;
					m_needGireCount++;

					m_isMoveNextFloor = false;

					m_fade->StartFadeIn();
				}

				if (m_ui)
				{
					m_goalState = enGoalState_DefeatEnemy;

					m_ui->ShowGoal(
						L"敵を倒せ"
					);
				}

				m_gameCamera->ResetCameraTimer();

				isInAnyArea = true;
				break;
			}
		}	
	}
	if (m_isMoveNextFloor)
	{
		if (m_fade->IsFadeOutFinished())
		{
			m_player->SetPosition(m_nextMovePos);

			m_intro = m_nextIntro;
			m_bossIntro = m_nextBossIntro;

			if (m_currentFadeArea != nullptr)
			{
				if (!m_currentFadeArea->countAdded)
				{
					m_floorNo++;
					m_needGireCount++;

					m_currentFadeArea->countAdded = true;
				}
			}

			m_fade->ResetFadeOut();

			m_isMoveNextFloor = false;

			m_fade->StartFadeIn();
		}
	}
	CreateGire();
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
				m_volumeSetting = NewGO<VolumeSettings>(0, "VolumeSetting");
				m_isSetting = true;

			break;

		case 2: // タイトルに戻る
			NewGO<Title>(0, "Title");
			m_volumeSetting = nullptr;
			DeleteGO(this);
			break;

		}
	}
}

void Game::FirstFloor()
{
	DeleteGO(m_barrier1);
	DeleteGO(m_barrier2);
}

void Game::SecondFloor()
{
	DeleteGO(m_barrier3);
	DeleteGO(m_barrier4);
}

void Game::ThirdFloor()
{
	DeleteGO(m_barrier5);
	DeleteGO(m_barrier6);
}

void Game::CreateGire()
{
	int gireCount = m_player->GetGireCount();

	if (m_numDefeatedEnemy == 6 && !m_createGire && gireCount == 0)
	{
		m_gire = NewGO<Gire>(0, "gire");
		m_gire->SetPosition(Vector3(0.0f, 0.0f, 0.0f));
		m_gire->SetScale(Vector3(3.0f, 3.0f, 3.0f));
		m_audioManager->PlaySE(enSound_GearDropSE, 0.5f);
		m_createGire = true;
		if (m_ui)
		{
			m_goalState = enGoalState_GetGear;

			m_ui->ShowGoal(L"歯車をとれ");
		}
	}
	if (m_numDefeatedEnemy == 11 && !m_createGire && gireCount == 1)
	{
		m_gire = NewGO<Gire>(0, "gire");
		m_gire->SetPosition(Vector3(0.0f, 2115.0f, 0.0f));
		m_gire->SetScale(Vector3(3.0f, 3.0f, 3.0f));
		m_audioManager->PlaySE(enSound_GearDropSE, 0.5f);
		m_createGire = true;
		if (m_ui)
		{
			m_goalState = enGoalState_GetGear;

			m_ui->ShowGoal(
				L"歯車をとれ"
			);
		}
	}
	if (m_numDefeatedEnemy == 13 && !m_createGire && gireCount == 2)
	{
		m_gire = NewGO<Gire>(0, "gire");
		m_gire->SetPosition(Vector3(0.0f, 4280.0f, 0.0f));
		m_gire->SetScale(Vector3(3.0f, 3.0f, 3.0f));
		m_audioManager->PlaySE(enSound_GearDropSE, 0.5f);
		m_createGire = true;
		if (m_ui)
		{
			m_goalState = enGoalState_GetGear;

			m_ui->ShowGoal(
				L"歯車をとれ"
			);
		}
	}
}

void Game::Render(RenderContext& rc)
{
	if (m_isLoading)
	{
		return;
	}
	m_gear.Draw(rc);
	m_Pos.Draw(rc);
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

bool Game::IsFade() const
{
	return m_fade != nullptr && m_fade->IsFade();
}