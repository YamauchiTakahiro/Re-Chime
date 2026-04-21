#include "stdafx.h"
#include "MediumRobot.h"
#include "Source/Actor/Character/Player/Player.h"
#include "Source/Actor/Item/Potion/Buff/AttackSpeedBuff/AttackSpeedBuff.h"
#include "Source/Actor/Item/Potion/Buff/PowerBuff/PowerBuff.h"
#include "collision/CollisionObject.h"
#include "Game.h"

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
	m_game = FindGO<Game>("game");
	return true;
}

void MediumRobot::Update()
{
	bool isPause = false;
	isPause = m_game->GetIsPause(isPause);
	if (isPause)
	{
		return;
	}
	Move();

	Rotation();

	Time();

	Hit();

	DamageIntarval();

	AttackHit();

	Dide();
	m_modelRender.Update();
}

void MediumRobot::Move()
{
	Vector3 playerPos = m_player->GetPosition();
	Vector3 toPlayer = playerPos - m_position;
	float distToPlayer = toPlayer.Length();
	if (distToPlayer <= 500 && m_timeCount == 0.0f)
	{
		Attack();
		m_timeCount = 2.0f;
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
		m_moveSpeed = toPlayer * 0.0f;
	}

	if (m_characterController.IsOnGround() == false)
	{
		//m_moveSpeed.y -= 40.0f;
	}

	m_position = m_characterController.Execute(m_moveSpeed, 2.0f / 60.0f);

	m_modelRender.SetPosition(m_position);
}

void MediumRobot::Rotation()
{
	Vector3 playerPos = m_player->GetPosition();
	Vector3 toPlayer = playerPos - m_position;
	float distToPlayer = toPlayer.Length();
	if (distToPlayer <= 1000)
	{
		toPlayer.Normalize();
		m_rotation.SetRotationYFromDirectionXZ(toPlayer);
	}
	m_modelRender.SetRotation(m_rotation);
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

void MediumRobot::AttackHit()
{
	const auto& collisions = g_collisionObjectManager->FindCollisionObjects("mediumRobotAttack");
	for (auto collision : collisions)
	{
		if (collision->IsHit(m_player->GetCharacterController()) == true)
		{
			int mediumRobotAttackPower = 0;
			mediumRobotAttackPower = GetAttackPower(mediumRobotAttackPower);
			m_player->TakeDamage(mediumRobotAttackPower, m_position);
		}
	}
}

void MediumRobot::DamageIntarval()
{
	m_damageIntarvalTime -= g_gameTime->GetFrameDeltaTime();
	if (m_damageIntarvalTime < 0.0f)
	{
		m_damageIntarvalTime = 0.0f;
	}
}

void MediumRobot::Dide()
{
	if (m_mediumRobotHp <= 0)
	{
		m_game->EnemyCount();
		int randomNum = rand() % 100 + 1;
		if (randomNum <= 20)
		{
			m_attackSpeedBuff = NewGO<AttackSpeedBuff>(0);
			m_attackSpeedBuff->SetPosition(m_position);
		}
		else if (randomNum > 20 && randomNum <= 40)
		{
			m_powerBuff = NewGO<PowerBuff>(0);
			m_powerBuff->SetPosition(m_position);
		}
		DeleteGO(this);
	}
}

Vector3 MediumRobot::GetPosition()const
{
	return m_position;
}

void MediumRobot::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);
}
