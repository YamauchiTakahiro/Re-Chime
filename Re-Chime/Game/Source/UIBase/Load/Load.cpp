#include "stdafx.h"
#include "Load.h"
#include "Game.h"
#include "Source/Sound/AudioManager/AudioManager.h"

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
	m_loadTimer += g_gameTime->GetFrameDeltaTime();

	if (m_loadTimer >= m_loadMax)
	{
		NewGO<Game>(0, "game");
		if (m_audioManager)
		{
			m_audioManager->StopBGM();
		}
		DeleteGO(this);
		return;
	}
	//ギアの回転
	m_GearRotation.SetRotationZ(m_loadTimer * m_GearRotSpeed);
	m_gear.SetRotation(m_GearRotation);
	m_gear.Update();

	m_GearRotation2.SetRotationZ(m_loadTimer * -m_GearRotSpeed);
	m_gear2.SetRotation(m_GearRotation2);
	m_gear2.Update();

	m_GearRotation3.SetRotationZ(m_loadTimer * m_GearRotSpeed);
	m_gear3.SetRotation(m_GearRotation3);
	m_gear3.Update();

	//ローディングバーの拡大
	Vector3 scale = m_BarFrame.GetScale();
	scale.x = m_BarFrame.GetScale().x + 0.7f * g_gameTime->GetFrameDeltaTime();
	m_BarFrame.SetScale(scale);
	m_BarFrame.Update();

	//ドット更新
	m_dotTimer += g_gameTime->GetFrameDeltaTime();

	if (m_dotTimer >= 0.3f)
	{
		m_dotTimer = 0.0f;
		m_dotCount++;

		if (m_dotCount > 3)
		{
			m_dotCount = 0;
		}
	}

	// 文字作成
	std::wstring text = L"読み込み中";

	// ドット追加
	for (int i = 0; i < m_dotCount; i++)
	{
		text += L"・";
	}

	m_Font.SetText(text.c_str());
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