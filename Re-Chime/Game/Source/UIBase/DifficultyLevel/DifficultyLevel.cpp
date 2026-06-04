#include "stdafx.h"
#include "Source/UIBase/DifficultyLevel/DifficultyLevel.h"
#include "Source/UIBase/Load/Load.h"
#include "Source/UIBase/Title/Title.h"
#include "Game.h"
#include "Difficulty.h"
#include "Source/Manager/AudioManager/AudioManager.h"

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

	m_Easy.Init("Assets/Sprite/EASY.DDs",720.0f,210.0f);
	m_Easy.SetPosition(Vector3(0, 390, 0));
	m_Easy.Update();

	m_Normal.Init("Assets/Sprite/NORMAL.DDs",720.0f,210.0f);
	m_Normal.SetPosition(Vector3(0, 130, 0));
	m_Normal.Update();

	m_Hard.Init("Assets/Sprite/HARD.DDs",720.0f,210.0f);
	m_Hard.SetPosition(Vector3(0, -130, 0));
	m_Hard.Update();

	m_Lunatic.Init("Assets/Sprite/LUNATIC.DDs",720.0f,210.0f);
	m_Lunatic.SetPosition(Vector3(0, -390, 0));
	m_Lunatic.Update();

	m_Cursor.SetText(L">");
	m_Cursor.SetScale(2.0f);

	return true;
}

void DifficultyLevel::Update()
{
	m_animTime += g_gameTime->GetFrameDeltaTime();

	DifficultySelect();

	float animScale = 1.0f + sinf(m_animTime * 5.0f) * 0.1f;

	float scaleEasy = (m_select == 0) ? animScale : 1.0f;
	float scaleNormal = (m_select == 1) ? animScale : 1.0f;
	float scaleHard = (m_select == 2) ? animScale : 1.0f;
	float scaleLunatic = (m_select == 3) ? animScale : 1.0f;

	m_Easy.SetScale(Vector3(scaleEasy, scaleEasy, 1));
	m_Normal.SetScale(Vector3(scaleNormal, scaleNormal, 1));
	m_Hard.SetScale(Vector3(scaleHard, scaleHard, 1));
	m_Lunatic.SetScale(Vector3(scaleLunatic, scaleLunatic, 1));

	m_Easy.Update();
	m_Normal.Update();
	m_Hard.Update();
	m_Lunatic.Update();

	float cursorY = -180.0f;

	switch (m_select)
	{
	case 0:
		cursorY = 430.0f;
		break;

	case 1:
		cursorY = 170.0f;
		break;

	case 2:
		cursorY = -90.0f;
		break;

	case 3:
		cursorY = -350.0f;
		break;
	}

	float cursorX =-480.0f +sinf(m_animTime * 6.0f) * 15.0f;
	
	m_Cursor.SetPosition(Vector3(cursorX,cursorY,0.0f));
	
	if (g_pad[0]->IsTrigger(enButtonA))
	{
		auto* load = NewGO<Load>(0, "Load");
		load->SetDifficulty(m_difficulty);
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

		if (m_audioManager)
		{
			m_audioManager->PlaySE(enSound_ChoiceSE);
		}
	}
	if (g_pad[0]->IsTrigger(enButtonDown))
	{
		m_select++;
		if (m_select > 3) m_select = 0;

		if (m_audioManager)
		{
			m_audioManager->PlaySE(enSound_ChoiceSE);
		}
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
	m_Easy.Draw(rc);
	m_Normal.Draw(rc);
	m_Hard.Draw(rc);
	m_Lunatic.Draw(rc);
	m_Cursor.Draw(rc);
}