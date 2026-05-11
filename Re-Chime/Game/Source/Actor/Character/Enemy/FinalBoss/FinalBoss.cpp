#include "stdafx.h"
#include "FinalBoss.h"
#include "Source/Actor/Character/Player/Player.h"
#include "collision/CollisionObject.h"
#include "Source/Actor/Item/Gire/Gire.h"
#include "Game.h"
#include "DamageText.h"

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
	m_characterController.Init(300.0f, 100.0f, m_position);

	m_bossHPFrame.Init("Assets/UIData/enemyHPFrame.DDs", 1024, 128.0f);
	m_bossHPFrame.SetPosition(Vector3(10.5f, 450.0f, 0.0f));
	m_bossHPFrame.SetScale(Vector3(1.52f, 0.3f, 1.0f));
	m_bossHPFrame.Update();

	m_bossHPBar.Init("Assets/UIData/enemyHPBar.DDs", 1024, 128.0f);
	m_bossHPBar.SetPosition(Vector3(-760.0f, 447.0f, 0.0f));
	m_bossHPBar.SetScale(Vector3(1.48f, 0.3f, 1.0f));
	m_bossHPBar.SetPivot(Vector2(0.0f, 0.5f));
	m_bossHPBar.Update();

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

	FinalBossHP();

	float dist = (m_player->GetPosition() - m_position).Length();

	if (dist <= 2000.0f)
	{
		m_isShowBossHP = true;
	}
	else
	{
		m_isShowBossHP = false;
	}

	m_modelRender.Update();
}

void FinalBoss::Move()
{
	if (m_finalBossState != enFinalBossState_Chase)
	{
		return;
	}

	m_position = m_characterController.Execute(m_moveSpeed, g_gameTime->GetFrameDeltaTime());
	if (m_characterController.IsOnGround())
	{
		m_moveSpeed.y = 0.0f;
	}
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
	if (m_isAttack == true)
	{
		OnCollision();
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
			damage = m_player->GetAttackPower();
			m_finalBossHp -= damage;
			m_damageIntarvalTime = 1.0f;

//========================
// ダメージ表示生成
//========================
			DamageText* damageText = NewGO<DamageText>(0);

			Vector3 textPos = m_position;

			textPos.y += 250.0f;

			damageText->SetPosition(textPos);

			damageText->SetDamage(damage);
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
			finalBossAttackPower = GetAttackPower();
			m_player->TakeDamage(finalBossAttackPower, m_position);
		}
	}
}

const bool FinalBoss::SearchPlayer() const
{
	Vector3 diff = m_player->GetPosition() - m_position;

	if (diff.LengthSq() <= 500.0f * 500.0f)
	{
		diff.Normalize();
		float cos = m_forward.Dot(diff);
		float angle = acosf(cos);
		if (angle <= (Math::PI / 180.0f) * 120.0f)
		{
			return true;
		}
	}
	return false;
}

void FinalBoss::DamageIntarval()
{
	m_damageIntarvalTime -= g_gameTime->GetFrameDeltaTime();
	if (m_damageIntarvalTime < 0.0f)
	{
		m_damageIntarvalTime = 0.0f;
	}
}

void FinalBoss::Death()
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
	case enFinalBossState_Chase:
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
	case enFinalBossState_Chase:
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
	m_idleTimer = 0.0f;
	m_chaseTimer = 0.0f;

	Vector3 diff = m_player->GetPosition() - m_position;

	if (SearchPlayer())
	{
		diff.Normalize();
		m_moveSpeed = diff * 1000.0f;
		if (IsCanAttack())
		{
			int ramdom = rand() % 100;
			if (ramdom > 60)
			{
				m_finalBossState = enFinalBossState_Attack;
				m_isAttack = true;
				return;
			}
			else
			{
				m_finalBossState = enFinalBossState_Chase;
				return;
			}
		}
	}
	else
	{
		m_finalBossState = enFinalBossState_Idle;
		return;
	}
	if (m_finalBossHp <= 0)
	{
		m_finalBossState = enFinalBossState_Death;
		return;
	}
}

void FinalBoss::IdleState()
{
	m_idleTimer += g_gameTime->GetFrameDeltaTime();
	if(m_idleTimer >= 0.9f)
	{
		FinalBossState();
	}
}

void FinalBoss::WalkState()
{
	if (IsCanAttack())
	{
		FinalBossState();
		return;
	}
	m_chaseTimer += g_gameTime->GetFrameDeltaTime();
	if(m_chaseTimer >= 0.8f)
	{
		FinalBossState();
	}
}

void FinalBoss::AttackState()
{
	if(m_modelRender.IsPlayingAnimation() == false)
	{
		m_isAttack = false;
		FinalBossState();
	}
}

void FinalBoss::DeathState()
{
	if(m_modelRender.IsPlayingAnimation() == false)
	{
		Death();
	}
}

Vector3 FinalBoss::GetPosition()const
{
	return m_position;
}

const bool FinalBoss::IsCanAttack() const
{
	Vector3 diff = m_player->GetPosition() - m_position;
	if (diff.LengthSq() <= 300.0f * 300.0f)
	{
		return true;
	}
	return false;
}

void FinalBoss::FinalBossHP()
{
	float rate = (float)m_finalBossHp / (float)m_finalBossMaxHp;

	Vector3 scale = { 1.5f, 0.3f, 1.0f };
	scale.x *= rate;

	m_bossHPBar.SetScale(scale);

	// HP少なくなったら色変更
	if (m_finalBossHp <= m_finalBossMaxHp / 4)
	{
		m_bossHPBar.SetMulColor(g_vec4Red);
	}
	else
	{
		m_bossHPBar.SetMulColor(g_vec4Green);
	}
	m_bossHPBar.Update();
}

void FinalBoss::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);

	if (m_isShowBossHP)
	{
		m_bossHPFrame.Draw(rc);
		m_bossHPBar.Draw(rc);
	}
}