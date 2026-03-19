#include "stdafx.h"
#include "Title.h"
#include "Source/Sound/BGM/BGMManager.h"
#include "Game.h"


Title::Title()
{
	m_Title.Init("Assets/Sprite/title.DDs", 1920.0f, 1080.0f);
	BGMManager* bgm = FindGO<BGMManager>("bgmManager");
	m_gameBGM = bgm->PlayingSound(Sound::enSound_TitleBGM);
}

Title::~Title()
{
	DeleteGO(m_gameBGM);
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
