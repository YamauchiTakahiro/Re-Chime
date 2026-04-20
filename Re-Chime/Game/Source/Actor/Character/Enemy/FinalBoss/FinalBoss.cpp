#include "stdafx.h"
#include "FinalBoss.h"
#include "Source/Actor/Character/Player/Player.h"
#include "collision/CollisionObject.h"
#include "Game.h"

FinalBoss::FinalBoss()
{
	
}

FinalBoss::~FinalBoss()
{
}

bool FinalBoss::Start()
{
	m_modelRender.Init("Assets/modelData/Enemy/finalBoss/finalBoss.tkm");
	m_characterController.Init(200.0f, 100.0f, m_position);
	m_player = FindGO<Player>("player");
	m_game = FindGO<Game>("game");
	return true;
}

void FinalBoss::Update()
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
	Dide();
	m_modelRender.Update();
}

void FinalBoss::Move()
{
	Vector3 playerPos = m_player->GetPosition();
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

void FinalBoss::Rotation()
{
	Vector3 playerPos = m_player->GetPosition();
	Vector3 toPlayer = playerPos - m_position;
	toPlayer.Normalize();
	Quaternion rot;
	rot.SetRotationYFromDirectionXZ(toPlayer);
	m_modelRender.SetRotation(rot);
}

void FinalBoss::Attack()
{
	OnCollision();
}

void FinalBoss::OnCollision()
{
	m_collisionObject = NewGO<CollisionObject>(0);

	Vector3 collisionPos = m_position;
	m_forward = Vector3::Front;
	m_rotation.Apply(m_forward);
	collisionPos += m_forward * 250.0f;
	m_collisionObject->CreateSphere(collisionPos, Quaternion::Identity, 200.0f);
	m_collisionObject->SetName("finalBossAttack");
}

void FinalBoss::Hit()
{
	const auto& collisions = g_collisionObjectManager->FindCollisionObjects("playerAttack");
	for (auto collision : collisions)
	{
		if (collision->IsHit(m_characterController) == true && m_damageIntarvalTime == 0.0f)
		{
			int damage = 0;
			damage = m_player->GetAttackPower(damage);
			m_finalBossHp -= damage;
			m_damageIntarvalTime = 1.0f;
		}
	}
}

void FinalBoss::AttackHit()
{
	const auto& collisions = g_collisionObjectManager->FindCollisionObjects("finalBossAttack");
	for (auto collision : collisions)
	{
		if (collision->IsHit(m_player->GetCharacterController()) == true)
		{
			int finalBossAttackPower = 0;
			finalBossAttackPower = GetAttackPower(finalBossAttackPower);
			m_player->TakeDamage(finalBossAttackPower, m_position);
		}
	}
}

void FinalBoss::Time()
{
	m_timeCount -= g_gameTime->GetFrameDeltaTime();
	if (m_timeCount < 0.0f)
	{
		m_timeCount = 0.0f;
	}
}

void FinalBoss::DamageIntarval()
{
	m_damageIntarvalTime -= g_gameTime->GetFrameDeltaTime();
	if (m_damageIntarvalTime < 0.0f)
	{
		m_damageIntarvalTime = 0.0f;
	}
}

void FinalBoss::Dide()
{
	if (m_finalBossHp <= 0)
	{
		m_game->EnemyCount();
		DeleteGO(this);
	}
}

Vector3 FinalBoss::GetPosition()const
{
	return m_position;
}

void FinalBoss::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);
}