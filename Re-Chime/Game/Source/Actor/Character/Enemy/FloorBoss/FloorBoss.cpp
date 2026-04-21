#include "stdafx.h"
#include "FloorBoss.h"
#include "Source/Actor/Character/Player/Player.h"
#include "collision/CollisionObject.h"
#include "Game.h"
#include "Source/Actor/Item/Potion/Buff/AttackSpeedBuff/AttackSpeedBuff.h"	
#include "Source/Actor/Item/Potion/Buff/PowerBuff/PowerBuff.h"

FloorBoss::FloorBoss()
{
	
}

FloorBoss::~FloorBoss()
{
}

bool FloorBoss::Start()
{
	/*m_animationClips[enAnimationClip_Idle].Load("Assets/animData/Enemy/floorBoss/floorBoss_idle.tka");
	m_animationClips[enAnimationClip_Idle].SetLoopFlag(true);
	m_animationClips[enAnimationClip_Walk].Load("Assets/animData/Enemy/floorBoss/floorBoss_walk.tka");
	m_animationClips[enAnimationClip_Walk].SetLoopFlag(true);
	m_animationClips[enAnimationClip_Death].Load("Assets/animData/Enemy/floorBoss/floorBoss_death.tka");
	m_animationClips[enAnimationClip_Death].SetLoopFlag(false);*/
	m_modelRender.Init("Assets/modelData/Enemy/floorBoss/FloorBoss.tkm"/*, m_animationClips, enAnimationClip_Num, enModelUpAxisY*/);
	m_characterController.Init(200.0f, 100.0f, m_position);
	m_player = FindGO<Player>("player");
	m_game = FindGO<Game>("game");
	return true;
}

void FloorBoss::Update()
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

void FloorBoss::Move()
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
		m_moveSpeed = toPlayer * 0.0f;
	}

	if (m_characterController.IsOnGround() == false)
	{
		//m_moveSpeed.y -= 40.0f;
	}

	m_position = m_characterController.Execute(m_moveSpeed, 2.0f / 60.0f);

	m_modelRender.SetPosition(m_position);
}

void FloorBoss::Rotation()
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

void FloorBoss::Attack()
{
	OnCollision();
}

void FloorBoss::OnCollision()
{
	m_collisionObject = NewGO<CollisionObject>(0);

	Vector3 collisionPos = m_position;
	m_forward = Vector3::Front;
	m_rotation.Apply(m_forward);
	collisionPos += m_forward * 250.0f;
	m_collisionObject->CreateSphere(collisionPos, Quaternion::Identity, 200.0f);
	m_collisionObject->SetName("smallRobotAttack");
}

void FloorBoss::Time()
{
	m_timeCount -= g_gameTime->GetFrameDeltaTime();
	if (m_timeCount < 0.0f)
	{
		m_timeCount = 0.0f;
	}
}

void FloorBoss::Hit()
{
	const auto& collisions = g_collisionObjectManager->FindCollisionObjects("playerAttack");
	for (auto collision : collisions)
	{
		if (collision->IsHit(m_characterController) == true && m_damageIntarvalTime == 0.0f)
		{
			int damage = 0;
			damage = m_player->GetAttackPower(damage);
			m_floorBossHP -= damage;
			m_damageIntarvalTime = 1.0f;
		}
	}
}

void FloorBoss::AttackHit()
{
	const auto& collisions = g_collisionObjectManager->FindCollisionObjects("floorBossAttack");
	for (auto collision : collisions)
	{
		if (collision->IsHit(m_player->GetCharacterController()) == true)
		{
			int floorBossAttackPower = 0;
			floorBossAttackPower = GetAttackPower(floorBossAttackPower);
			m_player->TakeDamage(floorBossAttackPower, m_position);
		}
	}
}

void FloorBoss::DamageIntarval()
{
	m_damageIntarvalTime -= g_gameTime->GetFrameDeltaTime();
	if (m_damageIntarvalTime < 0.0f)
	{
		m_damageIntarvalTime = 0.0f;
	}
}

void FloorBoss::Dide()
{
	if (m_floorBossHP <= 0)
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

Vector3 FloorBoss::GetPosition()const
{
	return m_position;
}

void FloorBoss::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);
}