#include "stdafx.h"
#include "FloorBoss.h"
#include "Source/Actor/Character/Player/Player.h"
#include "collision/CollisionObject.h"
#include "Game.h"
#include "Source/Actor/Item/Potion/Buff/AttackSpeedBuff/AttackSpeedBuff.h"	
#include "Source/Actor/Item/Potion/Buff/PowerBuff/PowerBuff.h"
#include "Source/Actor/Item/Potion/Heal/Heal.h"
#include "Source/Sound/AudioManager/AudioManager.h"
#include "Source/UIBase/DamageText/DamageText.h"
#include "Source/UIBase/DifficultyLevel/DifficultyLevel.h"
#include "Source/EffectManager/EffectManager.h"

FloorBoss::FloorBoss()
{
	
}

FloorBoss::~FloorBoss()
{
}

bool FloorBoss::Start()
{
	m_animationClips[enAnimationClip_Idle].Load("Assets/animData/Enemy/floorBoss/floorBossIdle.tka");
	m_animationClips[enAnimationClip_Idle].SetLoopFlag(true);
	m_animationClips[enAnimationClip_Walk].Load("Assets/animData/Enemy/floorBoss/floorBossWalk.tka");
	m_animationClips[enAnimationClip_Walk].SetLoopFlag(true);
	m_animationClips[enAnimationClip_Death].Load("Assets/animData/Enemy/floorBoss/floorBossDeath.tka");
	m_animationClips[enAnimationClip_Death].SetLoopFlag(false);
	m_modelRender.Init("Assets/modelData/Enemy/floorBoss/FloorBoss.tkm", m_animationClips, enAnimationClip_Num, enModelUpAxisY);
	m_characterController.Init(300.0f, 200.0f, m_position);

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
	m_audioManager = FindGO<AudioManager>("audioManager");
	m_difficultyLevel = FindGO<DifficultyLevel>("difficultyLevel");

	//========================
// 難易度設定
//========================
	if (m_game != nullptr)
	{
		switch (m_game->GetDifficulty())
		{
		case EASY:
			m_floorBossHP = 150;
			m_floorBossMaxHP = 150;
			m_attackPower = 10;
			m_attackIntervalTime = 3.0f;
			break;

		case NORMAL:
			m_floorBossHP = 250;
			m_floorBossMaxHP = 250;
			m_attackPower = 20;
			m_attackIntervalTime = 3.0f;
			break;

		case HARD:
			m_floorBossHP = 400;
			m_floorBossMaxHP = 400;
			m_attackPower = 35;
			m_attackIntervalTime = 2.5f;
			break;

		case LUNATIC:
			m_floorBossHP = 700;
			m_floorBossMaxHP = 700;
			m_attackPower = 50;
			m_attackIntervalTime = 2.0f;
			break;
		}
	}

	return true;
}

void FloorBoss::Update()
{
	bool isPause = false;
	isPause = m_game->GetIsPause(isPause);

	if (isPause || m_game->IsGameStop())
	{
		return;
	}

	Move();

	Rotation();

	Time();

	Hit();

	DamageIntarval();

	AttackHit();

	FloorBossHP();

	ManageState();

	PlayAnimation();

	m_attackCollisionLife -= g_gameTime->GetFrameDeltaTime();

	if (m_attackCollisionLife <= 0.0f)
	{
		DeleteGO(m_collisionObject);
		m_collisionObject = nullptr;
	}

	m_modelRender.Update();
}

void FloorBoss::Move()
{
	if (m_floorBossState == enFloorBossState_Attack)
	{
		m_moveSpeed = Vector3::Zero;

		m_position = m_characterController.Execute(
			m_moveSpeed,
			2.0f / 60.0f
		);

		m_modelRender.SetPosition(m_position);
		return;
	}

	Vector3 playerPos = m_player->GetPosition();
	Vector3 toPlayer = playerPos - m_position;
	float distToPlayer = toPlayer.Length();
	if (distToPlayer <= 1500)
	{
		toPlayer.Normalize();
		m_moveSpeed = toPlayer * 250.0f;
		m_moveSpeed.y = 0.0f;
	}
	else if (distToPlayer > 2000)
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
	if (distToPlayer <= 1500)
	{
		toPlayer.Normalize();
		m_rotation.SetRotationYFromDirectionXZ(toPlayer);
	}
	m_modelRender.SetRotation(m_rotation);
}

