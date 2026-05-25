#include "stdafx.h"
#include "VolumeSettings.h"
#include "Game.h"
#include "Source/Actor/Character/Player/Player.h"
#include "Source/Actor/Character/Enemy/SmallRobot/SmallRobot.h"
#include "Source/Camera/GameCamera.h"
#include "Source/UIBase/UI/UI.h"
#include "Source/Sound/AudioManager/AudioManager.h"
#include "Source/UIBase/Title/Title.h"

VolumeSettings::VolumeSettings()
{

}

VolumeSettings::~VolumeSettings()
{

}

bool VolumeSettings::Start()
{

    m_VolumeSettings.Init("Assets/Sprite/Pause.DDs", 1920.0f, 1080.0f);

    // ===== レイアウト設定 =====
    float baseY = 40.0f;   // ← 全体の中心
    float space = 180.0f;  // ← 間隔
    float barOffset = -30.0f;

    // ===== MASTER =====
    m_MasterBar.Init("Assets/UIData/HP.DDs", 1024.0f, 128.0f);
    m_MasterBar.SetPosition(Vector3(-450.0f, baseY + space + barOffset, 0.0f));
    m_MasterBar.SetPivot(Vector2(0.0f, 0.5f));
    m_MasterBar.SetScale(Vector3(m_masterVolume, 0.8f, 1.0f));

    m_MasterText.SetText(L"MASTER");
    m_MasterText.SetPosition(Vector3(-750.0f, baseY + space + 20.0f, 0.0f));
    m_MasterText.SetScale(2.0f);

    m_MasterValueText.SetScale(2.0f);

    // ===== BGM =====
    m_BGMBar.Init("Assets/UIData/HP.DDs", 1024.0f, 128.0f);
    m_BGMBar.SetPosition(Vector3(-450.0f, baseY + barOffset, 0.0f));
    m_BGMBar.SetPivot(Vector2(0.0f, 0.5f));
    m_BGMBar.SetScale(Vector3(m_bgmVolume, 0.8f, 1.0f));

    m_BGMText.SetText(L"BGM");
    m_BGMText.SetPosition(Vector3(-750.0f, baseY + 20.0f, 0.0f));
    m_BGMText.SetScale(2.0f);

    m_BGMValueText.SetScale(2.0f);

    // ===== SE =====
    m_SEBar.Init("Assets/UIData/HP.DDs", 1024.0f, 128.0f);
    m_SEBar.SetPosition(Vector3(-450.0f, baseY - space + barOffset, 0.0f));
    m_SEBar.SetPivot(Vector2(0.0f, 0.5f));
    m_SEBar.SetScale(Vector3(m_seVolume, 0.8f, 1.0f));

    m_SEText.SetText(L"SE");
    m_SEText.SetPosition(Vector3(-750.0f, baseY - space + 20.0f, 0.0f));
    m_SEText.SetScale(2.0f);

    m_SEValueText.SetScale(2.0f);

    // ===== 戻るテキスト =====
    m_BackText.SetText(L"Bボタンで戻る");
    m_BackText.SetPosition(Vector3(500.0f, -450.0f, 0.0f));
    m_BackText.SetScale(1.2f);
    m_BackText.SetColor(g_vec4White);

    // ===== カーソル =====
    m_Cursor.SetText(L">");
    m_Cursor.SetScale(1.5f);
    m_Cursor.SetColor(g_vec4White);

    m_cursorPos = Vector3(0, 0, 0);
    m_targetPos = m_cursorPos;

    m_audioManager = FindGO<AudioManager>("audioManager");

    if (m_audioManager)
    {
        m_masterVolume = m_audioManager->GetMasterVolume();
        m_bgmVolume = m_audioManager->GetBGMVolume();
        m_seVolume = m_audioManager->GetSEVolume();
    }

    return true;
}

