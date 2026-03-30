#include "stdafx.h"
#include "GameOver.h"
#include "Source/UIBase/Title/Title.h"

GameOver::GameOver()
{
	m_GameOver.Init("Assets/Sprite/gameover.DDs", 1920.0f, 1080.0f);
}

GameOver::~GameOver()
{

}

bool GameOver::Start()
{
	return true;
}

void GameOver::Update()
{
	if (g_pad[0]->IsTrigger(enButtonA))
	{
		NewGO<Title>(0, "title");
		DeleteGO(this);
	}
}

void GameOver::Render(RenderContext& rc)
{
	m_GameOver.Draw(rc);
}

