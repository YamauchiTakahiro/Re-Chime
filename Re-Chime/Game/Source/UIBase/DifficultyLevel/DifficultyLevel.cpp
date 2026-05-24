#include "stdafx.h"
#include "Source/UIBase/DifficultyLevel/DifficultyLevel.h"
#include "Source/UIBase/Load/Load.h"
#include "Source/UIBase/Title/Title.h"
#include "Game.h"

namespace
{
	float CalcCenterX(const wchar_t* text, float scale)
	{
		int len = wcslen(text);
		float width = len * 45.0f * scale; // ←ここは調整OK
		return 0.0f - width * 0.5f;
	}
}

DifficultyLevel::DifficultyLevel()
{
}

DifficultyLevel::~DifficultyLevel()
{
}

bool DifficultyLevel::Start()
{
	m_DifficultyLevel.Init("Assets/Sprite/DifficultyLevel.DDs", 1920.0f, 1080.0f);

	m_Easy.SetText(L"Easy");
	m_Normal.SetText(L"Normal");
	m_Hard.SetText(L"Hard");
	m_Lunatic.SetText(L"Lunatic");
	return true;
}

void DifficultyLevel::Update()
{
	DifficultySelect();

	if (g_pad[0]->IsTrigger(enButtonA))
	{
		Game* game = FindGO<Game>("game");

		if (game != nullptr)
		{
			game->SetDifficulty(
				static_cast<Game::Difficulty>(m_difficulty)
			);
		}

		NewGO<Load>(0, "Load");

		DeleteGO(this);
	}
	if(g_pad[0]->IsTrigger(enButtonB))
	{
		NewGO<Title>(0, "Title");
		DeleteGO(this);
	}

}

void DifficultyLevel::DifficultySelect()
{
	if (g_pad[0]->IsTrigger(enButtonUp))
	{
		m_select--;
		if (m_select < 0) m_select = 3;
	}
	if (g_pad[0]->IsTrigger(enButtonDown))
	{
		m_select++;
		if (m_select > 3) m_select = 0;
	}

	if(m_select == 0)
	{
		// イージーの処理
		m_difficulty = EASY;
	}
	else if(m_select == 1)
	{
		// ノーマルの処理
		m_difficulty = NORMAL;
	}
	else if(m_select == 2)
	{
		// ハードの処理
		m_difficulty = HARD;
	}
	else if(m_select == 3)
	{
		// ルナティックの処理
		m_difficulty = LUNATIC;
	}
}

void DifficultyLevel::Render(RenderContext& rc)
{
	m_DifficultyLevel.Draw(rc);

	float scaleEasy = (m_select == 0) ? 1.5f : 1.0f;
	float scaleNormal = (m_select == 1) ? 1.5f : 1.0f;
	float scaleHard = (m_select == 2) ? 1.5f : 1.0f;
	float scaleLunatic = (m_select == 3) ? 1.5f : 1.0f;

	m_Easy.SetScale(scaleEasy);
	m_Normal.SetScale(scaleNormal);
	m_Hard.SetScale(scaleHard);
	m_Lunatic.SetScale(scaleLunatic);

	float xEasy = CalcCenterX(L"Easy", scaleEasy);
	float xNormal = CalcCenterX(L"Normal", scaleNormal);
	float xHard = CalcCenterX(L"Hard", scaleHard);
	float xLunatic = CalcCenterX(L"Lunatic", scaleLunatic);

	float yEasy = -180.0f;
	float yNormal = -270.0f;
	float yHard = -360.0f;
	float yLunatic = -450.0f;

	m_Easy.SetPosition(Vector3(xEasy, yEasy	, 0));
	m_Normal.SetPosition(Vector3(xNormal, yNormal, 0));
	m_Hard.SetPosition(Vector3(xHard, yHard, 0));
	m_Lunatic.SetPosition(Vector3(xLunatic, yLunatic, 0));

	// ===== 色変更（選択中）=====
	m_Easy.SetColor(g_vec4White);
	m_Normal.SetColor(g_vec4White);
	m_Hard.SetColor(g_vec4White);
	m_Lunatic.SetColor(g_vec4White);

	if (m_select == 0) m_Easy.SetColor(1, 0, 0, 1);
	if (m_select == 1) m_Normal.SetColor(1, 0, 0, 1);
	if (m_select == 2) m_Hard.SetColor(1, 0, 0, 1);
	if (m_select == 3) m_Lunatic.SetColor(1, 0, 0, 1);

	m_Easy.Draw(rc);
	m_Normal.Draw(rc);
	m_Hard.Draw(rc);
	m_Lunatic.Draw(rc);
}