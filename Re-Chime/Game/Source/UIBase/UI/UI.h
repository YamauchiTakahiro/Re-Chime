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
    void Inventory();
	void Render(RenderContext& rc)override;
    void SetVisible(bool flag)
    {
        m_isVisible = flag;
    }

    bool IsInventoryOpen() const
    {
        return m_isInventoryOpen;
    }
private:
    Player* m_player = nullptr;
    Game* m_game = nullptr;
    SpriteRender m_inventory;
    SpriteRender m_PS1;
    SpriteRender m_PS2;
    SpriteRender m_PS3;
    SpriteRender m_selectFrame;
    SpriteRender m_Inventoryback;
    Quaternion m_inventoryRotation;
    FontRender m_CoolTimeText;
    FontRender m_pickUpText;
    FontRender m_PS1CountText;
    FontRender m_PS2CountText;
    FontRender m_PS3CountText;
    float m_blinkTimer = 0.0f;//点滅タイマー
    bool m_isBlinkOn = true;
    bool m_isShowCoolTime = false;
    bool m_isVisible = true;
    int m_selectItem = 0;
    bool m_isInventoryOpen = false;
};

