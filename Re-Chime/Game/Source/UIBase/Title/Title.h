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
private:
    SoundSource* m_gameBGM;
    AudioManager* m_audioManager;
    FontRender m_Explanation;
    FontRender m_Exit;
    FontRender m_Start;
    FontRender m_Sound;
    Load* m_load = nullptr;
    DifficultyLevel* m_difficultyLevel = nullptr;
    int m_SelectNum = 0;
    bool m_isExplanation = false;
};

