#pragma once
#include "Source/Actor/Character/Enemy/Enemy.h"

class Player;
class Game;

class FinalBoss : public Enemy
{
    public:
    FinalBoss();
    ~FinalBoss();
    bool Start() override;
    void Update() override;
    void Move() override;
    void Rotation() override;
    void Attack() override;
    void OnCollision() override;
    void Time() override;
    void Hit() override;
    void DamageIntarval() override;
    void Dide() override;
    void AttackHit() override;
    Vector3 GetPosition()const override;
    int GetHP(int hp) override
    {
        hp = m_finalBossHp;
        return hp;
    }
    int GetAttackPower(int attackPower)
    {
        attackPower = m_attackPower;
        return attackPower;
	}
    void SetPosition(Vector3 position) override
    {
        m_position = position;
        m_modelRender.SetPosition(position);
    }
    void SetScale(Vector3 scale) override
    {
        m_scale = scale;
        m_modelRender.SetScale(scale);
    }
	virtual void Render(RenderContext& rc)override;

private:
    ModelRender m_modelRender;
    CharacterController m_characterController;
    Vector3 m_position;
    Vector3 m_moveSpeed;
    Quaternion m_rotation;
    Player* m_player = nullptr;
	Game* m_game = nullptr;
    CollisionObject* m_collisionObject = nullptr;
    Vector3 m_forward;
    Vector3 m_scale;
    int m_finalBossHp = 200;		//!<最終ボスのHP。
	int m_attackPower = 20;		//!<攻撃力。
    float m_timeCount = 0.0f;		//!<タイマー用の変数。
	float m_damageIntarvalTime = 0.0f;	//!<ダメージを受けてからの無敵時間。
};

