#include "stdafx.h"
#include "GameOver.h"
#include "Source/UIBase/Title/Title.h"
#include "Source/Manager/AudioManager/AudioManager.h"
#include "Source/UIBase/Fade/Fade.h"

GameOver::GameOver()
{
}

GameOver::~GameOver()
{

}

bool GameOver::Start()
{
	m_GameOver.Init("Assets/Sprite/GameOver.DDs", 1920.0f, 1080.0f);
	m_audioManager = FindGO<AudioManager>("audioManager");
	if (m_audioManager)
	{
		m_audioManager->PlayBGM(enSound_GameOverBGM, 0.5f);
	}

	m_fade = NewGO<Fade>(0, "fade");
	m_fade->SetAlpha(1.0f);
	m_fade->StartFadeIn();
	return true;
}

void GameOver::Update()
{
	if (m_fade && m_fade->IsFade())
	{
		return;
	}

	if (g_pad[0]->IsTrigger(enButtonA))
	{
		NewGO<Title>(0, "title");
		if (m_audioManager)
		{
			m_audioManager->StopBGM(enSound_GameOverBGM);
		}
		DeleteGO(this);
	}
}

void GameOver::Render(RenderContext& rc)
{
	m_GameOver.Draw(rc);
}

