#include "stdafx.h"
#include "Title.h"


Title::Title()
{
	m_Title.Init("Assets/Sprite/title.DDs", 1920.0f, 1080.0f);
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
		NewGO<Load>(0, "Load");
		DeleteGO(FindGO<Title>("title"));
	}
}

void Title::Render(RenderContext& rc)
{
	m_Title.Draw(rc);
}
