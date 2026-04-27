#pragma once
#include "Source/Actor/Character/Enemy/Enemy.h"

class Player;
class Gire;
class Game;
class AttackSpeedBuff;
class PowerBuff;
class Heal;

enum enSmallRobotState
{
	enSmallRobotState_Idle,		//待機状態。
	enSmallRobotState_Walk,		//移動状態。
	enSmallRobotState_Num,
};

class SmallRobot : public Enemy
{
public:
	SmallRobot();
	~SmallRobot();
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
	void Dide() override;
	void EnemyHP();
	Vector3 GetPosition()const override
	{
		return m_position;
	}
	int GetHP(int hp) override
	{
		hp = m_smallRobotHp;
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
	enum EnAnimationClip {		//アニメーション。
		enAnimationClip_Idle,
		enAnimationClip_Walk,
		enAnimationClip_Num,
	};
	AnimationClip m_animationClips[enAnimationClip_Num];
	ModelRender m_modelRender;
	CharacterController m_characterController;
	Vector3 m_position;
	Vector3 m_moveSpeed;
	Quaternion m_rotation;
	Player* m_player = nullptr;
	AttackSpeedBuff* m_attackSpeedBuff = nullptr;
	PowerBuff* m_powerBuff = nullptr;
	Heal* m_heal = nullptr;
	CollisionObject* m_collisionObject = nullptr;
	Game* m_game = nullptr;
	Vector3 m_forward;
	Vector3 m_scale;
	SpriteRender m_enemyHP;//敵のHPを表示する。
	Vector2 m_enemyHPBarPosition = Vector2::Zero;
	int m_smallRobotHp = 50;		//!<小型ロボットのHP。
	int m_smallRobotMaxHp = 50;	//!<小型ロボットの最大HP。
	int m_attackPower = 10;		//!<攻撃力。
	float m_timeCount = 0.0f;		//!<タイマー用の変数。
	float m_damageIntarvalTime = 0.0f;	//!<ダメージを受けてからの無敵時間。
	bool m_isShowHP = false;
};

