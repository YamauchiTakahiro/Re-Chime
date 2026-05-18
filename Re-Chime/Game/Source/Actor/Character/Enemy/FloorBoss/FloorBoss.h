#pragma once
#include "Source/Actor/Character/Enemy/Enemy.h"

class Player;
class Game;
class AttackSpeedBuff;
class PowerBuff;
class Heal;
class AudioManager;

class FloorBoss : public Enemy
{
public:
	enum enFloorBossState
	{
		enFloorBossState_Idle,		//待機状態。
		enFloorBossState_Walk,		//移動状態。
		enFloorBossState_Death,		//死亡状態。
		enFloorBossState_Num,
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
	void ManageState();
	void FloorBossHP();
	void PlayAnimation();
	void FloorBossState();
	void IdleState();
	void WalkState();
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
	AudioManager* m_audioManager = nullptr;
	CollisionObject* m_collisionObject = nullptr;
	Vector3 m_forward;
	SpriteRender m_enemyHP;//敵のHPを表示する。
	Vector2 m_enemyHPBarPosition = Vector2::Zero;
	bool m_isShowHP = false;
	int m_floorBossHP = 100;
	int m_floorBossMaxHP = 100;
	int m_attackPower = 20;
	float m_timeCount = 0.0f;		//!<タイマー用の変数。
	float m_damageIntarvalTime = 0.0f;	//!<ダメージを受けてからの無敵時間。
};