void FloorBoss::Attack()
{
	if (m_isAttack == false)
	{
		return;
	}
	else
	{
		OnCollision();
		m_isAttack = false;
	}
}

void FloorBoss::OnCollision()
{
	m_collisionObject = NewGO<CollisionObject>(0);

	Vector3 collisionPos = m_position;
	m_forward = Vector3::Front;
	m_rotation.Apply(m_forward);
	collisionPos += m_forward * 450.0f;
	m_collisionObject->CreateSphere(collisionPos, Quaternion::Identity, 200.0f);
	m_collisionObject->SetName("floorBossAttack");

	m_attackCollisionLife = 0.1f; // 攻撃の当たり判定の寿命を設定
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
			damage = m_player->GetAttackPower();
			int randomNum = rand() % 100 + 1;
			if (randomNum <= 5)
			{
				damage *= 2;
				bool isHit = m_player->GetAttackHit();
				if (!isHit)
				{
					m_player->SetAttackHit(true);

					m_audioManager->PlaySE(
						enSound_CriticalSE,
						1.0f,
						enSEPlay_AllowOverlap
					);
				}
				m_floorBossHP -= damage;
			}
			else
			{
				damage *= 1;
				bool isHit = m_player->GetAttackHit();
				if (!isHit)
				{
					m_player->SetAttackHit(true);
					int r = rand() % 3;

					AudioID id;

					switch (r)
					{
					case 0: id = enSound_PlayerAttackSE_01; break;
					case 1: id = enSound_PlayerAttackSE_02; break;
					case 2: id = enSound_PlayerAttackSE_03; break;
					}

					m_audioManager->PlaySE(
						id,
						1.0f,
						enSEPlay_AllowOverlap
					);
				}
				m_floorBossHP -= damage;
			}
			m_damageIntarvalTime = 1.5f;
			m_player->SetAttackHit(true);

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

void FloorBoss::AttackHit()
{
	const auto& collisions = g_collisionObjectManager->FindCollisionObjects("floorBossAttack");
	for (auto collision : collisions)
	{
		if (collision->IsHit(m_player->GetCharacterController()) == true)
		{
			int floorBossAttackPower = 0;
			floorBossAttackPower = GetAttackPower();
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

void FloorBoss::Death()
{
	if (m_floorBossHP <= 0)
	{
		m_game->EnemyCount();
		m_audioManager->PlaySE(enSound_EnemyDeathSE, 0.5f, enSEPlay_AllowOverlap);
		MakeExplosionEffect();
		int randomNum = rand() % 100 + 1;
		if (randomNum <= 20)
		{
			m_attackSpeedBuff = NewGO<AttackSpeedBuff>(0);
			m_attackSpeedBuff->SetPosition(m_position);
			m_audioManager->PlaySE(enSound_ItemDropSE, 0.5f, enSEPlay_AllowOverlap);
		}
		else if (randomNum > 20 && randomNum <= 40)
		{
			m_powerBuff = NewGO<PowerBuff>(0);
			m_powerBuff->SetPosition(m_position);
			m_audioManager->PlaySE(enSound_ItemDropSE, 0.5f, enSEPlay_AllowOverlap);
		}
		else if (randomNum > 40 && randomNum <= 60)
		{
			m_heal = NewGO<Heal>(0);
			m_heal->SetPosition(m_position);
			m_audioManager->PlaySE(enSound_ItemDropSE, 0.5f, enSEPlay_AllowOverlap);
		}
		DeleteGO(this);
	}
}

void FloorBoss::MakeExplosionEffect()
{
	//爆発エフェクトの生成
	Vector3 effectPos = m_position;
	effectPos.y += 70.0f;

	EffectManager::GetInstance().PlayEffect(
		EffectManager::enEffect_BossExplosion,
		effectPos,
		50.0f
	);
}

void FloorBoss::MakeNoticeCircleEffect()
{
	//予告円エフェクトの生成
	Vector3 effectPos = m_position;
	m_forward = Vector3::Front;
	m_rotation.Apply(m_forward);
	effectPos += m_forward * 450.0f;
	EffectManager::GetInstance().PlayEffect(
		EffectManager::enEffect_NoticeCircle,
		effectPos,
		200.0f
	);
}

void FloorBoss::ManageState()
{
	switch (m_floorBossState)
	{
	case enFloorBossState_Idle:
		IdleState();
		break;

	case enFloorBossState_Walk:
		WalkState();
		break;

	case enFloorBossState_Attack:
		AttackState();
		break;

	case enFloorBossState_Death:
		DeathState();
		break;

	default:
		break;
	}
}

void FloorBoss::FloorBossHP()
{
	float rate = (float)m_floorBossHP / (float)m_floorBossMaxHP;

	if (rate < 0.0f)
	{
		rate = 0.0f;
	}

	Vector3 scale = { 1.5f, 0.3f, 1.0f };
	scale.x *= rate;

	m_bossHPBar.SetScale(scale);

	if (m_floorBossHP <= m_floorBossMaxHP / 4)
	{
		m_bossHPBar.SetMulColor(g_vec4Red);
	}
	else
	{
		m_bossHPBar.SetMulColor(g_vec4Green);
	}

	m_bossHPBar.Update();

	float dist = (m_player->GetPosition() - m_position).Length();

	m_isShowHP = (dist <= 2000.0f);
}

void FloorBoss::PlayAnimation()
{
	switch (m_floorBossState)
	{
	case enFloorBossState_Idle:
		m_modelRender.PlayAnimation(enAnimationClip_Idle);
		break;
	case enFloorBossState_Walk:
		m_modelRender.PlayAnimation(enAnimationClip_Walk);
		break;
	case enFloorBossState_Death:
		m_modelRender.PlayAnimation(enAnimationClip_Death);
		break;
	default:
		break;
	}
}

void FloorBoss::FloorBossState()
{
	Vector3 playerPos = m_player->GetPosition();
	float dist = (playerPos - m_position).Length();

	if (m_floorBossHP <= 0)
	{
		m_floorBossState = enFloorBossState_Death;
	}
	else if (dist <= 500.0f &&
         m_timeCount <= 0.0f &&
         m_floorBossState != enFloorBossState_Attack)
{
    m_floorBossState = enFloorBossState_Attack;

    m_attackStateTimer = 0.8f;
    m_attackPhase = enAttackPhase_Warn;

    m_timeCount = m_attackIntervalTime;
	MakeNoticeCircleEffect();
}
	else if (fabsf(m_moveSpeed.x) >= 0.001f || fabsf(m_moveSpeed.z) >= 0.001f)
	{
		m_floorBossState = enFloorBossState_Walk;
	}
	else
	{
		m_floorBossState = enFloorBossState_Idle;
	}
}
void FloorBoss::IdleState()
{
	FloorBossState();
}

void FloorBoss::WalkState()
{
	FloorBossState();
}

void FloorBoss::AttackState()
{
	m_moveSpeed = Vector3::Zero;

	m_attackStateTimer -= g_gameTime->GetFrameDeltaTime();

	// ①予備動作（絶対にここで見せる）
	if (m_attackPhase == enAttackPhase_Warn)
	{
		// 視覚的合図をここで作る

		if (m_attackStateTimer <= 0.0f)
		{
			m_attackPhase = enAttackPhase_Active;
			OnCollision(); // ←ここで初めて攻撃発生
		}
		return;
	}

	// ②ヒットフェーズ
	if (m_attackPhase == enAttackPhase_Active)
	{
		if (m_attackStateTimer <= 0.0f)
		{
			m_attackPhase = enAttackPhase_End;
		}
		return;
	}

	// ③終了
	if (m_attackPhase == enAttackPhase_End)
	{
		m_floorBossState = enFloorBossState_Idle;
	}
}

void FloorBoss::DeathState()
{
	if(m_modelRender.IsPlayingAnimation() == false)
	{
		Death();
	}
}

void FloorBoss::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);
	if (m_isShowHP)
	{
		m_bossHPFrame.Draw(rc);
		m_bossHPBar.Draw(rc);
	}
}