void VolumeSettings::Update()
{
    float baseY = 40.0f;
    float space = 180.0f;
    float barOffset = -30.0f;
    float valueOffset = 40.0f;

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
    m_BGMBar.SetScale(Vector3(m_bgmVolume, 0.8f, 1.0f));
    m_BGMBar.Update();

    m_SEBar.SetScale(Vector3(m_seVolume, 0.8f, 1.0f));
    m_SEBar.Update();

    m_MasterBar.SetScale(Vector3(m_masterVolume, 0.8f, 1.0f));
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
    SetRightAlign(m_MasterValueText, masterText, 700.0f, baseY + space + barOffset + valueOffset);
    SetRightAlign(m_BGMValueText, bgmText, 700.0f, baseY + barOffset + valueOffset);
    SetRightAlign(m_SEValueText, seText, 700.0f, baseY - space + barOffset + valueOffset);

    // 戻る
    if (g_pad[0]->IsTrigger(enButtonB))
    {
        if (m_audioManager)
        {
            m_audioManager->SaveVolume();
        }

        Title* title = FindGO<Title>("Title");

        if (m_title)
        {
            m_title->SetSetting(false);
        }

        Game* game = FindGO<Game>("game");
        if (game)
        {
            game->SetSetting(false);
        }
        DeleteGO(this);
    }
    Cursor();
}

void VolumeSettings::Cursor()
{
    float baseY = 0.0f;
    float offset = 180.0f;

    float cursorX = 0.0f;
    float cursorY = 0.0f;

    switch (m_select)
    {
    case 0:
        cursorX = m_MasterText.GetPosition().x - 80.0f;
        cursorY = m_MasterText.GetPosition().y;
        break;

    case 1:
        cursorX = m_BGMText.GetPosition().x - 80.0f;
        cursorY = m_BGMText.GetPosition().y;
        break;

    case 2:
        cursorX = m_SEText.GetPosition().x - 80.0f;
        cursorY = m_SEText.GetPosition().y;
        break;
    }

    m_Cursor.SetPosition(m_cursorPos);

    float targetX = 0.0f;
    float targetY = 0.0f;

    switch (m_select)
    {
    case 0:
        targetX = m_MasterText.GetPosition().x - 80.0f;
        targetY = m_MasterText.GetPosition().y;
        break;

    case 1:
        targetX = m_BGMText.GetPosition().x - 80.0f;
        targetY = m_BGMText.GetPosition().y;
        break;

    case 2:
        targetX = m_SEText.GetPosition().x - 80.0f;
        targetY = m_SEText.GetPosition().y;
        break;
    }

    m_targetPos = Vector3(targetX, targetY, 0);

    float speed = 0.2f; // ← 0.1〜0.3で調整

    m_cursorPos += (m_targetPos - m_cursorPos) * speed;

    // 揺れ幅と速さ
    float amplitude = 10.0f;     // 揺れの大きさ（左右の幅）
    float Speed = 5.0f;          // 揺れる速さ

    static float time = 0.0f;
    time += 0.016f;

    float offsetX = sinf(time * Speed) * amplitude;

    // 最終位置
    m_Cursor.SetPosition(Vector3(
        m_cursorPos.x + offsetX,
        m_cursorPos.y,
        0.0f
    ));
}


void VolumeSettings::Render(RenderContext& rc)
{
    m_VolumeSettings.SetMulColor({ 1,1,1,0.4f });
    m_VolumeSettings.Draw(rc);

    // 色リセット
    m_MasterText.SetColor(g_vec4White);
    m_BGMText.SetColor(g_vec4White);
    m_SEText.SetColor(g_vec4White);

    m_MasterValueText.SetColor(g_vec4White);
    m_BGMValueText.SetColor(g_vec4White);
    m_SEValueText.SetColor(g_vec4White);

    // 選択中を赤く
    if (m_select == 0)
    {
        m_MasterText.SetColor(1, 0, 0, 1);
        m_MasterValueText.SetColor(1, 0, 0, 1);
    }
    if (m_select == 1)
    {
        m_BGMText.SetColor(1, 0, 0, 1);
        m_BGMValueText.SetColor(1, 0, 0, 1);
    }
    if (m_select == 2)
    {
        m_SEText.SetColor(1, 0, 0, 1);
        m_SEValueText.SetColor(1, 0, 0, 1);
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

    m_Cursor.Draw(rc);
}