#pragma once
#include "Source/Actor/Character/Enemy/Enemy.h"

class Player;
class AttackSpeedBuff;
class PowerBuff;
class Game;

class MediumRobot : public Enemy
{
 public:
    enum enMediumRobotState
    {
        enMediumRobotState_Idle,		//待機状態。
        enMediumRobotState_Walk,		//移動状態。
		enMediumRobotState_Attack,	//攻撃状態。
		enMediumRobotState_Death,		//死亡状態。
        enMediumRobotState_Num,
	};
    MediumRobot();
    ~MediumRobot();
    bool Start() override;
    void Update() override;
    void Move() override;
    void Rotation() override;
    void Attack() override;
    void OnCollision() override;
    void Time() override;
    void Hit() override;
    void DamageIntarval() override;
    void AttackHit() override;
    void Death() override;
	void MakeExplosionEffect();
	void ManageState();
	void PlayAnimation();
	void MediumRobotState();
	void IdleState();
    void WalkState();
	//void AttackState();
	void DeathState();
    void MediumRobotHP();
    Vector3 GetPosition()const override;
    int GetHP()const override
    {
        return m_mediumRobotHp;
    }
    int GetAttackPower()const
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
    enum enAnimationClip {		//アニメーション。
        enAnimationClip_Idle,
        enAnimationClip_Walk,
        enAnimationClip_Death,
        enAnimationClip_Num,
	};
    AnimationClip m_animationClips[enAnimationClip_Num];	//!<アニメーションクリップ。
	enMediumRobotState m_mediumRobotState = enMediumRobotState_Idle;	//!<中型ロボットの状態。
    ModelRender m_modelRender;
    CharacterController m_characterController;
    Vector3 m_position;
    Vector3 m_moveSpeed;
    Quaternion m_rotation;
    Player* m_player = nullptr;
    AttackSpeedBuff* m_attackSpeedBuff = nullptr;
    PowerBuff* m_powerBuff = nullptr;
    CollisionObject* m_collisionObject = nullptr;
    Game* m_game = nullptr;
    Vector3 m_forward;
    Vector3 m_scale;
    //int m_mediumRobotHp = 10;		//!<中型ロボットのHP。
    SpriteRender m_enemyHP;//敵のHPを表示する。
    SpriteRender m_enemyHPFrame;
    Vector2 m_enemyHPBarPosition = Vector2::Zero;
    Vector2 m_enemyHPFramePosition = Vector2::Zero;
    bool m_isShowHP = false;
    int m_mediumRobotHp = 50;		//!<中型ロボットのHP。
	int m_mediumRobotMaxHp = 50;	//!<中型ロボットの最大HP。
	int m_attackPower = 10;		//!<攻撃力。
    float m_timeCount = 0.0f;		//!<タイマー用の変数。
    float m_damageIntarvalTime = 0.0f;	//!<ダメージを受けてからの無敵時間。
	bool m_isAttack = false;		//!<攻撃しているかどうか。
};

