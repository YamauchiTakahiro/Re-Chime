#pragma once
#include "Source/UIBase/UIBase.h"
class Player;
class Game;
class UI :public UIBase
{
    public:
    UI();
    ~UI();
    virtual bool Start() override;
    virtual void Update() override;
	virtual void Render(RenderContext& rc)override;

private:
    Player* m_player = nullptr;
    Game* m_game = nullptr;
};

