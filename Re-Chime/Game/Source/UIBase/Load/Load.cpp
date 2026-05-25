#include "stdafx.h"
#include "Load.h"
#include "Game.h"
#include "Source/Sound/AudioManager/AudioManager.h"
#include "Difficulty.h"

Load::Load()
{
}

Load::~Load()
{
	//DeleteGO(m_LoadBGM);
}

bool Load::Start()
{
	m_Load.Init("Assets/Sprite/Load.DDs", 1920.0f, 1080.0f);
	m_gear.Init("Assets/UIData/gear.DDs", 128.0f, 128.0f);
	m_gear.SetPosition(Vector3(800.0f, -425.0f, 0.0f));
	m_gear.SetScale(Vector3(1.0f, 1.0f, 1.0f));
	m_gear.Update();

	m_gear2.Init("Assets/UIData/gear.DDs", 128.0f, 128.0f);
	m_gear2.SetPosition(Vector3(725.0f, -375.0f, 0.0f));
	m_gear2.SetScale(Vector3(0.8f, 0.8f, 0.8f));
	m_gear2.Update();

	m_gear3.Init("Assets/UIData/gear.DDs", 128.0f, 128.0f);
	m_gear3.SetPosition(Vector3(775.0f, -330.0f, 0.0f));
	m_gear3.SetScale(Vector3(0.5f, 0.5f, 0.5f));
	m_gear3.Update();

	m_BarFrame.Init("Assets/UIData/HP.DDs", 400.0f, 40.0f);
	m_BarFrame.SetPosition(Vector3(-955.0f, -522.5f, 0.0f));
	m_BarFrame.SetPivot(Vector2(0.0f, 0.5f));
	m_BarFrame.Update();

	LoadTime = 350;
	m_GearRotSpeed = 5.0f;

	m_audioManager = FindGO<AudioManager>("audioManager");
	if (m_audioManager)
	{
		m_audioManager->PlayBGM(enSound_LoadBGM, 0.5f);
	}

	m_Font.SetPosition(-955.0f, -450.0f, 0.0f);
	m_Font.SetScale(1.0f);
	m_Font.SetColor(g_vec4White);

	return true;
}

void Load::Update()
{
	if (!m_isFirstRender)
	{
		m_isFirstRender = true;
		return;
	}

	// Game生成
	if (!m_isCreateGame)
	{
		m_game = NewGO<Game>(0, "game");

		m_game->SetDifficulty(m_difficulty);
		m_game->SetLoading(true);

		m_isCreateGame = true;
		return;
	}

	// ロード進行
	if (m_game)
	{
		float target = m_game->GetLoadProgress();

		m_barCurrent += (target - m_barCurrent) * 0.1f;

		Vector3 scale = m_BarFrame.GetScale();

		scale.x = 400.0f * m_barCurrent;

		m_BarFrame.SetScale(scale);

		m_BarFrame.Update();

		// ロード完了
		if (m_game->IsReady())
		{
			m_game->SetLoading(false);

			if (m_audioManager)
			{
				m_audioManager->StopBGM(enSound_LoadBGM);
			}

			DeleteGO(this);
			return;
		}
	}
}
void Load::Render(RenderContext& rc)
{
	m_Load.Draw(rc);
	m_gear.Draw(rc);
	m_BarFrame.Draw(rc);
	m_gear2.Draw(rc);
	m_gear3.Draw(rc);
	m_Font.Draw(rc);
}