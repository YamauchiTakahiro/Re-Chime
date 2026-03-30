#include "stdafx.h"
#include "Game.h"
#include "Source/Actor/Character/Player/Player.h"
#include "source/Camera/GameCamera.h"
#include "Source/Actor/Stage/Stage.h"
#include "Source/UIBase/Title/Title.h"
#include "Source/UIBase/UI/UI.h"
#include "Source/Actor/Character/Enemy/SmallRobot/SmallRobot.h"
#include "Source/UIBase/GameOver/GameOver.h"
#include "Source/UIBase/DifficultyLevel/DifficultyLevel.h"
#include "Source/Actor/Item/Gire/Gire.h"


Game::Game()
{
	//PhysicsWorld::GetInstance()->EnableDrawDebugWireFrame();
}

bool Game::Start()
{
	m_player = NewGO<Player>(0, "player");
	m_gameCamera = NewGO<GameCamera>(0, "gameCamera");
	m_stage = NewGO<Stage>(0, "stage");
	m_ui = NewGO<UI>(0, "ui");
	m_smallRobot = NewGO<SmallRobot>(0, "smallRobot");
	return true;
}

Game::~Game()
{
	DeleteGO(m_player);
	DeleteGO(m_gameCamera);
	DeleteGO(m_stage);
	DeleteGO(m_ui);
	DeleteGO(m_smallRobot);
}

void Game::Update()
{	
	//プレイヤーのHPが0以下になったらゲームオーバー。
	int playerHP = m_player->m_playerHp;
	if (playerHP <= 0)
	{
		NewGO<GameOver>(0, "GameOver");
		DeleteGO(this);
		return;
	}
}

void Game::Render(RenderContext& rc)
{
	m_gear.Draw(rc);
}