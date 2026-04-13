#pragma once
#include "Source/Actor/Character/Character.h"

class Gire;

class Player : public Character
{
public:
    Player();
    ~Player();
    bool Start() override;
    void Update() override;
    void Move() override;
    void Rotation() override;
	void Attack() override;
	void OnCollision() override;
	void Time() override;
	void Hit() override;
	void DamageIntarval() override;
    void GetGier();
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
    int GetMaxHP(int maxhp)
    {
        maxhp = m_playerMaxHp;
        return maxhp;
    }
    bool GetGier(bool getGier)
    {
        getGier = m_isGetGire;
		return getGier;
    }
    void Render(RenderContext& rc)override;


private:
    //メンバ変数
    ModelRender m_modelRender;
    CharacterController m_characterController;
    Vector3 m_moveSpeed;
    Quaternion m_rotation;
    Vector3 m_position;
    CollisionObject* m_collisionObject;
    Vector3 m_forward;
	Gire* m_gire = nullptr;
    int m_playerHp = 100;				//!<プレイヤーのHP。
	int m_playerMaxHp = 100;				//!<プレイヤーの最大HP。
	float m_timeCount = 0.0f;				//!<タイマー用の変数。
	float m_damageIntarvalTime = 0.0f;		//!<ダメージを受けてからの無敵時間。
    int m_gireCount;
	bool m_isGetGire = false;				//!<ギアを取ったかどうか。
};

