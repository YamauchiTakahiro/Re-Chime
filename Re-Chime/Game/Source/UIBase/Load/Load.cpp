#include "stdafx.h"
#include "Load.h"
#include "Game.h"

Load::Load()
{
	m_Load.Init("Assets/Sprite/Load.DDs", 1920.0f, 1080.0f);
	LoadTime = 10;
}

Load::~Load()
{

}

bool Load::Start()
{
	return true;
}

void Load::Update()
{
	if (LoadTime > 0)
	{
		LoadTime--;
	}
	else
	{
		NewGO<Game>(0, "Game");
		DeleteGO(FindGO<Load>("Load"));
	}
}

void Load::Render(RenderContext& rc)
{
	m_Load.Draw(rc);
}