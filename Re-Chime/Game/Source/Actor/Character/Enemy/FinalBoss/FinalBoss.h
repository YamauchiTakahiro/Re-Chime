#pragma once
#include "Source/Actor/Character/Enemy/Enemy.h"

class Player;
class Gire;
class Game;
class DamageText;
class Bullet;
class AudioManager;
class DifficultyLevel;

class FinalBoss : public Enemy
{
    public:
        enum EnFinalBossState {
            enFinalBossState_Idle,		//待機状態。
            enFinalBossState_Chase,		//追跡。
            enFinalBossState_Shot,		//攻撃。
            enFinalBossState_Death,		//死亡。
			enFinalBossState_Num,
		};
    FinalBoss();
    ~FinalBoss();
    bool Start() override;
    void Update() override;
    void Move() override;
    void Rotation() override;
    void Shot();
	void CreateBullet(float angleOffset);
	void PhaseChange();
    //void OnCollision() override;
    void Hit() override;
    void DamageIntarval() override;
    void Death() override;
    void MakeExplosionEffect();
    //void AttackHit() override;
	const bool SearchPlayer()const;
	void ManageState();
	void PlayAnimation();
    void FinalBossState();
	void IdleState();
	void WalkState();
	void ShotState();
	void DeathState();
	const bool IsCanAttack()const;
    void FinalBossHP();
    int GetHP() const override
    {
        return m_finalBossHp;
    }
    int GetAttackPower() const
    {
        return m_attackPower;
	}
    Vector3 GetPosition()const override
    {
        return m_position;
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
        enAnimationClip_Shot,
        enAnimationClip_Death,
        enAnimationClip_Num,
	};
	AnimationClip m_animationClips[enAnimationClip_Num];
	EnAnimationClip m_currentAnimationClip = enAnimationClip_Num;
	EnFinalBossState m_finalBossState = enFinalBossState_Idle;
    ModelRender m_modelRender;
    CharacterController m_characterController;
    Vector3 m_position = Vector3::Zero;
    Vector3 m_moveSpeed = Vector3::Zero;
    Quaternion m_rotation = Quaternion::Identity;
    Player* m_player = nullptr;
	Gire* m_gire = nullptr;
	Game* m_game = nullptr;
	Bullet* m_bullet = nullptr;
	AudioManager* m_audioManager = nullptr;
    DifficultyLevel* m_difficultyLevel = nullptr;
    DamageText* m_damageText = nullptr;
    CollisionObject* m_collisionObject = nullptr;
    Vector3 m_forward = Vector3::Front;
    Vector3 m_scale = Vector3::One;
    SpriteRender m_bossHPFrame;
    SpriteRender m_bossHPBar;
    SpriteRender m_alertMark;
    SpriteRender m_questionMark;
    Vector3 m_lastPlayerPos;
    Vector3 m_startPosition;
    Vector3 m_moveTarget;
    bool m_isReposition = false;
    bool m_isSearching = false;
    float m_searchTimer = 0.0f;
    bool m_isShowAlert = false;
    float m_alertTime = 0.0f;
    float m_alertScale = 0.3f;
    bool m_isShowQuestion = false;
    float m_questionTime = 0.0f;
    float m_questionScale = 0.0f;
    bool m_hasDetectedPlayer = false;
    bool m_isShowHP = false;
    int m_finalBossHp = 0;		//!<最終ボスのHP。
	int m_finalBossMaxHp = 0;	//!<最終ボスの最大HP。
	int m_attackPower = 0;		//!<攻撃力。
	float m_damageIntarvalTime = 0.0f;	//!<ダメージを受けてからの無敵時間。
    float m_chaseTimer = 0.0f;						//追跡タイマー。
    float m_idleTimer = 0.0f;						//待機タイマー。
	float m_attackCollisionLife = 0.0f;				//攻撃判定の有効時間。
    float m_shotCoolTime = 0.0f;
	float m_shotCoolTimeReset = 0.0f;						//攻撃のクールタイム。
	float m_searchRange = 0.0f;						//プレイヤーを発見する範囲。
	float m_bulletSpeed = 0.0f;						//弾の速さ。
	bool m_isShot = false;		//!<攻撃しているかどうか。
	bool m_discoveryPlayer = false;	//!<プレイヤーを発見しているかどうか。
    bool m_isShowBossHP = false;
    bool m_isDead = false;
	bool m_firstPhaseChange = false;		//最初のフェーズチェンジをしたかどうか。
	bool m_secondPhaseChange = false;		//2回目のフェーズチェンジをしたかどうか。
    float m_moveSpeedValue = 0.0f;
    float m_searchMotionTimer = 0.0f;
    float m_baseRotation = 0.0f;
    float m_rotationY = 0.0f;
    float m_lostWaitTime = 0.0f;
    bool m_isLostWaiting = false;
};

