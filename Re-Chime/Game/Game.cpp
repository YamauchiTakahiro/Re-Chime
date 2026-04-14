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
#include "Source/UIBase/Title/Title.h"

Game::Game()
{
	//PhysicsWorld::GetInstance()->EnableDrawDebugWireFrame();
}

bool Game::Start()
{
	m_Pause.Init("Assets/Sprite/pause.DDs", 1920.0f, 1080.0f);
	m_player = NewGO<Player>(0, "player");
	m_gameCamera = NewGO<GameCamera>(0, "gameCamera");
	m_ui = NewGO<UI>(0, "ui");
	m_barrier = NewGO<Barrier>(0, "barrier");
	m_barrier->SetPosition(Vector3(3000.0f, 0.0f, 3700.0f));
	m_levelRender.Init("Assets/modelData/Level/ReChaim.tkl", [&](LevelObjectData& objData)
		{
			if (objData.EqualObjectName(L"Stage") == true)
			{
				m_stage = NewGO<Stage>(0, "stage");

				m_stage->SetPosition(objData.position);
			}
			else if (objData.EqualObjectName(L"smallRobot") == true)
			{
				auto smallRobot = NewGO<SmallRobot>(0, "smallRobot");

				smallRobot->SetPosition(objData.position);

				smallRobot->SetScale(objData.scale);

				m_smallRobot.push_back(smallRobot);

				m_enemyCount++;
			}
			/*else if (objData.EqualObjectName(L"floorBoss") == true)
			{
				auto floorBoss = NewGO<FloorBoss>(0, "floorBoss");

				floorBoss->SetPosition(objData.position);

				floorBoss->SetScale(objData.scale);

				m_floorBoss.push_back(floorBoss);

				m_enemyCount++;
			}
			else if (objData.EqualObjectName(L"mediumRobot") == true)
			{
				auto mediumRobot = NewGO<MediumRobot>(0, "mediumRobot");

				mediumRobot->SetPosition(objData.position);

				mediumRobot->SetScale(objData.scale);

				m_mediumRobot.push_back(mediumRobot);

				m_enemyCount++;
			}
			else if (objData.EqualObjectName(L"finalBoss") == true)
			{
				auto finalBoss = NewGO<FinalBoss>(0, "finalBoss");
				
				finalBoss->SetPosition(objData.position);
				
				finalBoss->SetScale(objData.scale);

				m_finalBoss.push_back(finalBoss);

				m_enemyCount++;
			}*/
			return true;
	});

	m_TitleReturnText.SetText(L"タイトル戻る");
	m_TitleReturnText.SetPosition(Vector3(-245.0f, 0.0f, 0.0f));
	m_TitleReturnText.SetScale(1.5f);

	m_PlayerReturnText.SetText(L"ゲームに戻る");
	m_PlayerReturnText.SetPosition(Vector3(-245.0f, 80.0f, 0.0f));
	m_PlayerReturnText.SetScale(1.5f);
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
	Pause();
	PauseRender();
	if (m_isPause)
	{
		return;
	}
	//プレイヤーのHPが0以下になったらゲームオーバー。
	int hp = 0;
	int playerHP = m_player->GetHP(hp);
	if (playerHP <= 0)
	{
		NewGO<GameOver>(0, "GameOver");
		DeleteGO(this);
		return;
	}
	if (m_enemyCount <= 0)
	{
		FirstFloor();
	}
}

void Game::Pause()
{
	if (g_pad[0]->IsTrigger(enButtonStart) && m_isPause)
	{
		m_isPause = false;
		return;
	}
	else if (g_pad[0]->IsTrigger(enButtonStart) && !m_isPause)
	{
		m_isPause = true;
	}
	m_Pause.Update();
}

void Game::PauseRender()
{
	if (g_pad[0]->IsTrigger(enButtonDown))
	{
		m_isTitleReturn = true;
		m_isPlayerReturn = false;
	}

	if (g_pad[0]->IsTrigger(enButtonUp))
	{
		m_isTitleReturn = false;
		m_isPlayerReturn = true;
	}

	if (m_isPause && m_isTitleReturn && g_pad[0]->IsTrigger(enButtonA))
	{
		m_title = NewGO<Title>(0, "Title");
		DeleteGO(this);
	}

	if (m_isPause && m_isPlayerReturn && g_pad[0]->IsTrigger(enButtonA))
	{
		m_isPause = false;
	}
}

void Game::FirstFloor()
{
	m_gire = NewGO<Gire>(0, "gire");
	DeleteGO(m_barrier);
}

void Game::Render(RenderContext& rc)
{
	m_gear.Draw(rc);
	if (m_isPause)
	{
		m_Pause.SetMulColor({ 1.0f, 1.0f, 1.0f, 0.4f });
		m_Pause.Draw(rc);
		m_TitleReturnText.SetColor(g_vec4White);
		m_PlayerReturnText.SetColor(g_vec4White);
		m_TitleReturnText.Draw(rc);
		m_PlayerReturnText.Draw(rc);
		if (m_isTitleReturn)
		{
			m_TitleReturnText.SetColor(1.0f, 0.0f, 0.0f, 1.0f);
		}

		if (m_isPlayerReturn)
		{
			m_PlayerReturnText.SetColor(1.0f, 0.0f, 0.0f, 1.0f);
		}
	}
}