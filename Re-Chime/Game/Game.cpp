#include "stdafx.h"
#include "Game.h"
#include "Source/Actor/Character/Player/Player.h"
#include "source/Camera/GameCamera.h"
#include "Source/Actor/Stage/Stage.h"
#include "Source/UIBase/Title/Title.h"
#include "Source/UIBase/UI/UI.h"
#include "Source/Actor/Character/Enemy/SmallRobot/SmallRobot.h"
#include "Source/Actor/Character/Enemy/FloorBoss/FloorBoss.h"
#include "Source/UIBase/GameOver/GameOver.h"
#include "Source/UIBase/DifficultyLevel/DifficultyLevel.h"
#include "Source/Actor/Item/Gire/Gire.h"
#include "Source/Actor/Character/Enemy/MediumRobot/MediumRobot.h"
#include "Source/Actor/Character/Enemy/FinalBoss/FinalBoss.h"


Game::Game()
{
	//PhysicsWorld::GetInstance()->EnableDrawDebugWireFrame();
}

bool Game::Start()
{
	m_player = NewGO<Player>(0, "player");
	m_gameCamera = NewGO<GameCamera>(0, "gameCamera");
	m_ui = NewGO<UI>(0, "ui");
	m_levelRender.Init("Assets/modelData/Level/ReChaim.tkl", [&](LevelObjectData& objData)
		{
			if (objData.EqualObjectName(L"Stage") == true)
			{
				m_stage = NewGO<Stage>(0, "stage");

				m_stage->SetPosition(objData.position);
			}
			else if (objData.EqualObjectName(L"smallRobot") == true)
			{
				m_smallRobot = NewGO<SmallRobot>(0, "smallRobot");

				m_smallRobot->SetPosition(objData.position);

				m_smallRobot->SetScale(objData.scale);
			}
			/*else if (objData.EqualObjectName(L"floorBoss") == true)
			{
				m_floorBoss = NewGO<FloorBoss>(0, "floorBoss");

				m_floorBoss->SetPosition(objData.position);
				
				m_floorBoss->SetScale(objData.scale);
			}
			else if (objData.EqualObjectName(L"mediumRobot") == true)
			{
				m_mediumRobot = NewGO<MediumRobot>(0, "mediumRobot");
				
				m_mediumRobot->SetPosition(objData.position);
				
				m_mediumRobot->SetScale(objData.scale);
			}
			else if (objData.EqualObjectName(L"finalBoss") == true)
			{
				m_finalBoss = NewGO<FinalBoss>(0, "finalBoss");
				
				m_finalBoss->SetPosition(objData.position);
				
				m_finalBoss->SetScale(objData.scale);
			}*/
			return true;
		});
	return true;
}

Game::~Game()
{
	DeleteGO(m_player);
	DeleteGO(m_gameCamera);
	DeleteGO(m_stage);
	DeleteGO(m_ui);
	DeleteGO(m_smallRobot);
	DeleteGO(m_floorBoss);
}

void Game::Update()
{	
	Pause();
	//プレイヤーのHPが0以下になったらゲームオーバー。
	int hp = 0;
	int playerHP = m_player->GetHP(hp);
	if (playerHP <= 0)
	{
		NewGO<GameOver>(0, "GameOver");
		DeleteGO(this);
		return;
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
}

void Game::Render(RenderContext& rc)
{
	m_gear.Draw(rc);
}