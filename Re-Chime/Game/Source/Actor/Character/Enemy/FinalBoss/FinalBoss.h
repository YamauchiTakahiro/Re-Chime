#pragma once
#include "Source/Actor/Character/Enemy/Enemy.h"

class Player;
class Gire;
class Game;
class DamageText;

class FinalBoss : public Enemy
{
    public:
        enum EnFinalBossState {
            enFinalBossState_Idle,		//待機状態。
            enFinalBossState_Chase,		//追跡。
            enFinalBossState_Attack,		//攻撃。
            enFinalBossState_Death,		//死亡。
			enFinalBossState_Num,
		};
    FinalBoss();
    ~FinalBoss();
    bool Start() override;
    void Update() override;
    void Move() override;
    void Rotation() override;
    void Attack() override;
    void OnCollision() override;
    void Hit() override;
    void DamageIntarval() override;
    void Death() override;
    void MakeExplosionEffect();
    void AttackHit() override;
	const bool SearchPlayer()const;
	void ManageState();
	void PlayAnimation();
    void FinalBossState();
	void IdleState();
	void WalkState();
	void AttackState();
	void DeathState();
	const bool IsCanAttack()const;
    void FinalBossHP();
    Vector3 GetPosition()const override;
    int GetHP() const override
    {
        return m_finalBossHp;
    }
    int GetAttackPower() const
    {
        return m_attackPower;
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
    enum EnAnimationClip {		//アニメーション。
        enAnimationClip_Idle,
        enAnimationClip_Walk,
        enAnimationClip_Attack,
        enAnimationClip_Death,
        enAnimationClip_Num,
	};
	AnimationClip m_animationClips[enAnimationClip_Num];
	EnAnimationClip m_currentAnimationClip = enAnimationClip_Num;
	EnFinalBossState m_finalBossState = enFinalBossState_Idle;
    ModelRender m_modelRender;
    CharacterController m_characterController;
    Vector3 m_position;
    Vector3 m_moveSpeed;
    Quaternion m_rotation;
    Player* m_player = nullptr;
	Gire* m_gire = nullptr;
	Game* m_game = nullptr;
    DamageText* m_damageText = nullptr;
    CollisionObject* m_collisionObject = nullptr;
    Vector3 m_forward;
    Vector3 m_scale;
    //int m_finalBossHp = 10;		//!<最終ボスのHP。
    SpriteRender m_bossHPFrame;
    SpriteRender m_bossHPBar;
    bool m_isShowHP = false;
    int m_finalBossHp = 200;		//!<最終ボスのHP。
	int m_finalBossMaxHp = 200;	//!<最終ボスの最大HP。
	int m_attackPower = 20;		//!<攻撃力。
	float m_damageIntarvalTime = 0.0f;	//!<ダメージを受けてからの無敵時間。
    float m_chaseTimer = 0.0f;						//追跡タイマー。
    float m_idleTimer = 0.0f;						//待機タイマー。
	float m_attackCollisionLife = 0.0f;				//攻撃判定の有効時間。
	bool m_isAttack = false;		//!<攻撃しているかどうか。
	bool m_discoveryPlayer = false;	//!<プレイヤーを発見しているかどうか。
    bool m_isShowBossHP = false;
};

