#include "stdafx.h"
#include "DifficultyLevel.h"
#include "Game.h"
#include "Source/Actor/Character/Player/Player.h"
#include "Source/Actor/Character/Enemy/SmallRobot/SmallRobot.h"
#include "Source/Camera/GameCamera.h"
#include "Source/UIBase/UI/UI.h"
#include "Source/Sound/AudioManager/AudioManager.h"
#include "Source/UIBase/Title/Title.h"

DifficultyLevel::DifficultyLevel()
{

}

DifficultyLevel::~DifficultyLevel()
{

}

bool DifficultyLevel::Start()
{
    // ===== BGMバー =====
    m_DifficultyLevel.Init("Assets/Sprite/Pause.DDs", 1920.0f, 1080.0f);
    m_BGMBar.Init("Assets/UIData/HP.DDs", 1024.0f, 128.0f);
    m_BGMBar.SetPosition(Vector3(-450.0f, 100.0f, 0.0f));
    m_BGMBar.SetPivot(Vector2(0.0f, 0.5f));
    m_BGMBar.SetScale(Vector3(m_bgmVolume, 0.6f, 1.0f));

    // ===== SEバー =====
    m_SEBar.Init("Assets/UIData/HP.DDs", 1024.0f, 128.0f);
    m_SEBar.SetPosition(Vector3(-450.0f, -50.0f, 0.0f));
    m_SEBar.SetPivot(Vector2(0.0f, 0.5f));
    m_SEBar.SetScale(Vector3(m_seVolume, 0.6f, 1.0f));

    // ===== MASTERバー =====
    m_MasterBar.Init("Assets/UIData/HP.DDs", 1024.0f, 128.0f);
    m_MasterBar.SetPosition(Vector3(-450.0f, 250.0f, 0.0f));
    m_MasterBar.SetPivot(Vector2(0.0f, 0.5f));
    m_MasterBar.SetScale(Vector3(m_masterVolume, 0.6f, 1.0f));

    // テキスト
    m_BGMText.SetText(L"BGM");
    m_BGMText.SetPosition(Vector3(-750.0f, 120.0f, 0.0f));
    m_BGMText.SetScale(1.5f);

    m_BGMValueText.SetPosition(Vector3(650.0f, 100.0f, 0.0f));
    m_BGMValueText.SetScale(1.2f);

    m_SEText.SetText(L"SE");
    m_SEText.SetPosition(Vector3(-750.0f, -30.0f, 0.0f));
    m_SEText.SetScale(1.5f);

    m_SEValueText.SetPosition(Vector3(650.0f, -50.0f, 0.0f));
    m_SEValueText.SetScale(1.2f);

    m_MasterText.SetText(L"MASTER");
    m_MasterText.SetPosition(Vector3(-750.0f, 270.0f, 0.0f));
    m_MasterText.SetScale(1.5f);

    m_MasterValueText.SetPosition(Vector3(650.0f, 250.0f, 0.0f));
    m_MasterValueText.SetScale(1.2f);

    m_BackText.SetText(L"Bボタンで戻る");
    m_BackText.SetPosition(Vector3(500.0f, -400.0f, 0.0f)); // 右下あたり
    m_BackText.SetScale(1.0f);
    m_BackText.SetColor(g_vec4White);

	m_audioManager = FindGO<AudioManager>("audioManager");
	return true;
}

