#include "stdafx.h"
#include "Title.h"
#include "Game.h"
#include "Source/Sound/AudioManager/AudioManager.h"
#include "Source/UIBase/Load/Load.h"
#include "Source/UIBase/DifficultyLevel/DifficultyLevel.h"

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
	m_Start.SetText(L"ゲームスタート");
	m_Explanation.SetText(L"操作説明");
	m_Sound.SetText(L"設定");
	m_Exit.SetText(L"ゲーム終了");

	m_Start.SetPosition(Vector3(-200.0f, -180.0f, 0.0f));
	m_Explanation.SetPosition(Vector3(-200.0f, -270.0f, 0.0f));
	m_Sound.SetPosition(Vector3(-200.0f, -360.0f, 0.0f));
	m_Exit.SetPosition(Vector3(-200.0f, -450.0f, 0.0f));

	return true;
}

void Title::Update()
{
	if (m_isSetting)
	{
		return;
	}
	if (m_isExplanation)
	{
		if (g_pad[0]->IsTrigger(enButtonB))
		{
			m_isExplanation = false;
		}
		return;
	}
	if (g_pad[0]->IsTrigger(enButtonDown))
	{
		m_SelectNum++;
		if (m_SelectNum > 3) m_SelectNum = 0;
	}

	if (g_pad[0]->IsTrigger(enButtonUp))
	{
		m_SelectNum--;
		if (m_SelectNum < 0) m_SelectNum = 3;
	}

	if (g_pad[0]->IsTrigger(enButtonA))
	{
		//DeleteGO(this);
		switch (m_SelectNum)
		{
		case 0:
			NewGO<Load>(0, "Load");
			if (m_audioManager)
			{
				m_audioManager->StopBGM();
			}
			DeleteGO(this);
			break;

		case 1:
			m_isExplanation = true;
			break;

		case 2:
				m_difficultyLevel = NewGO<DifficultyLevel>(0, "DifficultyLevel");
				m_difficultyLevel->SetTitle(this);
				m_isSetting = true;
				break;
		case 3:
			PostQuitMessage(0);
			break;
		}
	}
}

void Title::Render(RenderContext& rc)
{
	m_Title.Draw(rc);

	if (m_isExplanation)
	{
		m_explanation.Draw(rc);
		return;
	}

	m_Start.SetScale(1.0f);
	m_Explanation.SetScale(1.0f);
	m_Sound.SetScale(1.0f);
	m_Exit.SetScale(1.0f);

	// 選択中だけ大きくする
	if (m_SelectNum == 0) m_Start.SetScale(1.5f);
	if (m_SelectNum == 1) m_Explanation.SetScale(1.5f);
	if (m_SelectNum == 2) m_Sound.SetScale(1.5f);
	if (m_SelectNum == 3) m_Exit.SetScale(1.5f);

	m_Start.Draw(rc);
	m_Explanation.Draw(rc);
	m_Sound.Draw(rc);
	m_Exit.Draw(rc);
}
