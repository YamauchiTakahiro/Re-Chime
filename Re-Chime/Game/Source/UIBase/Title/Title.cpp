#include "stdafx.h"
#include "Title.h"
#include "Game.h"
#include "Source/Manager/AudioManager/AudioManager.h"
#include "Source/UIBase/DifficultyLevel/DifficultyLevel.h"
#include "Source/UIBase/VolumeSettings/VolumeSettings.h"

namespace
{
	float CalcCenterX(const wchar_t* text, float scale)
	{
		int len = wcslen(text);
		float width = len * 45.0f * scale; // ←ここは調整OK
		return 0.0f - width * 0.5f;
	}
}

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
	m_Cursor.SetText(L">");
	m_Cursor.SetScale(1.5f);

	m_cursorPos = Vector3(0, 0, 0);
	m_targetPos = m_cursorPos;
	return true;
}

void Title::Update()
{
	m_moveTime += 0.05f;
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
		if (m_audioManager)
		{
			m_audioManager->PlaySE(enSound_ChoiceSE);
		}
	}

	if (g_pad[0]->IsTrigger(enButtonUp))
	{
		m_SelectNum--;
		if (m_SelectNum < 0) m_SelectNum = 3;

		if (m_audioManager)
		{
			m_audioManager->PlaySE(enSound_ChoiceSE);
		}
	}

	if (g_pad[0]->IsTrigger(enButtonA))
	{
		if (m_audioManager)
		{
			m_audioManager->PlaySE(enSound_DecisionSE);
		}

		switch (m_SelectNum)
		{
		case 0:
			if (m_audioManager)
			{
				m_audioManager->StopBGM(enSound_TitleBGM);
			}
			DeleteGO(this);
			NewGO<DifficultyLevel>(0, "DifficultyLevel");
			break;

		case 1:
			m_isExplanation = true;
			break;

		case 2:
				m_volumeSettings = NewGO<VolumeSettings>(0, "VolumeSettings");
				if (m_volumeSettings)
				{
					m_volumeSettings->SetTitle(this);
				}
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

	// ===== スケール設定 =====
	float scaleStart = (m_SelectNum == 0) ? 1.5f : 1.0f;
	float scaleExp = (m_SelectNum == 1) ? 1.5f : 1.0f;
	float scaleSound = (m_SelectNum == 2) ? 1.5f : 1.0f;
	float scaleExit = (m_SelectNum == 3) ? 1.5f : 1.0f;

	m_Start.SetScale(scaleStart);
	m_Explanation.SetScale(scaleExp);
	m_Sound.SetScale(scaleSound);
	m_Exit.SetScale(scaleExit);

	// ===== 中央揃え（毎フレーム再計算）=====
	float xStart = CalcCenterX(L"ゲームスタート", scaleStart);
	float xExp = CalcCenterX(L"操作説明", scaleExp);
	float xSound = CalcCenterX(L"設定", scaleSound);
	float xExit = CalcCenterX(L"ゲーム終了", scaleExit);

	float yStart = -180.0f;
	float yExp = -270.0f;
	float ySound = -360.0f;
	float yExit = -450.0f;

	m_Start.SetPosition(Vector3(xStart, yStart, 0));
	m_Explanation.SetPosition(Vector3(xExp, yExp, 0));
	m_Sound.SetPosition(Vector3(xSound, ySound, 0));
	m_Exit.SetPosition(Vector3(xExit, yExit, 0));

	// ===== 色変更（選択中）=====
	m_Start.SetColor(g_vec4White);
	m_Explanation.SetColor(g_vec4White);
	m_Sound.SetColor(g_vec4White);
	m_Exit.SetColor(g_vec4White);

	if (m_SelectNum == 0) m_Start.SetColor(1, 0, 0, 1);
	if (m_SelectNum == 1) m_Explanation.SetColor(1, 0, 0, 1);
	if (m_SelectNum == 2) m_Sound.SetColor(1, 0, 0, 1);
	if (m_SelectNum == 3) m_Exit.SetColor(1, 0, 0, 1);

	// ===== カーソル位置 =====
	float cursorX = 0.0f;
	float cursorY = 0.0f;

	switch (m_SelectNum)
	{
	case 0: cursorX = xStart - 60.0f; cursorY = yStart; break;
	case 1: cursorX = xExp - 60.0f; cursorY = yExp;   break;
	case 2: cursorX = xSound - 60.0f; cursorY = ySound; break;
	case 3: cursorX = xExit - 60.0f; cursorY = yExit;  break;
	}

	// 目標位置
	m_targetPos = Vector3(cursorX, cursorY, 0);

	// 補間（スムーズ移動）
	float speed = 0.2f;
	m_cursorPos += (m_targetPos - m_cursorPos) * speed;

	float move = sinf(m_moveTime) * 10.0f;

	m_Cursor.SetPosition(Vector3(
		m_cursorPos.x + move,
		m_cursorPos.y,
		0
	));

	m_Start.Draw(rc);
	m_Explanation.Draw(rc);
	m_Sound.Draw(rc);
	m_Exit.Draw(rc);
	m_Cursor.Draw(rc);
}
