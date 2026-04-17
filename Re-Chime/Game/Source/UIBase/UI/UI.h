#pragma once
#include "Source/UIBase/UIBase.h"
class Player;
class Game;
class UI :public UIBase
{
    public:
    UI();
    ~UI();
    bool Start() override;
    void Update() override;
	void Render(RenderContext& rc)override;
private:
    Player* m_player = nullptr;
    Game* m_game = nullptr;
    float m_blinkTimer = 0.0f;//点滅タイマー
    bool m_isBlinkOn = true;
};

