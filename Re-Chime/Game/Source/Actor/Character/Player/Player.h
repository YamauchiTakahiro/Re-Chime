#pragma once
#include "Source/Actor/Character/Character.h"

class Gire;
class Game;
class SmallRobot;
class MediumRobot;
class FloorBoss;
class FinalBoss;

class Player : public Character
{
public:
    enum EnPlayerState {
		enPlayerState_Idle,         //待機状態。
		enPlayerState_Walk,         //歩き。
        enPlayerState_Jump,         //ジャンプ。
		enPlayerState_Run,          //走り。
        enPlayerState_Attack,       //攻撃。
		enPlayerState_Guard,        //ガード。
		enPlayerState_KnockBack,      //ノックバック。
	};
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
	void TakeDamage(int damage, const Vector3& enemyPos);
    void GuardCollision();
	void GuradInterval();
	void GuradTimeLimit();
    void PlayerState();
	void PlayAnimation();
	void PowerBuff();
	void PowerBuffTime();
	void AttackSpeedBuffTime();
	void Heal();
	void AttackState();
	void IdleState();
	void WalkState();
	void RunState();
	void JumpState();
	void GuardState();
	void KnockBackState();
	void ManageState();
    Vector3 GetPosition()const override
    {
        return m_position;
    }
    const CharacterController& GetCharacterController() const;
    CharacterController& GetCharacterController();
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
    int GetAttackPower(int attackPower)
    {
        attackPower = m_attackPower;
        return attackPower;
	}
    bool GetGier(bool getGier)
    {
        getGier = m_isGetGire;
		return getGier;
    }
    int GetGireCount()
    {
        m_gireCount++;
        return m_gireCount;
    }
    bool GetPowerBuffFlag(bool powerBuffFlag)
    {
        powerBuffFlag = m_powerBuffFlag;
		return powerBuffFlag;
    }
    bool GetAttackSpeedBuffFlag(bool attackSpeedBuffFlag)
    {
        attackSpeedBuffFlag = m_attackSpeedBuffFlag;
		return attackSpeedBuffFlag;
	}
    float GetPowerBuffTime()
    {
        return m_powerBuffTime;
    }

    float GetAttackSpeedBuffTime()
    {
        return m_attackSpeedBuffTime;
    }
    void SetScale();
    void Render(RenderContext& rc)override;


private:
    //メンバ変数
    enum EnAnimationClip {		//アニメーション。
        enAnimationClip_Idle,
        enAnimationClip_Walk,
        enAnimationClip_Jump,
        enAnimationClip_Run,
		enAnimationClip_Attack,
		enAnimationClip_Guard,
		enAnimationClip_KnockBack,
        enAnimationClip_Num,
    };
	AnimationClip m_animationClips[enAnimationClip_Num];
    ModelRender m_modelRender;
    CharacterController m_characterController;
	float m_speed = 0.0f;					//!<移動速度。
    Vector3 m_moveSpeed;
    Quaternion m_rotation;
    Vector3 m_position;
	Vector3 m_scale;
    CollisionObject* m_collisionObject;
    Vector3 m_forward;
    Vector3 m_knockBack;
	Gire* m_gire = nullptr;
    Game* m_game = nullptr;
	SmallRobot* m_smallRobot = nullptr;
	MediumRobot* m_mediumRobot = nullptr;
	FloorBoss* m_floorBoss = nullptr;
	FinalBoss* m_finalBoss = nullptr;
	SoundSource* m_se;
    int m_playerHp = 100;				//!<プレイヤーのHP。
	int m_playerMaxHp = 100;				//!<プレイヤーの最大HP。
    EnPlayerState m_playerState = enPlayerState_Idle;
	int m_attackPower = 0;				//!<攻撃力。
    int m_heal = 20;
	float m_timeCount = 0.0f;				//!<タイマー用の変数。
	float m_damageIntarvalTime = 3.0f;		//!<ダメージを受けてからのクールタイム。
	float m_guardIntervalTime = 0.0f;			//!<ガード後のクールタイム。
	float m_powerBuffTime = 0.0f;				//!<攻撃力バフの時間。
	float m_attackSpeedBuffTime = 0.0f;		//!<攻撃速度バフの時間。
	float m_guardTimeLimit = 3.0f;				//!<ガードできる時間の上限。
    int m_gireCount = 0;						//!<ギアの数。
	bool m_isGetGire = false;				//!<ギアを取ったかどうか。
    bool m_guardFlag = false;
    bool m_powerBuffFlag = false;
    bool m_attackSpeedBuffFlag = false;
	bool m_isAttack = false;
    bool m_isKnockBack = false;
};

