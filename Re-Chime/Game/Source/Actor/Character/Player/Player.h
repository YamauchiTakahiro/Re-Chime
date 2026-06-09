#pragma once
#include "Source/Actor/Character/Character.h"

class Gire;
class Game;
class AudioManager;
class AttackSpeedBuff;
class PowerBuff;
class Heal;
class UI;
class EffectManager;

class Player : public Character
{
private:
    enum EnPlayerState {
		enPlayerState_Idle,         //待機状態。
		enPlayerState_Walk,         //歩き。
        enPlayerState_Jump,         //ジャンプ。
		enPlayerState_Run,          //走り。
        enPlayerState_Attack,       //攻撃。
		enPlayerState_Guard,        //ガード。
		enPlayerState_KnockBack,      //ノックバック。
	};
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
public:
    Player();
    ~Player();
    bool Start() override;
    void Update() override;
	void UpdateTimer();
    void Move() override;
	void JumpAndGravity();
    void Rotation() override;
	void Attack() override;
	void OnCollision() override;
	void Hit() override;
	void GetGires();
	void TakeDamage(int damage, const Vector3& enemyPos);
    void PlayerState();
	void PlayAnimation();
	void Heal();
	void MakeHealEffect();
	void MakePowerBuffEffect();
	void MakeAttackSpeedBuffEffect();
	void AttackState();
	void IdleState();
	void WalkState();
	void RunState();
	void JumpState();
	void GuardState();
	void KnockBackState();
	void ManageState();
	void FootStep();
	void ChangeAnimation(EnAnimationClip anim);
    Vector3 GetPosition()const override
    {
        return m_position;
    }
    void SetPosition(Vector3 position) override
    {
        m_position = position;
        m_modelRender.SetPosition(position);
		m_characterController.SetPosition(position);
    }
    const CharacterController& GetCharacterController() const;
    CharacterController& GetCharacterController();
    int GetHP() const override
    {
		return m_playerHp;
    }
    int GetMaxHP()const
    {
        return m_playerMaxHp;
    }
    int GetAttackPower()const
    {
        return m_attackPower;
	}
    bool GetGier() const
    {
        return m_isGetGire;
    }
    void addGireCount()
    {
        m_gireCount++;
        m_isGetGire = false;
	}
    int GetGireCount() const
    {
        return m_gireCount;
    }
    bool GetPowerBuffFlag()const
    {
        return m_powerBuffFlag;
    }
    bool GetAttackSpeedBuffFlag()const
    {
        return m_attackSpeedBuffFlag;
	}
    bool GetHealFlag() const
    {
        return m_isHealFlag;
    }
    void SetAttackHit(bool hit)
    {
        m_enemyHitFlag = hit;
    }
    bool GetAttackHit() const
    {
        return m_enemyHitFlag;
	}
    float GetPowerBuffTime() const
    {
        return m_powerBuffTime;
    }

    float GetAttackSpeedBuffTime() const
    {
        return m_attackSpeedBuffTime;
    }

    float GetCoolTime() const
    {
        return m_attackCoolTime;
    }

    float GetCoolTimeMax() const
    {
        return m_attackCoolTimeMax;
    }

    bool IsNearItem() const
    {
        return isNearItem;
    }

    int GetHealPotionCount() const
    {
        return m_healPotionCount;
    }

    int GetPowerPotionCount() const
    {
        return m_powerBuffPotionCount;
    }

    int GetAttackSpeedPotionCount() const
    {
        return m_attackSpeedPotionCount;
    }

    bool UseItem(int itemNo);
    void SetScale();
    void Render(RenderContext& rc)override;

private:
    //メンバ変数
	AnimationClip m_animationClips[enAnimationClip_Num];
    EnAnimationClip m_currentAnim = enAnimationClip_Num;
    ModelRender m_modelRender;
    CharacterController m_characterController;
	float m_speed = 0.0f;					//!<移動速度。
    Vector3 m_moveSpeed = Vector3::Zero;
    Quaternion m_rotation = Quaternion::Identity;
    Vector3 m_position = Vector3::Zero;
	Vector3 m_scale = Vector3::Zero;
    CollisionObject* m_collisionObject = nullptr;
    Vector3 m_forward = Vector3::Zero;
    Vector3 m_knockBack = Vector3::Zero;
	Gire* m_gire = nullptr;
    Game* m_game = nullptr;
	AudioManager* m_audioManager = nullptr;
    UI* m_ui = nullptr;
    EnPlayerState m_playerState = enPlayerState_Idle;
    int m_playerHp = 100;				//!<プレイヤーのHP。
	int m_playerMaxHp = 100;				//!<プレイヤーの最大HP。
	int m_attackPower = 0;				//!<攻撃力。
    int m_heal = 20;
    int m_healPotionCount = 1;
    int m_powerBuffPotionCount = 0;
    int m_attackSpeedPotionCount = 0;
	float m_attackCoolTime = 0.0f;				//!<攻撃のクールタイム。
    float m_attackCoolTimeMax = 0.0f;
	float m_damageIntarvalTime = 3.0f;		//!<ダメージを受けてからのクールタイム。
	float m_powerBuffTime = 0.0f;				//!<攻撃力バフの時間。
	float m_attackSpeedBuffTime = 0.0f;		//!<攻撃速度バフの時間。
	float m_guardTimeLimit = 3.0f;				//!<ガードできる時間の上限。
	float m_footStepTime = 0.0f;					//!<足音の時間。
	float m_jumpDelay = 0.0f;					//!<ジャンプの時間。
	float m_fadeTime = 0.0f;					//!<フェードの時間。
    float m_attackCollisionLife = 0.0f;
	float m_attackStartTime = 0.0f;
    float m_itemUseCoolTime = 0.0f;
	float m_knockBackPower = 0.0f;
    float m_guardCoolTime = 0.0f;
    int m_gireCount = 0;						//!<ギアの数。
	bool m_isGetGire = false;				//!<ギアを取ったかどうか。
    bool m_guardFlag = false;
    bool m_powerBuffFlag = false;
    bool m_attackSpeedBuffFlag = false;
	bool m_isAttack = false;
    bool m_isKnockBack = false;
	bool m_isPlayingWalkSE = false;
	bool m_isPlayingRunSE = false;
    bool m_isHealFlag = false;
	bool m_isJump = false;
	bool m_hasCreatedAttackCollision = false;
	bool m_enemyHitFlag = false;
	bool m_hasPlayedHitSE = false;
    bool isNearItem = false; //アイテムに近いかどうか
    bool m_canPickItem = false;
    bool m_isJumpStart = false;
    bool m_hasJumped = false;
};

