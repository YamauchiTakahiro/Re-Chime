#pragma once
#include "Source/Actor/Character/Character.h"
class Player :
    public Character
{
public:
    Player();
    ~Player();
    bool Start() override;
    void Update() override;
    void Move() override;
    void Rotation() override;
    Vector3 GetPosition(Vector3 pos) override
    {
        pos = m_position;
        return pos;
    }
    int GetHP(int hp) override
    {
		hp = m_playerHp;
		return hp;
    }
    void Render(RenderContext& rc)override;



private:
    //メンバ変数
    ModelRender m_modelRender;
    CharacterController m_characterController;
    Vector3 m_moveSpeed;
    Quaternion m_rotation;
    Vector3 m_position;
    bool m_doubleJump = false;		//!<二段ジャンプしたか？
	int m_playerHp = 100;				//!<プレイヤーのHP。
};

