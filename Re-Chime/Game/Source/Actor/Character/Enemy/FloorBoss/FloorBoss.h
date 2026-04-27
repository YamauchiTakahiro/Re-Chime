#pragma once
#include "Source/Actor/Character/Enemy/Enemy.h"

class Player;
class Game;
class AttackSpeedBuff;
class PowerBuff;

class FloorBoss : public Enemy
{
public:
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
	void Dide() override;
	Vector3 GetPosition()const override
	{
		return m_position;
	}
	int GetAttackPower(int attackPower)
	{
		attackPower = m_attackPower;
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
	int GetHP(int hp) override
	{
		hp = m_floorBossHP;
		return m_floorBossHP;
	}
	void Render(RenderContext& rc)override;



private:
	ModelRender m_modelRender;
	//enum EnAnimationClip {		//アニメーション。
	//	enAnimationClip_Idle,
	//	enAnimationClip_Walk,
	//	enAnimationClip_Death,
	//	enAnimationClip_Num,
	//};
	//AnimationClip m_animationClips[enAnimationClip_Num];
	CharacterController m_characterController;
	Vector3 m_position;
	Vector3 m_moveSpeed;
	Quaternion m_rotation;
	Vector3 m_scale;
	Player* m_player = nullptr;
	Game* m_game = nullptr;
	AttackSpeedBuff* m_attackSpeedBuff = nullptr;
	PowerBuff* m_powerBuff = nullptr;
	CollisionObject* m_collisionObject = nullptr;
	Vector3 m_forward;
	int m_floorBossHP = 100;
	int m_floorBossMaxHP = 100;
	int m_attackPower = 20;
	float m_timeCount = 0.0f;		//!<タイマー用の変数。
	float m_damageIntarvalTime = 0.0f;	//!<ダメージを受けてからの無敵時間。
};