void DifficultyLevel::Update()
{
    // ===== 選択切り替え =====
    if (g_pad[0]->IsTrigger(enButtonUp))
    {
        m_select--;
        if (m_select < 0) m_select = 2;
    }
    if (g_pad[0]->IsTrigger(enButtonDown))
    {
        m_select++;
        if (m_select > 2) m_select = 0;
    }

    // ===== 音量変更 =====
    if (g_pad[0]->IsPress(enButtonRight))
    {
        if (m_select == 0) m_masterVolume += 0.01f;
        if (m_select == 1) m_bgmVolume += 0.01f;
        if (m_select == 2) m_seVolume += 0.01f;
    }

    if (g_pad[0]->IsPress(enButtonLeft))
    {
        if (m_select == 0) m_masterVolume -= 0.01f;
        if (m_select == 1) m_bgmVolume -= 0.01f;
        if (m_select == 2) m_seVolume -= 0.01f;
    }

    // Clamp
    if (m_masterVolume < 0) m_masterVolume = 0;
    if (m_masterVolume > 1) m_masterVolume = 1;

    if (m_bgmVolume < 0) m_bgmVolume = 0;
    if (m_bgmVolume > 1) m_bgmVolume = 1;

    if (m_seVolume < 0) m_seVolume = 0;
    if (m_seVolume > 1) m_seVolume = 1;

    // Audio反映
    if (m_audioManager)
    {
        m_audioManager->SetMasterVolume(m_masterVolume);
        m_audioManager->SetBGMVolume(m_bgmVolume);
        m_audioManager->SetSEVolume(m_seVolume);
    }

    // ゲージ更新
    m_BGMBar.SetScale(Vector3(m_bgmVolume, 0.6f, 1.0f));
    m_BGMBar.Update();

    m_SEBar.SetScale(Vector3(m_seVolume, 0.6f, 1.0f));
    m_SEBar.Update();

    m_MasterBar.SetScale(Vector3(m_masterVolume, 0.6f, 1.0f));
    m_MasterBar.Update();

    auto SetRightAlign = [&](FontRender& font, const std::wstring& text, float rightX, float y)
        {
            float charWidth = 18.0f; // 調整ポイント
            float width = text.length() * charWidth;

            font.SetPosition(Vector3(rightX - width, y, 0.0f));
            font.SetText(text.c_str());
        };

    // 0～100に変換
    int master = static_cast<int>(m_masterVolume * 100);
    int bgm = static_cast<int>(m_bgmVolume * 100);
    int se = static_cast<int>(m_seVolume * 100);

    // 文字列作成
    std::wstring masterText = std::to_wstring(master) + L"%%";
    std::wstring bgmText = std::to_wstring(bgm) + L"%%";
    std::wstring seText = std::to_wstring(se) + L"%%";

    // セット
    SetRightAlign(m_MasterValueText, masterText, 700.0f, 250.0f);
    SetRightAlign(m_BGMValueText, bgmText, 700.0f, 100.0f);
    SetRightAlign(m_SEValueText, seText, 700.0f, -50.0f);

    // 戻る
    if (g_pad[0]->IsTrigger(enButtonB))
    {
        if (m_title)
        {
            m_title->SetSetting(false);
        }

        Game* game = FindGO<Game>("game");
        if (game)
        {
            game->SetSetting(false);
            game->SetDifficulty(nullptr);
        }


        DeleteGO(this);
    }
}

void DifficultyLevel::Render(RenderContext& rc)
{
    m_DifficultyLevel.SetMulColor({ 1,1,1,0.4f });
    m_DifficultyLevel.Draw(rc);

    // 色リセット
    m_MasterText.SetColor(g_vec4White);
    m_BGMText.SetColor(g_vec4White);
    m_SEText.SetColor(g_vec4White);

    // 選択中を赤く
    if (m_select == 0)
    {
        m_MasterText.SetColor(1, 0, 0, 1);
    }
    if (m_select == 1)
    {
        m_BGMText.SetColor(1, 0, 0, 1);
    }
    if (m_select == 2)
    {
        m_SEText.SetColor(1, 0, 0, 1);
    }

    m_MasterBar.Draw(rc);
    m_BGMBar.Draw(rc);
    m_SEBar.Draw(rc);

    m_MasterText.Draw(rc);
    m_BGMText.Draw(rc);
    m_SEText.Draw(rc);

    m_MasterValueText.Draw(rc);
    m_BGMValueText.Draw(rc);
    m_SEValueText.Draw(rc);

    m_BackText.Draw(rc);
}
