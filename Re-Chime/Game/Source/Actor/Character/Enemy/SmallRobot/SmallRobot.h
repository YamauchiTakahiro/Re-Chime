#pragma once
#include "Source/Actor/Character/Enemy/Enemy.h"

class Player;
class Gire;
class Game;
class AttackSpeedBuff;
class PowerBuff;
class Heal;
class DamageText;
class AudioManager;
class DifficultyLevel;

class SmallRobot : public Enemy
{
public:
	enum enSmallRobotState
{
	enSmallRobotState_Idle,		//待機状態。
	enSmallRobotState_Walk,		//移動状態。
	enSmallRobotState_Attack,	//攻撃状態。
	enSmallRobotState_Death,		//死亡状態。
	enSmallRobotState_Num,
};
	SmallRobot();
	~SmallRobot();
	bool Start() override;
	void Update() override;
	void UpdateTimer();
	void KnockBack();
	void Move() override;
	void Rotation() override;
	void Attack() override;
	void SearchPlayer();
	void OnCollision() override;
	void Time() override;
	void Hit() override;
	void TakeDamage(int damage, float knockBackTime);
	void DamageIntarval() override;
	void AttackHit() override;
	void Death() override;
	void EnemyHP();
	void MakeExplosionEffect();
	void ManageState();
	//void PlayAnimation();
	void EnemyState();
	void WalkState();
	void IdleState();
	void AttackState();
	void DeathState();
	Vector3 GetPosition()const override
	{
		return m_position;
	}
	int GetHP() const override
	{
		return m_smallRobotHp;
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
	//enum EnAnimationClip {		//アニメーション。
	//	enAnimationClip_Idle,
	//	enAnimationClip_Walk,
	//	enAnimationClip_Num,
	//};
	//AnimationClip m_animationClips[enAnimationClip_Num];
	ModelRender m_modelRender;
	CharacterController m_characterController;
	Vector3 m_position;
	Vector3 m_moveSpeed;
	Quaternion m_rotation;
	Player* m_player = nullptr;
	AttackSpeedBuff* m_attackSpeedBuff = nullptr;
	PowerBuff* m_powerBuff = nullptr;
	Heal* m_heal = nullptr;
	DifficultyLevel* m_difficultyLevel = nullptr;
	CollisionObject* m_collisionObject = nullptr;
	Game* m_game = nullptr;
	DamageText* m_damageText = nullptr;
	AudioManager* m_audioManager = nullptr;
	Vector3 m_forward;
	Vector3 m_scale;
	SpriteRender m_enemyHP;//敵のHPを表示する。
	SpriteRender m_enemyHPFrame;
	Vector2 m_enemyHPBarPosition = Vector2::Zero;
	Vector2 m_enemyHPFramePosition = Vector2::Zero;
	enSmallRobotState m_smallRobotState = enSmallRobotState_Idle;	//!<小型ロボットの状態。
	int m_smallRobotHp = 50;		//!<小型ロボットのHP。
	int m_smallRobotMaxHp = 50;	//!<小型ロボットの最大HP。
	int m_attackPower = 10;		//!<攻撃力。
	float m_timeCount = 0.0f;		//!<タイマー用の変数。
	float m_damageIntarvalTime = 0.0f;	//!<ダメージを受けてからの無敵時間。
	float m_attackCollisionLife = 0.0f;	//!<攻撃判定の有効時間。
	float m_knockBackPower = 0.0f; //ノックバックの強さ。
	float m_knockBackTime = 0.0f;
	float m_rotationSpeed = 15.0f;		//!<回転速度。
	Vector3 m_knockBackMove = Vector3::Zero;
	bool m_isShowHP = false;
	bool m_isAttack = false;
	bool m_isDeath = false;
	bool m_searchPlayer = false;
	bool m_isKnockBack = false;
	bool m_hasDetectedPlayer = false;
	float m_moveSpeedValue = 100.0f;
};

