#include "stdafx.h"
#include "MediumRobot.h"
#include "Source/Actor/Character/Player/Player.h"

MediumRobot::MediumRobot()
{
	
}

MediumRobot::~MediumRobot()
{
}

bool MediumRobot::Start()
{
	m_modelRender.Init("Assets/modelData/Enemy/mediumRobot/mediumRobot.tkm");
	m_characterController.Init(200.0f, 100.0f, m_position);
	m_player = FindGO<Player>("player");
	return true;
}

void MediumRobot::Update()
{
	Move();
	Rotation();
	Time();
	Hit();
	DamageIntarval();
	Dide();
	m_modelRender.Update();
}

void MediumRobot::Move()
{
	Vector3 playerPos;
	playerPos = m_player->GetPosition(playerPos);
	Vector3 toPlayer = playerPos - m_position;
	float distToPlayer = toPlayer.Length();
	if (distToPlayer <= 500 && m_timeCount == 0.0f)
	{
		Attack();
		m_timeCount = 1.0f;
		Time();
	}
	if (distToPlayer <= 1000)
	{
		toPlayer.Normalize();
		m_moveSpeed = toPlayer * 100.0f;
		m_moveSpeed.y = 0.0f;
	}
	else if (distToPlayer > 1000)
	{
		m_moveSpeed.Set(0.0f, 0.0f, 0.0f);
	}
	m_position = m_characterController.Execute(m_moveSpeed, 2.0f / 60.0f);
	m_modelRender.SetPosition(m_position);
}

void MediumRobot::Rotation()
{
	Vector3 playerPos;
	playerPos = m_player->GetPosition(playerPos);
	Vector3 toPlayer = playerPos - m_position;
	toPlayer.y = 0.0f;
	toPlayer.Normalize();
	Quaternion rot;
	rot.SetRotationYFromDirectionXZ(toPlayer);
	m_modelRender.SetRotation(rot);
}

void MediumRobot::Attack()
{
	OnCollision();
}

void MediumRobot::OnCollision()
{
	m_collisionObject = NewGO<CollisionObject>(0);

	Vector3 collisionPos = m_position;
	m_forward = Vector3::Front;
	m_rotation.Apply(m_forward);
	collisionPos += m_forward * 250.0f;
	m_collisionObject->CreateSphere(collisionPos, Quaternion::Identity, 200.0f);
	m_collisionObject->SetName("mediumRobotAttack");
}

void MediumRobot::Time()
{
	if (m_timeCount > 0.0f)
	{
		m_timeCount += 2.0f / 60.0f;
		if (m_timeCount >= 3.0f)
		{
			m_timeCount = 0.0f;
		}
	}
}

void MediumRobot::Hit()
{
	const auto& collisions = g_collisionObjectManager->FindCollisionObjects("playerAttack");
	for (auto collision : collisions)
	{
		if (collision->IsHit(m_characterController) == true && m_damageIntarvalTime == 0.0f)
		{
			int damage = 0;
			damage = m_player->GetAttackPower(damage);
			m_mediumRobotHp -= damage;
			m_damageIntarvalTime = 1.0f;
		}
	}
}

void MediumRobot::DamageIntarval()
{
	if (m_timeCount > 0.0f)
	{
		m_timeCount += 2.0f / 60.0f;
		if (m_timeCount >= 3.0f)
		{
			m_timeCount = 0.0f;
		}
	}
}

void MediumRobot::Dide()
{
	if (m_mediumRobotHp <= 0)
	{
		DeleteGO(this);
	}
}

Vector3 MediumRobot::GetPosition(Vector3 position)
{
	return m_position;
}

void MediumRobot::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);
}
