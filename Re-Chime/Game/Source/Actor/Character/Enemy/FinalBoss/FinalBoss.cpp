#include "stdafx.h"
#include "FinalBoss.h"
#include "Source/Actor/Character/Player/Player.h"
#include "collision/CollisionObject.h"
#include "Source/Actor/Item/Gire/Gire.h"
#include "Game.h"

FinalBoss::FinalBoss()
{
	
}

FinalBoss::~FinalBoss()
{
}

bool FinalBoss::Start()
{
	m_animationClips[enAnimationClip_Idle].Load("Assets/animData/Enemy/finalBoss/finalBossIdle.tka");
	m_animationClips[enAnimationClip_Idle].SetLoopFlag(true);
	m_animationClips[enAnimationClip_Walk].Load("Assets/animData/Enemy/finalBoss/finalBossWalk.tka");
	m_animationClips[enAnimationClip_Walk].SetLoopFlag(true);
	m_animationClips[enAnimationClip_Attack].Load("Assets/animData/Enemy/finalBoss/finalBossAttack.tka");
	m_animationClips[enAnimationClip_Attack].SetLoopFlag(false);
	m_animationClips[enAnimationClip_Death].Load("Assets/animData/Enemy/finalBoss/finalBossDeath.tka");
	m_animationClips[enAnimationClip_Death].SetLoopFlag(false);
	m_modelRender.Init("Assets/modelData/Enemy/finalBoss/finalBoss.tkm", m_animationClips, enAnimationClip_Num);
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

	AttackHit();

	ManageState();

	PlayAnimation();
	m_modelRender.Update();
}

void FinalBoss::Move()
{
	Vector3 playerPos = m_player->GetPosition();
	Vector3 toPlayer = playerPos - m_position;
	float distToPlayer = toPlayer.Length();
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
	if(m_finalBossState != enFinalBossState_Attack)
	{
		return;
	}
	if (m_isAttack == true && m_timeCount == 0.0f)
	{
		OnCollision();
		m_isAttack = false;
		m_timeCount = 2.0f;
	}
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
		MakeExplosionEffect();
		m_gire = NewGO<Gire>(0);
		m_gire->SetPosition(Vector3(m_position.x, m_position.y + 50.0f, m_position.z));
		DeleteGO(this);
	}
}

void FinalBoss::MakeExplosionEffect()
{
	// 爆発エフェクトの生成処理をここに実装
	EffectEmitter* effectEmitter = NewGO<EffectEmitter>(0);
	effectEmitter->Init(5);
	effectEmitter->SetScale(Vector3::One * 10.0f);
	Vector3 effectPos = m_position;
	effectEmitter->SetPosition(effectPos);
	effectEmitter->Play();
}

void FinalBoss::ManageState()
{
	switch (m_finalBossState)
	{
	case enFinalBossState_Idle:
		IdleState();
		break;
	case enFinalBossState_Walk:
		WalkState();
		break;
	case enFinalBossState_Attack:
		AttackState();
		break;
	case enFinalBossState_Death:
		DeathState();
		break;
	default:
		break;
	}
}

void FinalBoss::PlayAnimation()
{
	switch (m_finalBossState)
	{
	case enFinalBossState_Idle:
		m_modelRender.PlayAnimation(enAnimationClip_Idle);
		break;
	case enFinalBossState_Walk:
		m_modelRender.PlayAnimation(enAnimationClip_Walk);
		break;
	case enFinalBossState_Attack:
		m_modelRender.PlayAnimation(enAnimationClip_Attack);
		break;
	case enFinalBossState_Death:
		m_modelRender.PlayAnimation(enAnimationClip_Death);
		break;
	default:
		break;
	}
}

void FinalBoss::FinalBossState()
{
	Vector3 playerPos = m_player->GetPosition();
	Vector3 toPlayer = playerPos - m_position;
	float distToPlayer = toPlayer.Length();
	if (m_finalBossHp <= 0)
	{
		m_finalBossState = enFinalBossState_Death;
	}
	if (distToPlayer <= 500 && m_timeCount == 0.0f)
	{
		m_finalBossState = enFinalBossState_Attack;
		m_isAttack = true;
	}
	else if (m_moveSpeed.Length() > 0.01f)
	{
		m_finalBossState = enFinalBossState_Walk;
	}
	else
	{
		m_finalBossState = enFinalBossState_Idle;
	}
}

void FinalBoss::IdleState()
{
	FinalBossState();
}

void FinalBoss::WalkState()
{
	FinalBossState();
}

void FinalBoss::AttackState()
{
	if(m_modelRender.IsPlayingAnimation() == false)
	{
		FinalBossState();
	}
}

void FinalBoss::DeathState()
{
	if(m_modelRender.IsPlayingAnimation() == false)
	{
		Dide();
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