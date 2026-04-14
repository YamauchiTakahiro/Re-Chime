#pragma once
#include "Source/Actor/Character/Enemy/Enemy.h"

class Player;

class MediumRobot :
    public Enemy
{
    public:
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
    void Dide() override;
    Vector3 GetPosition(Vector3) override;
    int GetHP(int hp) override
    {
        hp = m_mediumRobotHp;
        return hp;
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
	ModelRender m_modelRender;
	CharacterController m_characterController;
	Vector3 m_position;
	Vector3 m_moveSpeed;
	Quaternion m_rotation;
	Player* m_player = nullptr;
	int m_mediumRobotHp = 100;
	CollisionObject* m_collisionObject = nullptr;
	Vector3 m_scale;
	Vector3 m_forward;
	float m_timeCount = 0.0f;
    float m_damageIntarvalTime = 0.0f;	//!<ダメージを受けてからの無敵時間。
};

