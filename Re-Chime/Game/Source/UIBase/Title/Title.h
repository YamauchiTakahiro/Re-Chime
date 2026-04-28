#pragma once
#include "Source/UIBase/UIBase.h"
#include "sound/SoundSource.h"

class AudioManager;

class Load;
class DifficultyLevel;
class Title :
    public UIBase
{
public:
    Title();
    ~Title();
    virtual bool Start() override;
    virtual void Update() override;
    virtual void Render(RenderContext& rc)override;
    void SetSetting(bool flag)
    {
        m_isSetting = flag;
    }
    void SetCenter(FontRender& font, const wchar_t* text, float x, float y)
    {
        font.SetText(text);

        float width = wcslen(text) * 40.0f; // 文字の横幅取得
        font.SetPosition(Vector3(x - width * 0.5f, y, 0.0f));
    }
private:
    SoundSource* m_gameBGM;
    AudioManager* m_audioManager;
    FontRender m_Explanation;
    FontRender m_Exit;
    FontRender m_Start;
    FontRender m_Sound;
    FontRender m_Cursor;
    Vector3 m_cursorPos;//現在位置
    Vector3 m_targetPos;//目標位置
    Load* m_load = nullptr;
    DifficultyLevel* m_difficultyLevel = nullptr;
    int m_SelectNum = 0;
    bool m_isExplanation = false;
    bool m_isSetting = false;
    float m_moveTime = 0.0f;
};

