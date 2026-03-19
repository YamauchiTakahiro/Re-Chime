#include "stdafx.h"
#include "Game.h"
#include "Source/Actor/Character/Player/Player.h"
#include "source/Camera/GameCamera.h"
#include "Source/Actor/Stage/Stage.h"
#include "Source/UIBase/Title/Title.h"
#include "Source/UIBase/UI/UI.h"
#include "Source/Actor/Character/Enemy/SmallRobot/SmallRobot.h"


Game::Game()
{
	PhysicsWorld::GetInstance()->EnableDrawDebugWireFrame();
}

bool Game::Start()
{
	m_player = NewGO<Player>(0, "player");
	m_gameCamera = NewGO<GameCamera>(0, "gameCamera");
	//m_stage = NewGO<Stage>(0, "stage");
	m_ui = NewGO<UI>(0, "ui");
	m_smallRobot = NewGO<SmallRobot>(0, "smallRobot");
	return true;
}

Game::~Game()
{
	DeleteGO(m_player);
}

void Game::Update()
{
	//g_renderingEngine->DisableRaytracing();
	//wchar_t healthText[256];
	//swprintf(healthText, 256, L"health:%d", m_player->GetHP();
	//m_health.SetText(healthText);
	//m_health.SetPosition(-950.0f, -475.0f, 0.0f);
	//m_health.SetScale(1.0f);
	//m_health.SetColor(g_vec4Black);
}

void Game::Render(RenderContext& rc)
{
}