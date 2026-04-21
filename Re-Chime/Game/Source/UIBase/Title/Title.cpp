#include "stdafx.h"
#include "Title.h"
#include "Game.h"
#include "Source/Sound/AudioManager/AudioManager.h"


Title::Title()
{
}

Title::~Title()
{
}

bool Title::Start()
{
	m_Title.Init("Assets/Sprite/Title.DDs", 1920.0f, 1080.0f);
	m_explanation.Init("Assets/Sprite/explanation.DDs", 1920.0f, 1080.0f);

	m_audioManager = FindGO<AudioManager>("audioManager");
	if (m_audioManager)
	{
		m_audioManager->PlayBGM(enSound_TitleBGM, 0.5f);
	}

	return true;
}

void Title::Update()
{
	if (g_pad[0]->IsTrigger(enButtonA))
	{
		NewGO<Load>(0, "Load");
		if (m_audioManager)
		{
			m_audioManager->StopBGM();
		}
		DeleteGO(this);
	}
}

void Title::Render(RenderContext& rc)
{
	m_Title.Draw(rc);
}
