#include "stdafx.h"
#include "Game.h"
#include "Source/Actor/Character/Player/Player.h"
#include "source/Camera/GameCamera.h"
#include "Source/Actor/Stage/Stage.h"
#include "Source/UIBase/Title/Title.h"


Game::Game()
{

}

bool Game::Start()
{
	m_player = NewGO<Player>(0, "player");
	m_gameCamera = NewGO<GameCamera>(0, "gameCamera");
	m_stage = NewGO<Stage>(0, "stage");
	m_title = NewGO<Title>(0, "title");
	return true;
}

Game::~Game()
{
	DeleteGO(m_player);
}

void Game::Update()
{
	g_renderingEngine->DisableRaytracing();
}

void Game::Render(RenderContext& rc)
{
}