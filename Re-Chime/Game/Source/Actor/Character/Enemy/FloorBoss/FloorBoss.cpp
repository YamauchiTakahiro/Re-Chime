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
	/*m_animationClips[enAnimationClip_Idle].Load("Assets/animData/Enemy/floorBoss/floorBossIdle.tka");
	m_animationClips[enAnimationClip_Idle].SetLoopFlag(true);
	m_animationClips[enAnimationClip_Walk].Load("Assets/animData/Enemy/floorBoss/floorBossWalk.tka");
	m_animationClips[enAnimationClip_Walk].SetLoopFlag(true);
	m_animationClips[enAnimationClip_Death].Load("Assets/animData/Enemy/floorBoss/floorBossDeath.tka");
	m_animationClips[enAnimationClip_Death].SetLoopFlag(false);*/
	m_modelRender.Init("Assets/modelData/Enemy/floorBoss/FloorBoss.tkm"/*, m_animationClips, enAnimationClip_Num*/);
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

	AttackHit();

	/*ManageState();

	PlayAnimation();*/
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
	m_game->EnemyCount();
	MakeExplosionEffect();
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

void FloorBoss::MakeExplosionEffect()
{
	// 爆発エフェクトの生成処理をここに実装
	EffectEmitter* effectEmitter = NewGO<EffectEmitter>(0);
	effectEmitter->Init(5);
	effectEmitter->SetScale(Vector3::One * 10.0f);
	Vector3 effectPos = m_position;
	effectEmitter->SetPosition(effectPos);
	effectEmitter->Play();
}

//void FloorBoss::ManageState()
//{
//	switch (m_floorBossState)
//	{
//	case enFloorBossState_Idle:
//		IdleState();
//		break;
//	case enFloorBossState_Walk:
//		WalkState();
//		break;
//	case enFloorBossState_Death:
//		DeathState();
//		break;
//	default:
//		break;
//	}
//}
//
//void FloorBoss::PlayAnimation()
//{
//	switch (m_floorBossState)
//	{
//	case enFloorBossState_Idle:
//		m_modelRender.PlayAnimation(enAnimationClip_Idle);
//		break;
//	case enFloorBossState_Walk:
//		m_modelRender.PlayAnimation(enAnimationClip_Walk);
//		break;
//	case enFloorBossState_Death:
//		m_modelRender.PlayAnimation(enAnimationClip_Death);
//		break;
//	default:
//		break;
//	}
//}
//
//void FloorBoss::FloorBossState()
//{
//	if (m_floorBossHP <= 0)
//	{
//		m_floorBossState = enFloorBossState_Death;
//	}
//	else if (fabsf(m_moveSpeed.x) >= 0.001f || fabsf(m_moveSpeed.z) >= 0.001f)
//	{
//		m_floorBossState = enFloorBossState_Walk;
//	}
//	else
//	{
//		m_floorBossState = enFloorBossState_Idle;
//	}
//}
//
//void FloorBoss::IdleState()
//{
//	FloorBossState();
//}
//
//void FloorBoss::WalkState()
//{
//	FloorBossState();
//}
//
//void FloorBoss::DeathState()
//{
//	if(m_modelRender.IsPlayingAnimation() == false)
//	{
//		Dide();
//	}
//}

void FloorBoss::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);
}