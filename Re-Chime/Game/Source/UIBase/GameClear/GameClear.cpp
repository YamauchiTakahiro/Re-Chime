#include "stdafx.h"
#include "GameClear.h"
#include "Source/UIBase/Title/Title.h"

GameClear::GameClear()
{
}

GameClear::~GameClear()
{
}

bool GameClear::Start()
{
	m_GameClear.Init("Assets/Sprite/clear.DDs", 1920.0f, 1080.0f);
	return true;
}

void GameClear::Update()
{
	if (g_pad[0]->IsTrigger(enButtonA))
	{
		NewGO<Title>(0, "title");
		DeleteGO(this);
	}
}

void GameClear::Render(RenderContext& rc)
{
	m_GameClear.Draw(rc);
}
