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
	m_animationClips[enAnimationClip_Idle].Load("Assets/animData/Enemy/mediumRobot/mediumRobotIdle.tka");
	m_animationClips[enAnimationClip_Idle].SetLoopFlag(true);
	m_animationClips[enAnimationClip_Walk].Load("Assets/animData/Enemy/mediumRobot/mediumRobotWalk.tka");
	m_animationClips[enAnimationClip_Walk].SetLoopFlag(true);
	m_animationClips[enAnimationClip_Death].Load("Assets/animData/Enemy/mediumRobot/mediumRobotDeath.tka");
	m_animationClips[enAnimationClip_Death].SetLoopFlag(false);
	m_modelRender.Init("Assets/modelData/Enemy/mediumRobot/mediumRobot.tkm", m_animationClips, enAnimationClip_Num);
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

	//Attack();

	DamageIntarval();

	AttackHit();

	ManageState();

	PlayAnimation();
	m_modelRender.Update();
}

void MediumRobot::Move()
{
	Vector3 playerPos = m_player->GetPosition();
	Vector3 toPlayer = playerPos - m_position;
	float distToPlayer = toPlayer.Length();
	if (distToPlayer <= 500 && m_timeCount == 0.0f)
	{
		m_timeCount = 2.0f;
		Attack();
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
	/*if (m_isAttack == false)
	{
		return;
	}
	else
	{
		OnCollision();
		m_isAttack = false;
	}*/
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
	m_timeCount -= g_gameTime->GetFrameDeltaTime();
	if (m_timeCount < 0.0f)
	{
		m_timeCount = 0.0f;
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
			damage = m_player->GetAttackPower();
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
			mediumRobotAttackPower = GetAttackPower();
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

void MediumRobot::Death()
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

void MediumRobot::MakeExplosionEffect()
{
	//爆発エフェクトを作る処理。
	EffectEmitter* effectEmitter = NewGO<EffectEmitter>(0);
	effectEmitter->Init(4);
	effectEmitter->SetScale(Vector3::One * 10.0f);
	Vector3 effectPos = m_position;
	effectEmitter->SetPosition(effectPos);
	effectEmitter->Play();
}

void MediumRobot::ManageState()
{
	switch (m_mediumRobotState)
	{
	case enMediumRobotState_Idle:
		IdleState();
		break;
	case enMediumRobotState_Walk:
		WalkState();
		break;
	case enMediumRobotState_Death:
		DeathState();
		break;
	default:
		break;
	}
}

void MediumRobot::PlayAnimation()
{
	switch (m_mediumRobotState)
	{
	case enMediumRobotState_Idle:
		m_modelRender.PlayAnimation(enAnimationClip_Idle);
		break;
	case enMediumRobotState_Walk:
		m_modelRender.PlayAnimation(enAnimationClip_Walk);
		break;
	case enMediumRobotState_Death:
		m_modelRender.PlayAnimation(enAnimationClip_Death);
		break;
	default:
		break;
	}
}

void MediumRobot::MediumRobotState()
{
	//if (m_timeCount == 0)
	//{
	//	//m_mediumRobotState = enMediumRobotState_Attack;
	//	//m_isAttack = true;
	//	Attack();
	//}
	if (m_mediumRobotHp <= 0)
	{
		m_mediumRobotState = enMediumRobotState_Death;
	}
	else if (fabsf(m_moveSpeed.x) >= 0.001f || fabsf(m_moveSpeed.z) >= 0.001f)
	{
		m_mediumRobotState = enMediumRobotState_Walk;
	}
	else
	{
		m_mediumRobotState = enMediumRobotState_Idle;
	}
}

void MediumRobot::IdleState()
{
	MediumRobotState();
}

void MediumRobot::WalkState()
{
	MediumRobotState();
}

//void MediumRobot::AttackState()
//{
//	MediumRobotState();
//}

void MediumRobot::DeathState()
{
	if (m_modelRender.IsPlayingAnimation() == false)
	{
		Death();
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
