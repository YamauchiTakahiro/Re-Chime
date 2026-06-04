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
#include "Source/Manager/AudioManager/AudioManager.h"
#include "Source/Actor/Item/Potion/Heal/Heal.h"
#include "Source/UIBase/Fade/Fade.h"
#include "Source/Manager/EffectManager/EffectManager.h"
#include "Source/UIBase/VolumeSettings/VolumeSettings.h"
#include "Source/Manager/SpawnData/SpawnData.h"

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

	m_gearArrow.Init("Assets/UIData/gearArrow.DDs", 128, 128);

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
	DeleteGO(m_finalBoss);
	DeleteGO(m_player);
	DeleteGO(m_gameCamera);
	DeleteGO(m_stage);
	DeleteGO(m_ui);
	//DeleteGO(m_gameOver);
	//DeleteGO(m_gameClear);
	DeleteGO(m_gire);
	DeleteGO(m_volumeSetting);
	DeleteGO(m_effectManager);
	DeleteGO(m_fade);
	const auto& barriers = FindGOs<Barrier>("barrier");
	for (auto barrier : barriers)
	{
		DeleteGO(barrier);
	}
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
						m_stage = NewGO<Stage>(0,"stage");

						m_stage->SetPosition(objData.position);
					}
					else if (objData.EqualObjectName(L"smallRobot") == true)
					{
						SpawnData spawnData;
						spawnData.spawnType = enSpawnType::enSpawnType_SmallRobot;
						spawnData.pos = objData.position;
						spawnData.scale = objData.scale;
						spawnData.floorNum = GetFloorFromY(objData.position.y);
						m_spawnList.push_back(spawnData);
					}
					else if (objData.EqualObjectName(L"mediumRobot"))
					{
						SpawnData spawnData;
						spawnData.spawnType = enSpawnType::enSpawnType_MediumRobot;
						spawnData.pos = objData.position;
						spawnData.scale = objData.scale;
						spawnData.floorNum = GetFloorFromY(objData.position.y);
						m_spawnList.push_back(spawnData);
					}
					else if (objData.EqualObjectName(L"FloorBoss") == true)
					{
						SpawnData spawnData;
						spawnData.spawnType = enSpawnType::enSpawnType_FloorBoss;
						spawnData.pos = objData.position;
						spawnData.scale = objData.scale;
						spawnData.floorNum = GetFloorFromY(objData.position.y);
						m_spawnList.push_back(spawnData);
					}
					else if (objData.EqualObjectName(L"finalBoss") == true)
					{
						SpawnData spawnData;
						spawnData.spawnType = enSpawnType::enSpawnType_FinalBoss;
						spawnData.pos = objData.position;
						spawnData.scale = objData.scale;
						spawnData.floorNum = GetFloorFromY(objData.position.y);
						m_spawnList.push_back(spawnData);
					}
					else if (objData.EqualObjectName(L"barrier") == true)
					{
						SpawnData spawnData;
						spawnData.spawnType = enSpawnType::enSpawnType_Barrier;
						spawnData.pos = objData.position;
						spawnData.scale = objData.scale;
						spawnData.rot = objData.rotation;
						spawnData.floorNum = GetFloorFromY(objData.position.y);
						m_spawnList.push_back(spawnData);
					}
					return true;
				});

			SpawnCurrentFloorEnemy();

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

			m_loadCount++;

			m_loadStep = enLoad_Fade;

			break;

		case enLoad_Fade:

			m_fade = NewGO<Fade>(0, "fade");
			m_fadeAreas.push_back({ Vector3(2350.0f, 350.0f, 3600.0f), 300.0f, Vector3(-1276.3, 2137.0f, 3600.0f), true, false, false, false }); // 1階
			m_fadeAreas.push_back({ Vector3(-1680.0f, 2600.0f, -4010.0f), 300.0f, Vector3(1900.0f, 4285.0f, -4050.0f), true, false, false, false });      // 2階
			m_fadeAreas.push_back({ Vector3(2200.0f, 4700.0f, 3600.0f), 300.0f, Vector3(-1324.0f, 6442.2f, 3639.6f), false, true, false, false });      // 3階

			m_loadCount++;

			m_loadStep = enLoad_End;

			break;

		case enLoad_End:

			m_audioManager =FindGO<AudioManager>("audioManager");

			if (m_audioManager)
			{
				m_audioManager->PlayBGM(enSound_StageBGM,0.5f);
			}

			if (m_ui)
			{
				m_goalState = enGoalState_DefeatEnemy;
				m_ui->ShowGoal(L"敵を倒せ");
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
		if (!m_getGire)
		{
			switch (m_floorNo)
			{
			case 1:
				DeleteBarriers();
				break;

			case 2:
				DeleteBarriers();
				break;

			case 3:
				DeleteBarriers();
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
			m_getGire = true;
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

		for (int i = 0; i < m_fadeAreas.size(); i++) {
			auto& area = m_fadeAreas[i];
			Vector3 diff = playerPos - area.pos;
			if (diff.Length() < area.radius &&playerPos.y > area.pos.y - 50.0f)
			{
				m_currentFadeAreaIndex = i;

				m_nextMovePos = area.targetPos;

				if (!area.playedIntro)
				{
					m_nextIntro = area.intro;
					m_nextBossIntro = area.bossIntro;

					area.playedIntro = true;
				}
				else
				{
					m_nextIntro = false;
					m_nextBossIntro = false;
				}

				m_isMoveNextFloor = true;

				m_audioManager->PlaySE(enSound_StairsSE,0.5f,enSEPlay_AllowOverlap);

				m_fade->StartFadeOut();

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

			if (m_intro || m_bossIntro)
			{
				if (m_bossIntro)
				{
					m_gameCamera->StartBossCamera();
				}
				else if (m_intro)
				{
					m_gameCamera->StartIntroCamera();
				}
				m_fade->StartFadeIn();

				m_gameCamera->ResetCameraTimer();
			}

			auto& area = m_fadeAreas[m_currentFadeAreaIndex];

			if (!area.movedFloor)
			{
				m_floorNo++;
				m_needGireCount++;

				area.movedFloor = true;
			}

			SpawnCurrentFloorEnemy();

			if (m_ui)
			{
				m_goalState = enGoalState_DefeatEnemy;

				m_goalDelayTimer = 0.0f;
				m_isGoalDelay = true;
			}

			m_isMoveNextFloor = false;

			if (!m_intro && !m_bossIntro)
			{
				m_fade->StartFadeIn();
			}
		}
	}

	if (m_isGoalDelay)
	{
		m_goalDelayTimer += g_gameTime->GetFrameDeltaTime();

		if (m_goalDelayTimer >= 0.1f) // ← 0.1秒後
		{
			if (m_ui)
			{
				if (m_floorNo == 4)
				{
					m_ui->ShowGoal(L"ボスを倒せ");
				}
				else
				{
					m_ui->ShowGoal(L"敵を倒せ");
				}
			}

			m_isGoalDelay = false;
		}
	}
	CreateGire();
	m_navTimer += g_gameTime->GetFrameDeltaTime();
}

void Game::SpawnEnemy(const SpawnData& spawnData)
{
	switch (spawnData.spawnType)
	{
		case enSpawnType::enSpawnType_SmallRobot:
		{
			auto smallRobot = NewGO<SmallRobot>(0, "smallRobot");
			smallRobot->SetPosition(spawnData.pos);
			smallRobot->SetScale(spawnData.scale);
			m_smallRobot.push_back(smallRobot);
			break;
		}
		case enSpawnType::enSpawnType_MediumRobot:
		{
			auto mediumRobot = NewGO<MediumRobot>(0, "mediumRobot");
			mediumRobot->SetPosition(spawnData.pos);
			mediumRobot->SetScale(spawnData.scale);
			m_mediumRobot.push_back(mediumRobot);
			break;
		}
		case enSpawnType::enSpawnType_FloorBoss:
		{
			auto floorBoss = NewGO<FloorBoss>(0, "floorBoss");
			floorBoss->SetPosition(spawnData.pos);
			floorBoss->SetScale(spawnData.scale);
			m_floorBoss.push_back(floorBoss);
			break;
		}
		case enSpawnType::enSpawnType_FinalBoss:
		{
			m_finalBoss = NewGO<FinalBoss>(0, "finalBoss");
			m_finalBoss->SetPosition(spawnData.pos);
			m_finalBoss->SetScale(spawnData.scale);
			break;
		}
		case enSpawnType::enSpawnType_Barrier:
		{
			auto barrier = NewGO<Barrier>(0, "barrier");
			barrier->SetPosition(spawnData.pos);
			barrier->SetScale(spawnData.scale);
			barrier->SetRotation(spawnData.rot);
			m_barrier.push_back(barrier);
			break;
		}
	}
}

void Game::SpawnCurrentFloorEnemy()
{
	m_remainEnemyCount = 0;
	for (auto& spawnData : m_spawnList)
	{
		if (spawnData.spawned)
		{
			continue;
		}

		if (spawnData.floorNum != m_floorNo)
		{
			continue;
		}

		SpawnEnemy(spawnData);

		spawnData.spawned = true;

		if(spawnData.spawnType != enSpawnType::enSpawnType_Barrier)
		{
			m_remainEnemyCount++;
		}
	}
	m_createGire = false;
	m_getGire = false;
}

void Game::Pause()
{
	if (m_isSetting)
	{
		return;
	}

	if (m_ui && m_ui->IsInventoryOpen())
	{
		return;
	}

	if (g_pad[0]->IsTrigger(enButtonStart)) 
	{
		m_isPause = !m_isPause; 

		if (m_audioManager != nullptr)
		{
			if (m_isPause)
			{
				// 開いた
				m_audioManager->PlaySE(
					enSound_PauseSE,
					0.3f,
					enSEPlay_AllowOverlap
				);
			}
			else
			{
				// 閉じた
				m_audioManager->PlaySE(
					enSound_PauseCloseSE,
					0.3f,
					enSEPlay_AllowOverlap
				);
			}
		}
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
		if (m_audioManager)
		{
			m_audioManager->PlaySE(
				enSound_ChoiceSE,
				0.3f,
				enSEPlay_AllowOverlap
			);
		}
	}

	// ↑移動
	if (g_pad[0]->IsTrigger(enButtonUp))
	{
		m_pauseSelect--;
		if (m_pauseSelect < 0) m_pauseSelect = 2;
		if (m_audioManager)
		{
			m_audioManager->PlaySE(
				enSound_ChoiceSE,
				0.3f,
				enSEPlay_AllowOverlap
			);
		}
	}

	// 決定
	if (g_pad[0]->IsTrigger(enButtonA))
	{
		if (m_audioManager)
		{
			m_audioManager->PlaySE(
				enSound_DecisionSE,
				0.3f,
				enSEPlay_AllowOverlap
			);
		}
		switch (m_pauseSelect)
		{
		case 0: // ゲームに戻る
			m_isPause = false;
			m_audioManager->PlayBGM(enSound_StageBGM, 0.5f);
			break;
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

void Game::DeleteBarriers()
{
	m_audioManager->PlaySE(enSound_BrokenBarricadeSE, 0.5f, enSEPlay_NoOverlap);
	const auto& barriers = FindGOs<Barrier>("barrier");
	for (auto barrier : barriers)
	{
		DeleteGO(barrier);
	}
}

void Game::CreateGire()
{
	if (m_remainEnemyCount == 0 && !m_createGire)
	{
		Vector3 pos;

		switch (m_floorNo)
		{
		case 1:
			pos = Vector3(0.0f, 0.0f, 0.0f);
			break;

		case 2:
			pos = Vector3(0.0f, 2115.0f, 0.0f);
			break;

		case 3:
			pos = Vector3(0.0f, 4280.0f, 0.0f);
			break;
		}
		m_audioManager->PlaySE(enSound_GearDropSE, 0.5f);
		m_createGire = true;
		if (m_ui)
		{
			m_goalState = enGoalState_GetGear;

			m_ui->ShowGoal(L"歯車をとれ");
		}
		m_gire = NewGO<Gire>(0, "gire");
		m_gire->SetPosition(pos);
		m_gire->SetScale(Vector3(3.0f, 3.0f, 3.0f));
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
	m_gire = FindGO<Gire>("gire");

	if (m_gire != nullptr)
	{
		DrawGearArrow(rc);
	}
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

float Game::GetInventoryCoolTime() const
{
	switch (m_difficulty)
	{
	case EASY:
		return 5.0f;

	case NORMAL:
		return 10.0f;

	case HARD:
		return 10.0f;

	case LUNATIC:
		return 20.0f;
	}

	return 10.0f;
}

int Game::GetFloorFromY(float y) const
{
	if (y < 2137.0f)
	{
		return 1;
	}
	else if (y < 4285.0f)
	{
		return 2;
	}
	else if (y < 6442.2f)
	{
		return 3;
	}
	else
	{
		return 4;
	}
}

void Game::DrawGearArrow(RenderContext& rc)
{
	if (m_player == nullptr || m_gire == nullptr)
	{
		return;
	}

	Vector3 playerPos = m_player->GetPosition();
	Vector3 gearPos = m_gire->GetPosition();

	// 距離
	float distance = (gearPos - playerPos).Length();

	// 近すぎたら非表示
	if (distance < 300.0f)
	{
		return;
	}

	// プレイヤー→歯車方向
	Vector3 dir = gearPos - playerPos;
	dir.y = 0.0f;

	if (dir.LengthSq() < 0.01f)
	{
		return;
	}

	dir.Normalize();

	// カメラ基準
	Vector3 camForward = g_camera3D->GetForward();
	camForward.y = 0.0f;
	camForward.Normalize();

	Vector3 camRight;
	camRight.Cross(Vector3::AxisY, camForward);
	camRight.Normalize();

	float screenX = dir.Dot(camRight);
	float screenY = dir.Dot(camForward);

	// 目標角度
	float targetAngle = atan2f(screenX, screenY);

	// -----------------------------
	// 角度補間
	// -----------------------------
	float diff = targetAngle - m_navAngle;

	while (diff > Math::PI)
	{
		diff -= Math::PI * 2.0f;
	}

	while (diff < -Math::PI)
	{
		diff += Math::PI * 2.0f;
	}

	m_navAngle += diff * 0.15f;

	// -----------------------------
	// 距離で半径変化
	// -----------------------------
	float radius = 250.0f + min(distance * 0.05f, 250.0f);

	// -----------------------------
	// フワフワ
	// -----------------------------
	float wave = sinf(m_navTimer * 5.0f) * 10.0f;

	Vector3 targetPos;
	targetPos.x = sinf(m_navAngle) * radius;
	targetPos.y = cosf(m_navAngle) * radius + wave;
	targetPos.z = 0.0f;

	// -----------------------------
	// 補間移動
	// -----------------------------
	m_navPos += (targetPos - m_navPos) * 0.1f;

	// -----------------------------
	// 距離でサイズ変化
	// -----------------------------
	float scale = distance / 3000.0f;

	if (scale < 0.8f)
	{
		scale = 0.8f;
	}

	if (scale > 2.0f)
	{
		scale = 2.0f;
	}

	m_gearArrow.SetScale(Vector3(scale, scale, scale));

	// -----------------------------
	// 回転
	// -----------------------------
	Quaternion rot;
	rot.SetRotationZ(m_navAngle - Math::DegToRad(90.0f));

	m_gearArrow.SetRotation(rot);
	m_gearArrow.SetPosition(m_navPos);

	m_gearArrow.Update();
	m_gearArrow.Draw(rc);
}