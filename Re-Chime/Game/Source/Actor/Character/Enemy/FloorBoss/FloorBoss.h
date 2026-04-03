#pragma once
#include "Source/Actor/Character/Enemy/Enemy.h"

class Player;

class FloorBoss : public Enemy
{
public:
	FloorBoss();
	virtual ~FloorBoss();
	virtual bool Start() override;
	virtual void Update() override;
	virtual void Move() override;
	virtual void Rotation() override;
	virtual void Attack() override;
	virtual void OnCollision() override;
	virtual void Time() override;
	virtual void Hit() override;
	virtual void DamageIntarval() override;
	virtual void Dide() override;
	virtual Vector3 GetPosition(Vector3) override;
	void SetPosition(const Vector3& pos)
	{
		m_position = pos;
	}
	virtual int GetHP(int) override;
	virtual void Render(RenderContext& rc)override;



private:
	ModelRender m_modelRender;
	enum EnAnimationClip {		//アニメーション。
		enAnimationClip_Idle,
		enAnimationClip_Walk,
		enAnimationClip_Death,
		enAnimationClip_Num,
	};
	AnimationClip m_animationClips[enAnimationClip_Num];
	CharacterController m_characterController;
	Vector3 m_position;
	Vector3 m_moveSpeed;
	Quaternion m_rotation;
	Player* m_player = nullptr;
	CollisionObject* m_collisionObject = nullptr;
	Vector3 m_forward;
	int m_floorBossHP = 100;
	float m_timeCount = 0.0f;		//!<タイマー用の変数。
	float m_damageIntarvalTime = 0.0f;	//!<ダメージを受けてからの無敵時間。
};

