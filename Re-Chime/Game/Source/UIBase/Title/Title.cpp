#include "stdafx.h"
#include "Title.h"
#include "Game.h"

Title::Title()
{
	m_spriteRender.Init("Assets/Sprite/title.DDs", 1920.0f, 1080.0f);
}

Title::~Title()
{
}

bool Title::Start()
{
	return true;
}

void Title::Update()
{
	if (g_pad[0]->IsTrigger(enButtonA))
	{
		//NewGO<Load>(0, "Load");
		NewGO<Game>(0, "game");
		DeleteGO(FindGO<Title>("title"));
	}
}

void Title::Render(RenderContext& rc)
{
	m_spriteRender.Draw(rc);
}
