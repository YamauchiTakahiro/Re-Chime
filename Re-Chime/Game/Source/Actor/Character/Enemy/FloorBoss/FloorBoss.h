#pragma once
#include "Source/Actor/Character/Enemy/Enemy.h"

class Player;
class Game;
class AttackSpeedBuff;
class PowerBuff;
class Heal;
class AudioManager;
class DifficultyLevel;

class FloorBoss : public Enemy
{
public:
	enum enFloorBossState
	{
		enFloorBossState_Idle,		//待機状態。
		enFloorBossState_Walk,		//移動状態。
		enFloorBossState_Attack,		//攻撃状態。
		enFloorBossState_Death,		//死亡状態。
		enFloorBossState_Num,
	};

	enum EnAttackPhase
	{
		enAttackPhase_Warn,   // 構え
		enAttackPhase_Active, // 当たり判定
		enAttackPhase_End     // 終了
	};

	FloorBoss();
	~FloorBoss();
	bool Start() override;
	void Update() override;
	void Move() override;
	void Rotation() override;
	void Attack() override;
	void OnCollision() override;
	void Time() override;
	void Hit() override;
	void AttackHit() override;
	void DamageIntarval() override;
	void Death() override;
	void MakeExplosionEffect();
	void MakeNoticeCircleEffect();
	void ManageState();
	void FloorBossHP();
	void PlayAnimation();
	void FloorBossState();
	void IdleState();
	void WalkState();
	void AttackState();
	void DeathState();
	Vector3 GetPosition()const override
	{
		return m_position;
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
	int GetHP() const override
	{
		return m_floorBossHP;
	}
	void Render(RenderContext& rc)override;



private:
	ModelRender m_modelRender;
	enum EnAnimationClip {		//アニメーション。
		enAnimationClip_Idle,
		enAnimationClip_Walk,
		enAnimationClip_Death,
		enAnimationClip_Num,
	};
	AnimationClip m_animationClips[enAnimationClip_Num];
	enFloorBossState m_floorBossState = enFloorBossState_Idle;
	EnAttackPhase m_attackPhase;
	CharacterController m_characterController;
	Vector3 m_position;
	Vector3 m_moveSpeed;
	Quaternion m_rotation;
	Vector3 m_scale;
	Player* m_player = nullptr;
	Game* m_game = nullptr;
	PowerBuff* m_powerBuff = nullptr;
	AttackSpeedBuff* m_attackSpeedBuff = nullptr;
	Heal* m_heal = nullptr;
	DifficultyLevel* m_difficultyLevel = nullptr;
	AudioManager* m_audioManager = nullptr;
	CollisionObject* m_collisionObject = nullptr;
	Vector3 m_forward;
	SpriteRender m_bossHPFrame;
	SpriteRender m_bossHPBar;
	Vector2 m_enemyHPBarPosition = Vector2::Zero;
	SpriteRender m_alertMark;
	SpriteRender m_questionMark;

	bool m_hasDetectedPlayer = false;

	bool m_isShowAlert = false;
	float m_alertTime = 0.0f;
	float m_alertScale = 0.0f;

	bool m_isShowQuestion = false;
	float m_questionTime = 0.0f;
	float m_questionScale = 0.0f;
	bool m_isShowHP = false;
	bool m_isAttack = false;
	int m_floorBossHP = 80;
	int m_floorBossMaxHP = 80;
	int m_attackPower = 5;
	float m_timeCount = 0.0f;		//!<タイマー用の変数。
	float m_damageIntarvalTime = 0.0f;	//!<ダメージを受けてからの無敵時間。
	float m_attackCollisionLife = 0.0f;	//!<攻撃判定の有効時間。
	float m_attackStateTimer = 0.0f;	//!<攻撃状態のタイマー。
	float m_attackDelayTimer = 0.0f; 
	float m_attackIntervalTime = 0.0f;
	bool m_attackHitActive = false;
	float m_attackWarnTimer;
	float m_attackActiveTimer;
	float m_moveSpeedValue = 250.0f;
};