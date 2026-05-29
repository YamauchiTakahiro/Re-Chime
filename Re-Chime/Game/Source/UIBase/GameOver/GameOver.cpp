#include "stdafx.h"
#include "GameOver.h"
#include "Source/UIBase/Title/Title.h"
#include "Source/Manager/AudioManager/AudioManager.h"

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
	return true;
}

void GameOver::Update()
{
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

