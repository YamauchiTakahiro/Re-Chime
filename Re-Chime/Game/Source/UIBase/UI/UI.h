#pragma once
#include "Source/UIBase/UIBase.h"
#include "CoolRing.h"
class Player;
class Game;
class DifficultyLevel;
class AudioManager;
class CoolRing;
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

    void ShowGoal(const wchar_t* text);
private:
    Player* m_player = nullptr;
    Game* m_game = nullptr;
    AudioManager* m_audioManager = nullptr;
    CoolRing m_coolRing;
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
    FontRender m_goalText;
    FontRender m_ItemCoolTimeText;
    FontRender m_Cursor;
    Vector3 m_cursorPos;
    float m_goalShowTimer = 5.0f;
    float m_goalAlpha = 1.0f;
    float m_blinkTimer = 0.0f;//点滅タイマー
    float m_goalFade = 1.0f;
    float m_goalTimer = 0.0f;
    float m_inventoryUseCoolTime = 0.0f;
    bool m_isBlinkOn = true;
    bool m_isShowCoolTime = false;
    bool m_isVisible = true;
    bool m_isInventoryOpen = false;
    bool m_isShowGoal = false;
    bool m_isUseItem = false;
    bool m_startItemCoolTime = false;
    int m_selectItem = 0;
    std::wstring m_goalMessage;
};

